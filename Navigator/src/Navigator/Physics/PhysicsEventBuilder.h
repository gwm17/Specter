#ifndef PHYSICS_EVENT_BUILDER_H
#define PHYSICS_EVENT_BUILDER_H

#include "Navigator/NavCore.h"
#include "AnalysisStack.h"
#include "AnalysisStage.h"
#include "PhysicsHitSort.h"
#include "DataSource.h"
#include <mutex>
#include <atomic>

namespace Navigator {

	class PhysicsEventBuilder
	{
	public:
		PhysicsEventBuilder();
		virtual ~PhysicsEventBuilder();

		void Run();

		void AttachDataSource(const std::string& loc, DataSource::SourceType type);
		void DetachDataSource();

		void SetCoincidenceWindow(uint64_t window) { m_rawSort.SetCoincidenceWindow(window); }
		void PushStage(AnalysisStage* stage);
		bool IsRunning() { return m_runFlag; }

		inline static PhysicsEventBuilder& Get() { return *s_instance; }

	private:
		AnalysisStack m_physStack;
		std::atomic<bool> m_runFlag; //ensures defined read/write across threads
		static PhysicsEventBuilder* s_instance;
		PhysicsHitSort m_rawSort;

		std::mutex m_sourceLock;

		std::unique_ptr<DataSource> m_source;

	};

	PhysicsEventBuilder* CreatePhysicsEventBuilder();

}

#endif
