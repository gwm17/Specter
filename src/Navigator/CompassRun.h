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

#include "CompassFile.h"
#include "ShiftMap.h"

namespace Navigator {
	
	class CompassRun 
	{
	
	public:
		CompassRun();
		CompassRun(const std::string& dir);
		~CompassRun();
		inline void SetDirectory(const std::string& dir) { m_directory = dir; }
		inline void SetRunNumber(int n) { m_runNum = n; }
		inline void SetShiftMap(const std::string& filename) { m_smap.SetFile(filename); }
	
	
	private:
		bool GetHitsFromFiles();
	
		std::string m_directory;
		std::vector<CompassFile> m_datafiles;
		unsigned int startIndex; //this is the file we start looking at; increases as we finish files.
		ShiftMap m_smap;
	
		//Potential branch variables
		CompassHit hit;
	
		//what run is this
		int m_runNum;
		unsigned int m_totalHits;
	
		//Scaler switch
	};

}

#endif
