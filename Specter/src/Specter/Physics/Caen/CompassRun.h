/*
	CompassRun.h
	Class designed as abstraction of a collection of binary files that represent the total data in a single
	Compass data run. It handles the user input (shift maps, file collection etc.) and creates a list of
	CompassFiles from which to draw data. It then draws data from these files, organizes them in time,
	and writes to a ROOT file for further processing.

	Written by G.W. McCann Oct. 2020

	Modifed and updated for use in Specter. Obviously stripped out any ROOT code. Also, now uses the very nice std::filesystem
	library to handle filepathing.

	GWM -- Feb 2022

	Update to reflect new CAEN binary data format with headers to indicate data contents.

	GWM -- May 2022

	Make it so that number of channels per board is no longer fixed. Use pairing function defined in Utils/Functions.h to generate a UUID for each board channel/pair.

	GWM -- Oct 2022
*/
#ifndef COMPASSRUN_H
#define COMPASSRUN_H

#include "Specter/Core/SpecCore.h"
#include "Specter/Physics/DataSource.h"
#include "CompassFile.h"
#include "Specter/Physics/ShiftMap.h"
#include <filesystem>

namespace Specter {
	
	class CompassRun : public DataSource
	{
	public:
		CompassRun(const std::string& dir, uint64_t coincidenceWindow);
		virtual ~CompassRun();
		virtual void ProcessData() override;
		virtual const std::vector<SpecEvent>& GetEvents() override
		{
			m_isEventReady = false;
			return m_eventBuilder.GetReadyEvents();
		}
		inline void SetDirectory(const std::string& dir) { m_directory = dir; CollectFiles(); }
		inline void SetShiftMap(const std::string& filename) { m_smap.SetFile(filename); }
	
	
	private:
		void CollectFiles();
		bool GetHitsFromFiles();
	
		std::filesystem::path m_directory;
		const std::string m_extension = ".BIN";

		std::vector<CompassFile> m_datafiles;
		unsigned int m_startIndex; //this is the file we start looking at; increases as we finish files.

		ShiftMap m_smap;

		CompassHit m_hit;
	
		unsigned int m_totalHits;

	};

}

#endif
