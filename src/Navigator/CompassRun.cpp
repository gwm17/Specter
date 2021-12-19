/*
	CompassRun.cpp
	Class designed as abstraction of a collection of binary files that represent the total data in a single
	Compass data run. It handles the user input (shift maps, file collection etc.) and creates a list of
	CompassFiles from which to draw data. It then draws data from these files, organizes them in time,
	and writes to a ROOT file for further processing.

	Written by G.W. McCann Oct. 2020

	Updated to also handle scaler data. -- GWM Oct. 2020
*/
#include <navpch.h>
#include "CompassRun.h"

namespace Navigator {
	
	CompassRun::CompassRun() :
		m_directory("")
	{
	}
	
	CompassRun::CompassRun(const std::string& dir) :
		m_directory(dir)
	{
	
	}
	
	CompassRun::~CompassRun() {}
	
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
	
		std::pair<CompassHit, bool*> earliestHit = std::make_pair(CompassHit(), nullptr);
		for(unsigned int i=startIndex; i<m_datafiles.size(); i++) 
		{
			if(m_datafiles[i].CheckHitHasBeenUsed()) 
				m_datafiles[i].GetNextHit();
	
			if(m_datafiles[i].IsEOF()) 
			{
				if(i == startIndex)
					startIndex++;
				continue;
			} 
			else if(i == startIndex) 
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
		hit = earliestHit.first;
		*earliestHit.second = true;
		return true;
	}

}