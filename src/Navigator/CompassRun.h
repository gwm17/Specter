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
#include "DataStructs.h"
#include "RunCollector.h"
#include "ShiftMap.h"
#include "ProgressCallback.h"
#include <TParameter.h>

namespace Navigator {
	
	class CompassRun 
	{
	
	public:
		CompassRun();
		CompassRun(const std::string& dir);
		~CompassRun();
		inline void SetDirectory(const std::string& dir) { m_directory = dir; }
		inline void SetScalerInput(const std::string& filename) { m_scalerinput = filename; }
		inline void SetRunNumber(int n) { m_runNum = n; }
		inline void SetShiftMap(const std::string& filename) { m_smap.SetFile(filename); }
		void Convert2RawRoot(const std::string& name);
		void Convert2SortedRoot(const std::string& name, const std::string& mapfile, double window);
		void Convert2FastSortedRoot(const std::string& name, const std::string& mapfile, double window, double fsi_window, double fic_window);
		void Convert2SlowAnalyzedRoot(const std::string& name, const std::string& mapfile, double window,
								  int zt, int at, int zp, int ap, int ze, int ae, double bke, double b, double theta);
		void Convert2FastAnalyzedRoot(const std::string& name, const std::string& mapfile, double window, double fsi_window, double fic_window,
								  int zt, int at, int zp, int ap, int ze, int ae, double bke, double b, double theta);
	
		inline void SetProgressCallbackFunc(const ProgressCallbackFunc& function) { m_progressCallback = function; }
		inline void SetProgressFraction(double frac) { m_progressFraction = frac; }
	
	private:
		bool GetBinaryFiles();
		bool GetHitsFromFiles();
		void SetScalers();
		void ReadScalerData(const std::string& filename);
	
		std::string m_directory, m_scalerinput;
		std::vector<CompassFile> m_datafiles;
		unsigned int startIndex; //this is the file we start looking at; increases as we finish files.
		ShiftMap m_smap;
		std::unordered_map<std::string, TParameter<Long64_t>> m_scaler_map; //maps scaler files to the TParameter to be saved
	
		//Potential branch variables
		CompassHit hit;
		CoincEvent event;
		ProcessedEvent pevent;
	
		//what run is this
		int m_runNum;
		unsigned int m_totalHits;
	
		//Scaler switch
		bool m_scaler_flag;
	
		ProgressCallbackFunc m_progressCallback;
		double m_progressFraction;
	};

}

#endif
