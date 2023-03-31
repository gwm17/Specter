/*
	CompassRun.cpp
	Class designed as abstraction of a collection of binary files that represent the total data in a single
	Compass data run. It handles the user input (shift maps, file collection etc.) and creates a list of
	CompassFiles from which to draw data. It then draws data from these files, organizes them in time,
	and writes to a ROOT file for further processing.

	Written by G.W. McCann Oct. 2020

	Updated to also handle scaler data. -- GWM Oct. 2020

	Modifed and updated for use in Specter. Obviously stripped out any ROOT code. Also, now uses the very nice std::filesystem
	library to handle filepathing.

	GWM -- Feb 2022

	Update to reflect new CAEN binary data format with headers to indicate data contents.

	GWM -- May 2022

	Make it so that number of channels per board is no longer fixed. Use pairing function defined in Utils/Functions.h to generate a UUID for each board channel/pair.

	GWM -- Oct 2022
*/
#include "CompassRun.h"

namespace Specter {
	
	CompassRun::CompassRun(const std::string& dir, uint64_t coincidenceWindow) :
		DataSource(coincidenceWindow), m_directory(dir), m_startIndex(0)
	{
		CollectFiles();
	}
	
	CompassRun::~CompassRun() {}
	
	void CompassRun::CollectFiles()
	{
		SPEC_PROFILE_FUNCTION();
		int nfiles=0;
		for(auto& item : std::filesystem::directory_iterator(m_directory))
		{
			if(item.path().extension() == m_extension)
				nfiles++;
		}

		m_datafiles.clear();
		m_datafiles.reserve(nfiles);
		for(auto& item : std::filesystem::directory_iterator(m_directory))
		{
			if(item.path().extension() == m_extension)
			{
				m_datafiles.emplace_back(item.path().string());
			}
		}

		long total_hits=0;
		for(auto& file : m_datafiles)
		{
			
			if(!file.IsOpen())
			{
				SPEC_ERROR("Unable to open file with name {0}", file.GetName());
				m_validFlag = false;
				return;
			}

			if(m_smap.IsValid())
				file.AttachShiftMap(&m_smap);

			total_hits += file.GetNumberOfHits();
		}

		if(m_datafiles.size() == 0)
		{
			SPEC_WARN("Unable to find any files with extension {0} in directory {1}. CompassRun killed.", m_extension, m_directory);
			m_validFlag = false;
		}
		else
		{
			SPEC_INFO("Succesfully opened {0} files with {1} total hits", nfiles, total_hits);
			m_validFlag = true;
		}
	}

	/*
		GetHitsFromFiles() is the function which actually retrieves and sorts the data from the individual
		files. There are several tricks which allow this to happen. First is that, after sorting, it is impossible
		to determine which file the data originally came from (short of parsing the name of the file against board/channel).
		However, we need to let the file know that we want it to pull the next hit. To do this, a pointer to the UsedFlag of the file
		is retrieved along with the data. This flag is flipped so that on the next hit cycle a new hit is pulled. Second is the use
		of a rolling start index. Once a file has gone EOF, we no longer need it. If this is the first file in the list, we can just skip
		that index all together. In this way, the loop can go from N times to N-1 times.
	*/
	bool CompassRun::GetHitsFromFiles() 
	{
		SPEC_PROFILE_FUNCTION();
		std::pair<CompassHit, bool*> earliestHit = std::make_pair(CompassHit(), nullptr);
		for(unsigned int i=m_startIndex; i<m_datafiles.size(); i++) 
		{
			if(m_datafiles[i].CheckHitHasBeenUsed()) 
				m_datafiles[i].GetNextHit();
	
			if(m_datafiles[i].IsEOF()) 
			{
				if(i == m_startIndex)
					m_startIndex++;
				continue;
			} 
			else if(i == m_startIndex) 
			{
				earliestHit = std::make_pair(m_datafiles[i].GetCurrentHit(), m_datafiles[i].GetUsedFlagPtr());
			} 
			else if(m_datafiles[i].GetCurrentHit().timestamp < earliestHit.first.timestamp) 
			{
				earliestHit = std::make_pair(m_datafiles[i].GetCurrentHit(), m_datafiles[i].GetUsedFlagPtr());
			}
		}
	
		if(earliestHit.second == nullptr) 
			return false; //Make sure that there actually was a hit
		m_hit = earliestHit.first;
		*earliestHit.second = true;
		return true;
	}

	void CompassRun::ProcessData()
	{
		SPEC_PROFILE_FUNCTION();
		if(!IsValid())
		{
			SPEC_ERROR("Trying to access CompassRun data when invalid, bug detected!");
			return;
		}

		if (!GetHitsFromFiles())
		{
			m_validFlag = false;
			return;
		}
		
		//Convert data from CoMPASS format to universal Specter format.
		m_datum.longEnergy = m_hit.energy;
		m_datum.shortEnergy = m_hit.energyShort;
		m_datum.calEnergy = m_hit.energyCalibrated;
		m_datum.timestamp = m_hit.timestamp;
		m_datum.id = Utilities::GetBoardChannelUUID(m_hit.board, m_hit.channel);

		m_eventBuilder.AddDatum(m_datum);
	}

}