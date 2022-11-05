#ifndef DY_FILE_SOURCE_H
#define DY_FILE_SOURCE_H

#include "DaqGrimoire.h"
#include "Specter/Physics/DataSource.h"
#include <filesystem>

namespace Specter {

	class DYFileSource : public DataSource
	{
	public:
		DYFileSource(const std::string& directory, uint64_t coincidenceWindow);
		virtual ~DYFileSource();

		virtual void ProcessData() override;
		virtual const std::vector<SpecEvent>& GetEvents() override
		{
			m_isEventReady = false;
			return m_eventBuilder.GetReadyEvents();
		}

	private:
		void CollectFiles();
		bool GetNextHit();

		std::filesystem::path m_directory;
		static constexpr std::string_view s_extension = ".dybin";

		std::vector<DaqGrimoire::DYFileReader> m_files;
		DaqGrimoire::DYListData m_dyHit;
		std::size_t m_startIndex;

		uint64_t m_totalDataHits;
	};
}

#endif