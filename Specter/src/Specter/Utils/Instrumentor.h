/*
	Instrumentor.h
	Instrumentation classes for generating JSON trace files for use in Google Chrome's tracer. Based entirely upon the work done by @TheCherno in his game engine series. See his Hazel
	repository for more details.

	To activate the profiling, switch the SPEC_PROFILE macro to a value greater than 0. By default the instrumentaion is disabled (duh). Should only be used for development cases.

	GWM -- May 2022
*/
#ifndef INSTRUMENTOR_H
#define INSTRUMENTOR_H

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

#include "Specter/Core/Logger.h"
#include "Timer.h"

namespace Specter {

	using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

	struct ProfileResult
	{
		std::string name;
		FloatingPointMicroseconds start;
		std::chrono::microseconds elapsedTime;
		std::thread::id threadID;
	};

	struct InstrumentationSession
	{
		std::string name;
	};

	class Instrumentor
	{
	public:
		Instrumentor(const Instrumentor&) = delete;
		Instrumentor(Instrumentor&&) = delete;

		void BeginSession(const std::string& name, const std::string filename="inst_results.json")
		{
			std::scoped_lock<std::mutex> guard(m_instMutex);

			if (m_session)
			{
				/*
				* Can only ever have one session rolling. If for whatever reason attempt to open second, handle gracefully
				*/
				if (Logger::GetLogger()) //Ensure logger exists
				{
					SPEC_ERROR("Attempting to create new instr. session ({0}) while session ({1}) is running! Setting to session {0}", name, m_session->name);
				}

				InternalEndSession();
			}

			m_outputFile.open(filename);
			if (m_outputFile.is_open())
			{
				m_session = new InstrumentationSession({ name });
				WriteHeader();
			}
			else if(Logger::GetLogger())
			{
				SPEC_ERROR("Unable to open instr. output file named {0}!", filename);
			}
		}

		void EndSession()
		{
			std::scoped_lock<std::mutex> guard(m_instMutex);
			InternalEndSession();
		}

		void WriteProfile(const ProfileResult& result)
		{
			std::scoped_lock<std::mutex> guard(m_instMutex);
			std::stringstream json_string;
			json_string << std::setprecision(3) << std::fixed;
			json_string << ",{";
			json_string << "\"cat\":\"function\",";
			json_string << "\"dur\":" << (result.elapsedTime.count()) << ',';
			json_string << "\"name\":\"" << result.name << "\",";
			json_string << "\"ph\":\"X\",";
			json_string << "\"pid\":0,";
			json_string << "\"tid\":" << result.threadID << ",";
			json_string << "\"ts\":" << result.start.count();
			json_string << "}";

			if (m_session)
			{
				m_outputFile << json_string.str();
				m_outputFile.flush();
			}
		}

		static Instrumentor& Get()
		{
			static Instrumentor s_instance;
			return s_instance;
		}

	private:
		Instrumentor() :
			m_session(nullptr)
		{

		}

		~Instrumentor()
		{
			EndSession();
		}

		void InternalEndSession()
		{
			if (m_session)
			{
				WriteFooter();
				m_outputFile.close();
				delete m_session;
				m_session = nullptr;
			}
		}

		void WriteHeader()
		{
			m_outputFile << "{\"otherData\": {},\"traceEvents\":[{}";
			m_outputFile.flush();
		}

		void WriteFooter()
		{
			m_outputFile << "]}";
			m_outputFile.flush();
		}

		std::mutex m_instMutex;
		InstrumentationSession* m_session;
		std::ofstream m_outputFile;
	};

	class InstrumentationTimer
	{
	public:
		InstrumentationTimer(const char* name) :
			m_name(name), m_stopped(false)
		{
			m_startTime = Clock::now();
		}

		~InstrumentationTimer()
		{
			if (!m_stopped)
				Stop();
		}


		void Stop()
		{
			auto stopTime = Clock::now();
			FloatingPointMicroseconds start = FloatingPointMicroseconds((m_startTime).time_since_epoch());
			auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(stopTime).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_startTime).time_since_epoch();
			m_stopped = true;

			Instrumentor::Get().WriteProfile({ m_name, start, elapsedTime, std::this_thread::get_id() });
		}

	private:
		using Time = std::chrono::steady_clock::time_point;
		using Clock = std::chrono::steady_clock;

		const char* m_name;
		Time m_startTime;
		bool m_stopped;
	};

	namespace InstrumentorUtils {

		template<size_t N>
		struct ChangeResult
		{
			char data[N];
		};

		template<size_t N, size_t M>
		constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[M])
		{
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t destIndex = 0;
			while (srcIndex < N)
			{
				size_t matchIndex = 0;
				while (matchIndex < M - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == M - 1)
					srcIndex += matchIndex;
				result.data[destIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}
			return result;
		}

	}
}

//Macros for ease of use

#define SPEC_PROFILE 0
#if SPEC_PROFILE

//Attempt to resolve function signature with precompiler
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define SPEC_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define SPEC_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define SPEC_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define SPEC_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define SPEC_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define SPEC_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define SPEC_FUNC_SIG __func__
#else
#define SPEC_FUNC_SIG "SPEC_FUNC_SIG unknown!"
#endif

#define SPEC_PROFILE_BEGIN_SESSION(name, filename) ::Specter::Instrumentor::Get().BeginSession(name, filename);
#define SPEC_PROFILE_END_SESSION() ::Specter::Instrumentor::Get().EndSession();
#define SPEC_PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::Specter::InstrumentorUtils::CleanupOutputString(name, "__cdecl ");\
											   ::Specter::InstrumentationTimer timer##line(fixedName##line.data)
#define SPEC_PROFILE_SCOPE_LINE(name, line) SPEC_PROFILE_SCOPE_LINE2(name, line)
#define SPEC_PROFILE_SCOPE(name) SPEC_PROFILE_SCOPE_LINE(name, __LINE__)
#define SPEC_PROFILE_FUNCTION() SPEC_PROFILE_SCOPE(SPEC_FUNC_SIG)

#else

#define SPEC_PROFILE_BEGIN_SESSION(name, filename)
#define SPEC_PROFILE_END_SESSION()
#define SPEC_PROFILE_SCOPE_LINE2(name, line)
#define SPEC_PROFILE_SCOPE_LINE(name, line)
#define SPEC_PROFILE_SCOPE(name)
#define SPEC_PROFILE_FUNCTION()

#endif

#endif
