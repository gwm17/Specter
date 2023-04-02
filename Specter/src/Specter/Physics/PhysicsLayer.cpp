/*
	PhysicsLayer.cpp
	The layer of the application representing all physics. This includes the data source and the thread upon which data is piped, event built, and analyzed.
	PhysicsLayer receives a PhysicsStartEvent or PhysicsStopEvent to handle changes to the state of the secondary thread. As it handles the thread, it must have synchronization.
	This is handled on two levels. The first is a mutex that synchronizes access to the DataSource. The second is an atomic boolean flag which is used to control the state of the physics thread.
	PhysicsLayer also owns the AnalysisStack for the application.

	GWM -- Feb 2022
*/
#include "PhysicsLayer.h"
#include "SpecData.h"

namespace Specter {

	PhysicsLayer::PhysicsLayer(const SpectrumManager::Ref& manager) :
		m_manager(manager), m_activeFlag(false), m_source(nullptr), m_physThread(nullptr)
	{
	}

	PhysicsLayer::~PhysicsLayer() 
	{
		SPEC_PROFILE_FUNCTION();
		if (m_activeFlag)
		{
			DetachDataSource();
		}
	}

	void PhysicsLayer::OnAttach()
	{
	}

	void PhysicsLayer::OnDetach()
	{
	}

	void PhysicsLayer::OnEvent(Event& event)
	{
		SPEC_PROFILE_FUNCTION();
		EventDispatcher dispatch(event);
		dispatch.Dispatch<PhysicsStartEvent>(BIND_EVENT_FUNCTION(PhysicsLayer::OnPhysicsStartEvent));
		dispatch.Dispatch<PhysicsStopEvent>(BIND_EVENT_FUNCTION(PhysicsLayer::OnPhysicsStopEvent));
	}

	bool PhysicsLayer::OnPhysicsStartEvent(PhysicsStartEvent& event)
	{
		SPEC_PROFILE_FUNCTION();
		//If the Physics thread is active, kill it
		if(m_activeFlag)
		{
			DetachDataSource();
		}

		AttachDataSource(event.GetSourceArgs());

		return true;
	}

	bool PhysicsLayer::OnPhysicsStopEvent(PhysicsStopEvent& event)
	{
		SPEC_PROFILE_FUNCTION();
		if (m_activeFlag)
		{
			DetachDataSource();
		}
		return true;
	}

	void PhysicsLayer::PushStage(AnalysisStage* stage)
	{
		m_physStack.PushStage(stage);
	}

	void PhysicsLayer::OnUpdate(Timestep& step) {}


	/*Threaded functions*/

	void PhysicsLayer::AttachDataSource(const SourceArgs& args)
	{
		SPEC_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_sourceMutex); //Shouldn't matter for this, but safety first
		m_source.reset(CreateDataSource(args));
		if (m_source->IsValid())
		{
			SPEC_INFO("Source attached... Starting new analysis thread...");
			m_activeFlag = true;

			m_physThread = new std::thread(&PhysicsLayer::RunSource, std::ref(*this));
		}
		else
		{
			SPEC_ERROR("DataSource attach failed... check for error conditions.");
			m_source.reset(nullptr);
		}
	}

	void PhysicsLayer::DetachDataSource()
	{
		SPEC_PROFILE_FUNCTION();
		SPEC_INFO("Detaching physics data source...");

		m_activeFlag = false;
		{
			std::scoped_lock<std::mutex> guard(m_sourceMutex);
			m_source.reset(nullptr);
		}
		
		if (m_physThread != nullptr && m_physThread->joinable())
		{
			m_physThread->join();
		}
		delete m_physThread;
		m_physThread = nullptr;

		SPEC_INFO("Detach succesful.");
	}

	void PhysicsLayer::RunSource()
	{
		SPEC_PROFILE_FUNCTION();

		std::vector<SpecEvent> events;
		while(m_activeFlag)
		{
			//Scope to encapsulate access to the data source
			{
				std::scoped_lock<std::mutex> guard(m_sourceMutex);
				if (m_source == nullptr || !m_source->IsValid())
				{
					SPEC_INFO("End of data source.");
					return;
				}
				
				m_source->ProcessData();
				if(m_source->IsEventReady())
				{
					events = m_source->GetEvents();
				}
			}

			for (auto& event : events)
			{
				for (auto& stage : m_physStack)
					stage->AnalyzePhysicsEvent(event);

				//Now that the analysis stack has filled all our Parameters with data, update the histogram counts
				m_manager->UpdateHistograms();
				//Invalidate all parameters to get ready for next event
				m_manager->InvalidateParameters();
			}

			if(!events.empty())
				events.clear();
		}
	}
}
