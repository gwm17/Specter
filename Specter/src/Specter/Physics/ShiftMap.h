/*
	ShiftMap.h
	New class to act a go-between for timestamp shifts to channels. Takes in a
	formated file containing data for shifts and then stores them in an unordered_map.
	Key is a global compass channel (board#*16 + channel). Shifts in ps.

	Written by G.W. McCann Oct. 2020

	Not currently implemented for Specter though it could still be useful. Leave this here as a maybe upgrade path.

	GWM -- Feb 2022
*/
#ifndef SHIFTMAP_H
#define SHIFTMAP_H

#include "Specter/Core/SpecCore.h"

namespace Specter {

	class ShiftMap 
	{
	public:
		ShiftMap();
		ShiftMap(const std::string& filename);
		~ShiftMap();
		void SetFile(const std::string& filename);
		bool IsValid() { return m_validFlag; }
		std::string GetFilename() { return m_filename; }
		uint64_t GetShift(int gchan);
	
	private:
		void ParseFile();
	
		std::string m_filename;
		bool m_validFlag;
	
		std::unordered_map<int, uint64_t> m_map;
	
	};

}

#endif
