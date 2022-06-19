/*
	ShiftMap.h
	New class to act a go-between for timestamp shifts to channels. Takes in a
	formated file containing data for shifts and then stores them in an unordered_map.
	Key is a global compass channel (board#*16 + channel). Shifts in ps.

	Written by G.W. McCann Oct. 2020

	Not currently implemented for Specter though it could still be useful. Leave this here as a maybe upgrade path.

	GWM -- Feb 2022
*/
#include "ShiftMap.h"

namespace Specter {

	ShiftMap::ShiftMap() :
		m_filename(""), m_validFlag(false)
	{
	}
	
	ShiftMap::ShiftMap(const std::string& filename) :
		m_filename(filename), m_validFlag(false)
	{
		ParseFile();
	}
	
	ShiftMap::~ShiftMap() {}
	
	void ShiftMap::SetFile(const std::string& filename) 
	{
		m_filename = filename;
		ParseFile();
	}
	
	uint64_t ShiftMap::GetShift(int gchan) 
	{
		if(!m_validFlag)
			return 0;
		
		auto iter = m_map.find(gchan);
		if(iter == m_map.end())
			return 0;
		else 
			return iter->second;
	}
	
	void ShiftMap::ParseFile() 
	{
		m_validFlag = false;
		std::ifstream input(m_filename);
		if(!input.is_open()) 
			return;
	
		int board, channel, gchan;
		uint64_t shift;
		std::string junk, temp;
	
		std::getline(input, junk);
		std::getline(input, junk);
	
		while(input>>board) 
		{
			input>>temp;
			input>>shift;
			if(temp == "all") //keyword to set all channels in this board to same shift
			{ 
				for(int i=0; i<16; i++) 
				{
					gchan = board*16 + i;
					m_map[gchan] = shift;
				}
			}
			else 
			{
				channel = stoi(temp);
				gchan = channel + board*16;
				m_map[gchan] = shift;
			}
		}
	
		m_validFlag = true;
	}

}