/*
	CompassRun.h
	Class designed as abstraction of a collection of binary files that represent the total data in a single
	Compass data run. It handles the user input (shift maps, file collection etc.) and creates a list of
	CompassFiles from which to draw data. It then draws data from these files, organizes them in time,
	and writes to a ROOT file for further processing.

	Written by G.W. McCann Oct. 2020

	Modifed and updated for use in Navigator. Obviously stripped out any ROOT code. Also, now uses the very nice std::filesystem
	library to handle filepathing. One change of great import: Navigator wants a unqiue ID on each hit. To do this we use the idiom:
	id = board_number * nchannels_per_board + channel_number
	This requires two things: that the class variable m_nchannels_per_board be set to match your physical digitizers, and that ALL of your
	digitizers have the SAME number of channels. By default CompassRun assumes 16 channels per board, as this is what is used with the SE-SPS setup at FoxLab.
	If you use a different set of boards, CHANGE THIS VALUE! If you use mixed boards, you will need to invent a new id scheme altogether.

	GWM -- Feb 2022

	Update to reflect new CAEN binary data format with headers to indicate data contents.

	GWM -- May 2022
*/
#ifndef COMPASSRUN_H
#define COMPASSRUN_H

#include "Navigator/Core/NavCore.h"
#include "Navigator/Physics/DataSource.h"
#include "CompassFile.h"
#include "Navigator/Physics/ShiftMap.h"
#include <filesystem>

namespace Navigator {
	
	class NAV_API CompassRun : public DataSource
	{
	
	public:
		CompassRun();
		CompassRun(const std::string& dir, int channels_per_board=16);
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
		unsigned int m_startIndex; //this is the file we start looking at; increases as we finish files.
		int m_nchannels_per_board; //IMPORTANT: Used for ID'ing channels uniquely. If you use boards with 32 or 8 or 64 channels you must change this! If you mix boards with
										//different numbers of channels, you will have to find a different id solution.
		ShiftMap m_smap;

		CompassHit m_hit;
	
		unsigned int m_totalHits;
	};

}

#endif
