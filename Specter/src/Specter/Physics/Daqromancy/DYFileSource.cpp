#include "DYFileSource.h"

namespace Specter {

	DYFileSource::DYFileSource(const std::string& directory, uint64_t coicidenceWindow) :
		DataSource(coicidenceWindow), m_directory(directory)
	{
		CollectFiles();
	}

	DYFileSource::~DYFileSource()
	{
	}

	void DYFileSource::CollectFiles()
	{
		SPEC_PROFILE_FUNCTION();
		int nfiles = 0;
		for (auto& item : std::filesystem::directory_iterator(m_directory))
		{
			if (item.path().extension() == s_extension)
				nfiles++;
		}

		m_files.clear();
		m_files.reserve(nfiles);
		for (auto& item : std::filesystem::directory_iterator(m_directory))
		{
			if (item.path().extension() == s_extension)
			{
				m_files.emplace_back(item.path().string());
			}
		}

		long total_hits = 0;
		for (auto& file : m_files)
		{

			if (!file.IsOpen())
			{
				SPEC_ERROR("Unable to open a DYFile file {0}", file.GetFilePath());
				m_validFlag = false;
				return;
			}

			total_hits += file.GetFileSizeEvents();
		}

		if (m_files.size() == 0)
		{
			SPEC_WARN("Unable to find any files with extension {0} in directory {1}. CompassRun killed.", s_extension, m_directory);
			m_validFlag = false;
		}
		else
		{
			SPEC_INFO("Succesfully opened {0} files with {1} total hits", nfiles, total_hits);
			m_validFlag = true;
		}
	}

	bool DYFileSource::GetNextHit()
	{
		SPEC_PROFILE_FUNCTION();
		DaqGrimoire::DYFileReader* earliestFile = nullptr;

		for (std::size_t i = m_startIndex; i < m_files.size(); i++)
		{
			if (m_files[i].IsHitUsed())
				m_files[i].ReadNextEvent();

			if (m_files[i].IsEOF())
			{
				if (i == m_startIndex)
					m_startIndex++;
				continue;
			}
			else if (i == m_startIndex || m_files[i].GetCurrentEvent().timestamp < earliestFile->GetCurrentEvent().timestamp)
			{
				earliestFile = &(m_files[i]);
			}
		}

		if (earliestFile == nullptr)
			return false;

		m_dyHit = earliestFile->GetCurrentEvent();
		earliestFile->SetHitUsed();
		return true;
	}

	void DYFileSource::ProcessData()
	{
		if (!IsValid())
		{
			SPEC_ERROR("Trying to access DYFileSource data when invalid, bug detected!");
			return;
		}

		if (!GetNextHit())
		{
			m_validFlag = false;
		}
		//Convert data from Daqromancy format to universal Specter format.
		m_datum.longEnergy = m_dyHit.energy;
		m_datum.shortEnergy = m_dyHit.energyShort;
		m_datum.timestamp = m_dyHit.timestamp;
		m_datum.id = Utilities::GetBoardChannelUUID(m_dyHit.board, m_dyHit.channel);
		if(m_eventBuilder.AddDatum(m_datum))
		{
			m_isEventReady = true;
		}
	}
}