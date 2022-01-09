#include "PhysicsEventBuilder.h"
#include "Navigator/ParameterMap.h"

//temp
#include "CompassHit.h"

namespace Navigator {

	PhysicsEventBuilder* PhysicsEventBuilder::s_instance = nullptr;

	PhysicsEventBuilder::PhysicsEventBuilder() :
		m_runFlag(false), m_source(nullptr), m_histMap(nullptr)
	{
		if(s_instance != nullptr)
		{
			NAV_ERROR("Trying to create a second PhysicsEventBuilder, this is not allowed!");
			return;
		}
		s_instance = this;

		CreateParameterMap();

		NavParameter par("joseph", "mama");
		par.SetValue(8);
		NAV_INFO("Does the par exist? {0}", ParameterMap::GetInstance().IsParameterValid("joseph"));
		NAV_INFO("What is its value? {0}", ParameterMap::GetInstance().GetParameterValue("joseph"));
	}

	PhysicsEventBuilder::~PhysicsEventBuilder() 
	{
	}

	void PhysicsEventBuilder::AttachDataSource(const std::string& loc, DataSource::SourceType type)
	{
		std::lock_guard<std::mutex> lock(m_sourceLock); //Auto free lock at end of scope
		NAV_INFO("Attempting to attach phyiscs data source at location {0}", loc);

		m_runFlag = false;

		m_source.reset(CreateDataSource(loc, type));
		if(m_source->IsValid())
		{
			NAV_INFO("Attach successful. Enabling data pull...");
			m_runFlag = true;
		}
		else
		{
			NAV_ERROR("DataSource attach failed... check for error conditions.");
			m_source.reset(nullptr);
		}
	}

	void PhysicsEventBuilder::DetachDataSource()
	{
		std::lock_guard<std::mutex> lock(m_sourceLock);
		NAV_INFO("Detaching physics data source...");
		m_runFlag = false;
		m_source.reset(nullptr);
		NAV_INFO("Detach successful");
	}

	void PhysicsEventBuilder::PushStage(AnalysisStage* stage)
	{
		m_physStack.PushStage(stage);
	}

	void PhysicsEventBuilder::Run()
	{
		if(!m_runFlag)
		{
			NAV_WARN("Trying to Run PhysicsEventBuilder without a Data Source, killing thread!");
		}
		else if (m_histMap == nullptr || m_source == nullptr)
		{
			NAV_WARN("Internal state of PhysicsEventBuilder not set properly! Either histogram map or data source not initialized!");
			return;
		}

		CompassHit hit;
		
		while(m_runFlag)
		{

			//small scope for locking access to the data source
			{
				std::lock_guard<std::mutex> lock(m_sourceLock);
				if(m_source == nullptr) // safety for stop occuring while running
				{
					continue;
				}
				else if(m_source->IsValid())
				{
					hit = m_source->GetData();
				}
	
				/*
					Looks funny, but two conditions lead to !IsValid(). Either source prev. shutdown,
					OR we reached end of source, indicated after prev. data grab
				*/
				if(!m_source->IsValid())
				{
					NAV_INFO("End of data source.");
					m_runFlag = false;
					continue;
				}
			}

			//NAV_INFO("Doing a physics");
			if(m_rawSort.IsHitInWindow(hit))
			{
				//NAV_INFO("Adding hit to event with timestamp {0}", hit.timestamp);
				m_rawSort.AddHit(hit);
			}
			else
			{
				//NAV_INFO("Obtaining built event...");
				auto event = m_rawSort.GetRawPhysicsEvent();
				//NAV_INFO("Built event size: {0}", event.size());
				for (auto& stage : m_physStack)
					stage->AnalyzeRawPhysicsEvent(event);
				m_histMap->UpdateHistograms();

				//Cleanup to be ready for next event
				ParameterMap::GetInstance().InvalidateParameters();
				m_rawSort.ClearRawPhysicsEvent();
				//Need to add hit in hand, start new event
				m_rawSort.AddHit(hit);
			}
			
		}
	}

}
