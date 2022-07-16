/*
	CompassFile.cpp
	Wrapper class around a shared pointer to an ifstream. Here the shared pointer is used
	to overcome limitations of the ifstream class, namely that it is written such that ifstream
	cannot be modified by move semantics. Contains all information needed to parse a single binary
	CompassFile. Currently has a class wide defined buffer size; may want to make this user input
	in the future.

	Written by G.W. McCann Oct. 2020

	Modified for Specter; not really any significant changes. Just some simple changes, removal of unused data.

	GWM -- Feb 2022

	Update to reflect new CAEN binary data format with headers to indicate data contents.

	GWM -- May 2022
*/
#include "CompassFile.h"

namespace Specter {

	CompassFile::CompassFile() :
		m_filename(""), m_bufferIter(nullptr), m_bufferEnd(nullptr), m_smap(nullptr), m_hitUsedFlag(true), m_file(std::make_shared<std::ifstream>()), m_eofFlag(false)
	{
	}
	
	CompassFile::CompassFile(const std::string& filename) :
		m_filename(""), m_bufferIter(nullptr), m_bufferEnd(nullptr), m_smap(nullptr), m_hitUsedFlag(true), m_file(std::make_shared<std::ifstream>()), m_eofFlag(false)
	{
		Open(filename);
	}
	
	CompassFile::CompassFile(const std::string& filename, int bsize) :
		m_filename(""), m_bufferIter(nullptr), m_bufferEnd(nullptr), m_smap(nullptr), m_hitUsedFlag(true),
		m_bufsize(bsize), m_file(std::make_shared<std::ifstream>()), m_eofFlag(false)
	{
		Open(filename);
	}
	
	CompassFile::~CompassFile() 
	{
		Close();
	}
	
	void CompassFile::Open(const std::string& filename) 
	{
		SPEC_PROFILE_FUNCTION();
		m_eofFlag = false;
		m_hitUsedFlag = true;
		m_filename = filename;
		m_file->open(m_filename, std::ios::binary | std::ios::in);
	
		m_file->seekg(0, std::ios_base::end);
		m_size = (unsigned int)m_file->tellg();
		if(m_size == 2) 
		{
			m_eofFlag = true;
		} 
		else 
		{
			m_file->seekg(0, std::ios_base::beg);
			ReadHeader();
			m_nHits = m_size / m_hitsize;
			m_buffersize = m_hitsize * m_bufsize;
			m_hitBuffer.resize(m_buffersize);
		}
	}
	
	void CompassFile::Close() 
	{
		if(IsOpen()) 
			m_file->close();
	}
	
	void CompassFile::ReadHeader() 
	{
		if(!IsOpen()) 
		{
			SPEC_WARN("Unable to get hit size from file {0}, sending invalid value.", m_filename);
			return;
		}

		char* header = new char[2];
		m_file->read(header, 2);
		m_header = *((uint16_t*)header);
		m_hitsize = 16; //default hitsize 16 bytes
		if (Compass_IsEnergy(m_header))
			m_hitsize += 2;
		if (Compass_IsEnergyShort(m_header))
			m_hitsize += 2;
		if (Compass_IsEnergyCalibrated(m_header))
			m_hitsize += 8;
		if (Compass_IsWaves(m_header))
		{
			m_hitsize += 5;
			char* firstHit = new char[m_hitsize]; //Read chunk of first hit
			m_file->read(firstHit, m_hitsize);
			firstHit += m_hitsize - 4; //Move to the Nsamples value
			uint32_t nsamples = *((uint32_t*)firstHit);
			m_hitsize += nsamples * 2; //Each sample is two bytes
			m_file->seekg(0, std::ios_base::beg);
			m_file->read(header, 2);

			delete[] firstHit;
		}

		delete[] header;
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
		SPEC_PROFILE_FUNCTION();
		if(!IsOpen()) return true;
	
		if((m_bufferIter == nullptr || m_bufferIter == m_bufferEnd) && !IsEOF()) 
		{
			GetNextBuffer();
		}
	
		if(!IsEOF()) 
		{
			ParseNextHit();
			m_hitUsedFlag = false;
		}
	
		return m_eofFlag;
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
		SPEC_PROFILE_FUNCTION();
		if(m_file->eof())
		{
			m_eofFlag = true;
			return;
		}
	
		m_file->read(m_hitBuffer.data(), m_hitBuffer.size());
	
		m_bufferIter = m_hitBuffer.data();
		m_bufferEnd = m_bufferIter + m_file->gcount(); //one past the last datum
	
	}
	
	void CompassFile::ParseNextHit() 
	{
		SPEC_PROFILE_FUNCTION();
		m_currentHit.board = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.channel = *((uint16_t*)m_bufferIter);
		m_bufferIter += 2;
		m_currentHit.timestamp = *((uint64_t*)m_bufferIter);
		m_bufferIter += 8;
		if (Compass_IsEnergy(m_header))
		{
			m_currentHit.energy = *((uint16_t*)m_bufferIter);
			m_bufferIter += 2;
		}
		if (Compass_IsEnergyCalibrated(m_header))
		{
			m_currentHit.energyCalibrated = *((uint64_t*)m_bufferIter);
			m_bufferIter += 8;
		}
		if (Compass_IsEnergyShort(m_header))
		{
			m_currentHit.energyShort = *((uint16_t*)m_bufferIter);
			m_bufferIter += 2;
		}
		m_currentHit.flags = *((uint32_t*)m_bufferIter);
		m_bufferIter += 4;
		if (Compass_IsWaves(m_header))
		{
			m_currentHit.waveCode = *((uint8_t*)m_bufferIter);
			m_bufferIter += 1;
			m_currentHit.Ns = *((uint32_t*)m_bufferIter);
			m_bufferIter += 4;
			if (m_currentHit.samples.size() != m_currentHit.Ns)
				m_currentHit.samples.resize(m_currentHit.Ns);
			for (size_t i = 0; i < m_currentHit.samples.size(); i++)
			{
				m_currentHit.samples[i] = *((uint16_t*)m_bufferIter);
				m_bufferIter += 2;
			}
		}
		
		if(m_smap != nullptr) 
		{ //memory safety
			int gchan = m_currentHit.channel + m_currentHit.board*16;
			m_currentHit.timestamp += m_smap->GetShift(gchan);
		}
	
	}

}