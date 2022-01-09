/*
	CompassFile.h
	Wrapper class around a shared pointer to an ifstream. Here the shared pointer is used
	to overcome limitations of the ifstream class, namely that it is written such that ifstream
	cannot be modified by move semantics. Contains all information needed to parse a single binary
	CompassFile. Currently has a class wide defined buffer size; may want to make this user input
	in the future.

	Written by G.W. McCann Oct. 2020
*/
#ifndef COMPASSFILE_H
#define COMPASSFILE_H

#include "Navigator/NavCore.h"
#include "CompassHit.h"
#include "ShiftMap.h"

namespace Navigator {

	class NAV_API CompassFile 
	{
		
	public:
		CompassFile();
		CompassFile(const std::string& filename);
		CompassFile(const std::string& filename, int bsize);
		~CompassFile();
		void Open(const std::string& filename);
		void Close();
		bool GetNextHit();
	
		inline bool IsOpen() const { return m_file->is_open(); };
		inline CompassHit GetCurrentHit() const { return m_currentHit; }
		inline std::string GetName() const { return  m_filename; }
		inline bool CheckHitHasBeenUsed() const { return hitUsedFlag; } //query to find out if we've used the current hit
		inline void SetHitHasBeenUsed() { hitUsedFlag = true; } //flip the flag to indicate the current hit has been used
		inline bool IsEOF() const { return eofFlag; } //see if we've read all available data
		inline bool* GetUsedFlagPtr() { return &hitUsedFlag; }
		inline void AttachShiftMap(ShiftMap* map) { m_smap = map; }
		inline unsigned int GetSize() const { return m_size; }
		inline unsigned int GetNumberOfHits() const { return m_nHits; }
	
	
	private:
		int GetHitSize();
		void ParseNextHit();
		void GetNextBuffer();
	
		using Buffer = std::vector<char>;
	
		using FilePointer = std::shared_ptr<std::ifstream>; //to make this class copy/movable
	
		std::string m_filename;
		Buffer hitBuffer;
		char* bufferIter;
		char* bufferEnd;
		ShiftMap* m_smap; //NOT owned by CompassFile. DO NOT delete
	
		bool hitUsedFlag;
		int bufsize = 200000; //size of the buffer in hits
		int hitsize = 24; //size of a CompassHit in bytes (without alignment padding)
		int m_buffersize;
	
		CompassHit m_currentHit;
		FilePointer m_file;
		bool eofFlag;
		unsigned int m_size; //size of the file in bytes
		unsigned int m_nHits; //number of hits in the file (m_size/24)
	
	};

}

#endif
