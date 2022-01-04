#include "PhysicsEventBuilder.h"

//temp
#include "CompassHit.h"

//GWM Jan. 3 2021 -- Make DataSource to unique ptr 

namespace Navigator {

	PhysicsEventBuilder* PhysicsEventBuilder::s_instance = nullptr;

	PhysicsEventBuilder::PhysicsEventBuilder() :
		m_runFlag(false), m_source(nullptr)
	{
		if(s_instance != nullptr)
		{
			NAV_ERROR("Trying to create a second PhysicsEventBuilder, this is not allowed!");
			return;
		}
		s_instance = this;
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
				m_rawSort.ClearRawPhysicsEvent();
			}
			
		}
	}

}
