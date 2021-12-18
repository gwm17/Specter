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
#include "RunCollector.h"
#include "SlowSort.h"
#include "FastSort.h"
#include "SFPAnalyzer.h"
#include "FlagHandler.h"

namespace Navigator {
	
	CompassRun::CompassRun() :
		m_directory(""), m_scalerinput(""), m_runNum(0), m_scaler_flag(false), m_progressFraction(0.1)
	{
	}
	
	CompassRun::CompassRun(const std::string& dir) :
		m_directory(dir), m_scalerinput(""), m_runNum(0), m_scaler_flag(false), m_progressFraction(0.1)
	{
	
	}
	
	CompassRun::~CompassRun() {}
	
	
	/*Load em into a map*/
	void CompassRun::SetScalers() 
	{
		std::ifstream input(m_scalerinput);
		if(!input.is_open()) 
			return;
	
		m_scaler_flag = true;
		std::string junk, filename, varname;
		Long64_t init = 0;
		std::getline(input, junk);
		std::getline(input, junk);
		m_scaler_map.clear();
		while(input>>filename) 
		{
			input>>varname;
			filename = m_directory+filename+"_run_"+std::to_string(m_runNum)+".bin";
			m_scaler_map[filename] = TParameter<Long64_t>(varname.c_str(), init);
		}
		input.close();
	}
	
	bool CompassRun::GetBinaryFiles() 
	{
		std::string prefix = "";
		std::string suffix = ".bin"; //binaries
		RunCollector grabber(m_directory, prefix, suffix);
		grabber.GrabAllFiles();
	
		m_datafiles.clear(); //so that the CompassRun can be reused
		m_datafiles.reserve(grabber.GetFileList().size());
		bool scalerd;
		m_totalHits = 0; //reset total run size
	
		for(auto& entry : grabber.GetFileList()) 
		{
			//Handle scaler files, if they exist
			if(m_scaler_flag) 
			{
				scalerd = false;
				for(auto& scaler_pair : m_scaler_map) 
				{
					if(entry == scaler_pair.first) 
					{
						ReadScalerData(entry);
						scalerd = true;
						break;
					}
				}
				if(scalerd) 
					continue;
			}
	
			m_datafiles.emplace_back(entry);
			m_datafiles[m_datafiles.size()-1].AttachShiftMap(&m_smap);
			//Any time we have a file that fails to be found, we terminate the whole process
			if(!m_datafiles[m_datafiles.size() - 1].IsOpen()) 
				return false;
	
			m_totalHits += m_datafiles[m_datafiles.size()-1].GetNumberOfHits();
		}
	
		return true;
	}
	
	/*
		Pure counting of scalers. Potential upgrade path to something like
		average count rate etc. 
	*/
	void CompassRun::ReadScalerData(const std::string& filename) 
	{
		if(!m_scaler_flag) 
			return;
	
		Long64_t count;
		count = 0;
		CompassFile file(filename);
		auto& this_param = m_scaler_map[file.GetName()];
		while(true) 
		{
			file.GetNextHit();
			if(file.IsEOF()) 
				break;
			count++;
		}
		this_param.SetVal(count);
	}
	
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
	
	void CompassRun::Convert2RawRoot(const std::string& name) {
		TFile* output = TFile::Open(name.c_str(), "RECREATE");
		TTree* outtree = new TTree("Data", "Data");
	
		outtree->Branch("Board", &hit.board);
		outtree->Branch("Channel", &hit.channel);
		outtree->Branch("Energy", &hit.lgate);
		outtree->Branch("EnergyShort", &hit.sgate);
		outtree->Branch("Timestamp", &hit.timestamp);
		outtree->Branch("Flags", &hit.flags);
	
		if(!m_smap.IsValid()) 
		{
			EVB_WARN("Bad shift map ({0}) at CompassRun::Convert(), shifts all set to 0.", m_smap.GetFilename());
		}
	
		SetScalers();
	
		if(!GetBinaryFiles()) 
		{
			EVB_ERROR("Unable to find binary files at CompassRun::Convert(), exiting!");
			return;
		}
	
		unsigned int count = 0, flush = m_totalHits*m_progressFraction, flush_count = 0;
	
		startIndex = 0; //Reset the startIndex
		if(flush == 0) 
			flush = 1;
		while(true) 
		{
			count++;
			if(count == flush) 
			{ //Progress Log
				count = 0;
				flush_count++;
				m_progressCallback(flush_count*flush, m_totalHits);
			}
	
			if(!GetHitsFromFiles()) 
				break;
			outtree->Fill();
		}
	
		output->cd();
		outtree->Write(outtree->GetName(), TObject::kOverwrite);
		for(auto& entry : m_scaler_map)
			entry.second.Write();
	
		output->Close();
	}
	
	void CompassRun::Convert2SortedRoot(const std::string& name, const std::string& mapfile, double window) 
	{
		TFile* output = TFile::Open(name.c_str(), "RECREATE");
		TTree* outtree = new TTree("SortTree", "SortTree");
	
		outtree->Branch("event", &event);
	
		if(!m_smap.IsValid()) 
		{
			EVB_WARN("Bad shift map ({0}) at CompassRun::Convert2SortedRoot(), shifts all set to 0.", m_smap.GetFilename());
		}
	
		SetScalers();
	
		if(!GetBinaryFiles()) 
		{
			EVB_ERROR("Unable to find binary files at CompassRun::Convert2SortedRoot(), exiting!");
			return;
		}
	
		unsigned int count = 0, flush = m_totalHits*m_progressFraction, flush_count = 0;
	
		startIndex = 0;
		SlowSort coincidizer(window, mapfile);
		bool killFlag = false;
		if(flush == 0) 
			flush = 1;
		while(true) 
		{
			count++;
			if(count == flush) 
			{
				count = 0;
				flush_count++;
				m_progressCallback(flush_count*flush, m_totalHits);
			}
	
			if(!GetHitsFromFiles()) 
			{
				coincidizer.FlushHitsToEvent();
				killFlag = true;
			} 
			else
				coincidizer.AddHitToEvent(hit);
	
			if(coincidizer.IsEventReady()) 
			{
				event = coincidizer.GetEvent();
				outtree->Fill();
				if(killFlag) break;
			}
		}
	
		output->cd();
		outtree->Write(outtree->GetName(), TObject::kOverwrite);
		for(auto& entry : m_scaler_map)
			entry.second.Write();
	
		coincidizer.GetEventStats()->Write();
		output->Close();
	}
	
	void CompassRun::Convert2FastSortedRoot(const std::string& name, const std::string& mapfile, double window, double fsi_window, double fic_window) 
	{
		TFile* output = TFile::Open(name.c_str(), "RECREATE");
		TTree* outtree = new TTree("SortTree", "SortTree");
	
		outtree->Branch("event", &event);
	
		if(!m_smap.IsValid()) 
		{
			EVB_WARN("Bad shift map ({0}) at CompassRun::Convert2FastSortedRoot(), shifts all set to 0.", m_smap.GetFilename());
		}
	
		SetScalers();
	
		if(!GetBinaryFiles()) 
		{
			EVB_ERROR("Unable to find binary files at CompassRun::Convert2FastSortedRoot(), exiting!");
			return;
		}
	
		unsigned int count = 0, flush = m_totalHits*m_progressFraction, flush_count = 0;
	
		startIndex = 0;
		CoincEvent this_event;
		std::vector<CoincEvent> fast_events;
		SlowSort coincidizer(window, mapfile);
		FastSort speedyCoincidizer(fsi_window, fic_window);
	
		FlagHandler flagger;
	
		bool killFlag = false;
		if(flush == 0) 
			flush = 1;
		while(true) 
		{
			count++;
			if(count == flush) 
			{
				count = 0;
				flush_count++;
				m_progressCallback(flush_count*flush, m_totalHits);
			}
			
			if(!GetHitsFromFiles()) 
			{
				coincidizer.FlushHitsToEvent();
				killFlag = true;
			} 
			else 
			{
				flagger.CheckFlag(hit.board, hit.channel, hit.flags);
				coincidizer.AddHitToEvent(hit);
			}
	
			if(coincidizer.IsEventReady()) 
			{
				this_event = coincidizer.GetEvent();
	
				fast_events = speedyCoincidizer.GetFastEvents(this_event);
				for(auto& entry : fast_events) 
				{
					event = entry;
					outtree->Fill();
				}
				if(killFlag) 
					break;
			}
		}
	
		output->cd();
		outtree->Write(outtree->GetName(), TObject::kOverwrite);
		for(auto& entry : m_scaler_map)
			entry.second.Write();
		
		coincidizer.GetEventStats()->Write();
		output->Close();
	}
	
	
	void CompassRun::Convert2SlowAnalyzedRoot(const std::string& name, const std::string& mapfile, double window,
										  int zt, int at, int zp, int ap, int ze, int ae, double bke, double b, double theta) 
	{
	
		TFile* output = TFile::Open(name.c_str(), "RECREATE");
		TTree* outtree = new TTree("SPSTree", "SPSTree");
	
		outtree->Branch("event", &pevent);
	
		if(!m_smap.IsValid()) 
		{
			EVB_WARN("Bad shift map ({0}) at CompassRun::Convert2SlowAnalyzedRoot(), shifts all set to 0.", m_smap.GetFilename());
		}
	
		SetScalers();
	
		if(!GetBinaryFiles()) 
		{
			EVB_ERROR("Unable to find binary files at CompassRun::Convert2SlowAnalyzedRoot(), exiting!");
			return;
		}
	
		unsigned int count = 0, flush = m_totalHits*m_progressFraction, flush_count = 0;
	
		startIndex = 0;
		CoincEvent this_event;
		SlowSort coincidizer(window, mapfile);
		SFPAnalyzer analyzer(zt, at, zp, ap, ze, ae, bke, theta, b);
	
		std::vector<TParameter<Double_t>> parvec;
		parvec.reserve(9);
		parvec.emplace_back("ZT", zt);
		parvec.emplace_back("AT", at);
		parvec.emplace_back("ZP", zp);
		parvec.emplace_back("AP", ap);
		parvec.emplace_back("ZE", ze);
		parvec.emplace_back("AE", ae);
		parvec.emplace_back("Bfield", b);
		parvec.emplace_back("BeamKE", bke);
		parvec.emplace_back("Theta", theta);
	
		bool killFlag = false;
		if(flush == 0) 
			flush = 1;
		while(true) 
		{
			count++;
			if(count == flush) 
			{
				count = 0;
				flush_count++;
				m_progressCallback(flush_count*flush, m_totalHits);
			}
	
			if(!GetHitsFromFiles()) 
			{
				coincidizer.FlushHitsToEvent();
				killFlag = true;
			} 
			else 
			{
				coincidizer.AddHitToEvent(hit);
			}
	
			if(coincidizer.IsEventReady()) 
			{
				this_event = coincidizer.GetEvent();
				pevent = analyzer.GetProcessedEvent(this_event);
				outtree->Fill();
				if(killFlag) 
					break;
			}
		}
	
		output->cd();
		outtree->Write(outtree->GetName(), TObject::kOverwrite);
		for(auto& entry : m_scaler_map)
			entry.second.Write();
	
		for(auto& entry : parvec)
			entry.Write();
	
		coincidizer.GetEventStats()->Write();
		analyzer.GetHashTable()->Write();
		analyzer.ClearHashTable();
		output->Close();
	}
	
	void CompassRun::Convert2FastAnalyzedRoot(const std::string& name, const std::string& mapfile, double window, double fsi_window, double fic_window,
										  int zt, int at, int zp, int ap, int ze, int ae, double bke, double b, double theta) 
	{
	
		TFile* output = TFile::Open(name.c_str(), "RECREATE");
		TTree* outtree = new TTree("SPSTree", "SPSTree");
	
		outtree->Branch("event", &pevent);
	
		if(!m_smap.IsValid()) 
		{
			EVB_WARN("Bad shift map ({0}) at CompassRun::Convert2FastAnalyzedRoot(), shifts all set to 0.", m_smap.GetFilename());
		}
	
		SetScalers();
	
		if(!GetBinaryFiles()) 
		{
			EVB_ERROR("Unable to find binary files at CompassRun::Convert2FastAnalyzedRoot(), exiting!");
			return;
		}
	
		unsigned int count = 0, flush = m_totalHits*m_progressFraction, flush_count = 0;
	
		startIndex = 0;
		CoincEvent this_event;
		std::vector<CoincEvent> fast_events;
		SlowSort coincidizer(window, mapfile);
		FastSort speedyCoincidizer(fsi_window, fic_window);
		SFPAnalyzer analyzer(zt, at, zp, ap, ze, ae, bke, theta, b);
	
		std::vector<TParameter<Double_t>> parvec;
		parvec.reserve(9);
		parvec.emplace_back("ZT", zt);
		parvec.emplace_back("AT", at);
		parvec.emplace_back("ZP", zp);
		parvec.emplace_back("AP", ap);
		parvec.emplace_back("ZE", ze);
		parvec.emplace_back("AE", ae);
		parvec.emplace_back("Bfield", b);
		parvec.emplace_back("BeamKE", bke);
		parvec.emplace_back("Theta", theta);
	
		FlagHandler flagger;
	
		bool killFlag = false;
		if(flush == 0) 
			flush = 1;
		while(true) 
		{
			count++;
			if(count == flush) 
			{
				count = 0;
				flush_count++;
				m_progressCallback(flush_count*flush, m_totalHits);
			}
	
			if(!GetHitsFromFiles()) 
			{
				coincidizer.FlushHitsToEvent();
				killFlag = true;
			} 
			else 
			{
				flagger.CheckFlag(hit.board, hit.channel, hit.flags);
				coincidizer.AddHitToEvent(hit);
			}
	
			if(coincidizer.IsEventReady()) 
			{
				this_event = coincidizer.GetEvent();
	
				fast_events = speedyCoincidizer.GetFastEvents(this_event);
				for(auto& entry : fast_events) 
				{
					pevent = analyzer.GetProcessedEvent(entry);
					outtree->Fill();
				}
				if(killFlag) 
					break;
			}
		}
	
		output->cd();
		outtree->Write(outtree->GetName(), TObject::kOverwrite);
		for(auto& entry : m_scaler_map) 
			entry.second.Write();
	
		for(auto& entry : parvec)
			entry.Write();
	
		coincidizer.GetEventStats()->Write();
		analyzer.GetHashTable()->Write();
		analyzer.ClearHashTable();
		output->Close();
	}
}