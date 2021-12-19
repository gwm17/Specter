/*
	CompassFile.cpp
	Wrapper class around a shared pointer to an ifstream. Here the shared pointer is used
	to overcome limitations of the ifstream class, namely that it is written such that ifstream
	cannot be modified by move semantics. Contains all information needed to parse a single binary
	CompassFile. Currently has a class wide defined buffer size; may want to make this user input
	in the future.

	Written by G.W. McCann Oct. 2020
*/
#include <navpch.h>
#include "CompassFile.h"

namespace Navigator {

	CompassFile::CompassFile() :
		m_filename(""), bufferIter(nullptr), bufferEnd(nullptr), m_smap(nullptr), hitUsedFlag(true), m_file(std::make_shared<std::ifstream>()), eofFlag(false)
	{
		m_buffersize = bufsize*hitsize;
		hitBuffer.resize(m_buffersize);
	}
	
	CompassFile::CompassFile(const std::string& filename) :
		m_filename(""), bufferIter(nullptr), bufferEnd(nullptr), m_smap(nullptr), hitUsedFlag(true), m_file(std::make_shared<std::ifstream>()), eofFlag(false)
	{
		m_buffersize = bufsize*hitsize;
		hitBuffer.resize(m_buffersize);
		Open(filename);
	}
	
	CompassFile::CompassFile(const std::string& filename, int bsize) :
		m_filename(""), bufferIter(nullptr), bufferEnd(nullptr), m_smap(nullptr), hitUsedFlag(true),
		bufsize(bsize), m_file(std::make_shared<std::ifstream>()), eofFlag(false)
	{
		m_buffersize = bufsize*hitsize;
		hitBuffer.resize(m_buffersize);
		Open(filename);
	}
	
	CompassFile::~CompassFile() 
	{
		Close();
	}
	
	void CompassFile::Open(const std::string& filename) 
	{
		eofFlag = false;
		hitUsedFlag = true;
		m_filename = filename;
		m_file->open(m_filename, std::ios::binary | std::ios::in);
	
		m_file->seekg(0, std::ios_base::end);
		m_size = m_file->tellg();
		m_nHits = m_size/24;
		if(m_size == 0) 
		{
			eofFlag = true;
		} 
		else 
		{
			m_file->seekg(0, std::ios_base::beg);
		}
	}
	
	void CompassFile::Close() 
	{
		if(IsOpen()) 
		{
			m_file->close();
		}
	}
	
	int CompassFile::GetHitSize() 
	{
		if(!IsOpen()) 
		{
			NAV_WARN("Unable to get hit size from file {0}, sending invalid value.", m_filename);
			return 0;
		}
	
		char* firstHit = new char[24]; //A compass hit by default has 24 bytes (at least in our setup)
	
		m_file->read(firstHit, 24);
	
		firstHit += 16;
		int nsamples = *((uint32_t*) firstHit);
	
		m_file->seekg(0, std::ios_base::beg);
	
		delete[] firstHit;
	
		return 24 + nsamples*16; 
	
	}
	
	/*
		GetNextHit() is the function which... gets the next hit
		Has to check if the buffer needs refilled/filled for the first time
		Upon pulling a hit, sets the UsedFlag to false, letting the next level know
		that the hit should be free game.
	
		If the file cannot be opened, signals as though file is EOF
	*/
	bool CompassFile::GetNextHit()
	{
		if(!IsOpen()) return true;
	
		if((bufferIter == nullptr || bufferIter == bufferEnd) && !IsEOF()) 
		{
			GetNextBuffer();
		}
	
		if(!IsEOF()) 
		{
			ParseNextHit();
			hitUsedFlag = false;
		}
	
		return eofFlag;
	}
	
	/*
		GetNextBuffer() ... self-explanatory name
		Note tht this is where the EOF flag is set. The EOF is only singaled
		after the LAST buffer is completely read (i.e literally no more data). ifstream sets its eof 
		bit upon pulling the last buffer, but this class waits until that entire
		last buffer is read to singal EOF (the true end of file). 
	*/
	void CompassFile::GetNextBuffer() 
	{
	
		if(m_file->eof()) 
		{
			eofFlag = true;
			return;
		}
	
		m_file->read(hitBuffer.data(), hitBuffer.size());
	
		bufferIter = hitBuffer.data();
		bufferEnd = bufferIter + m_file->gcount(); //one past the last datum
	
	}
	
	void CompassFile::ParseNextHit() 
	{
	
		m_currentHit.board = *((uint16_t*)bufferIter);
		bufferIter += 2;
		m_currentHit.channel = *((uint16_t*)bufferIter);
		bufferIter += 2;
		m_currentHit.timestamp = *((uint64_t*)bufferIter);
		bufferIter += 8;
		m_currentHit.lgate = *((uint16_t*)bufferIter);
		bufferIter += 2;
		m_currentHit.sgate = *((uint16_t*)bufferIter);
		bufferIter += 2;
		m_currentHit.flags = *((uint32_t*)bufferIter);
		bufferIter += 4;
		m_currentHit.Ns = *((uint32_t*)bufferIter);
		bufferIter += 4;
	
		if(m_smap != nullptr) 
		{ //memory safety
			int gchan = m_currentHit.channel + m_currentHit.board*16;
			m_currentHit.timestamp += m_smap->GetShift(gchan);
		}
	
	}

}