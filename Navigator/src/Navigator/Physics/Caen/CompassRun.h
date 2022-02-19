/*
	CompassRun.h
	Class designed as abstraction of a collection of binary files that represent the total data in a single
	Compass data run. It handles the user input (shift maps, file collection etc.) and creates a list of
	CompassFiles from which to draw data. It then draws data from these files, organizes them in time,
	and writes to a ROOT file for further processing.

	Written by G.W. McCann Oct. 2020
*/
#ifndef COMPASSRUN_H
#define COMPASSRUN_H

#include "Navigator/NavCore.h"
#include "Navigator/Physics/DataSource.h"
#include "CompassFile.h"
#include "Navigator/Physics/ShiftMap.h"
#include <filesystem>

namespace Navigator {
	
	class NAV_API CompassRun : public DataSource
	{
	
	public:
		CompassRun();
		CompassRun(const std::string& dir);
		virtual ~CompassRun();
		virtual const NavData& GetData() override;
		inline void SetDirectory(const std::string& dir) { m_directory = dir; CollectFiles(); }
		inline void SetShiftMap(const std::string& filename) { m_smap.SetFile(filename); }
	
	
	private:
		void CollectFiles();
		bool GetHitsFromFiles();
	
		std::filesystem::path m_directory;
		const std::string m_extension = ".bin";

		std::vector<CompassFile> m_datafiles;
		unsigned int startIndex; //this is the file we start looking at; increases as we finish files.

		ShiftMap m_smap;

		CompassHit m_hit;
	
		unsigned int m_totalHits;
	};

}

#endif
