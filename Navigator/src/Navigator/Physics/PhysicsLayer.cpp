/*
	PhysicsLayer.cpp
	The layer of the application representing all physics. This includes the data source and the thread upon which data is piped, event built, and analyzed.
	PhysicsLayer receives a PhysicsStartEvent or PhysicsStopEvent to handle changes to the state of the secondary thread. As it handles the thread, it must have synchronization.
	This is handled on two levels. The first is a mutex that synchronizes access to the DataSource. The second is an atomic boolean flag which is used to control the state of the physics thread.
	PhysicsLayer also owns the AnalysisStack for the application.

	GWM -- Feb 2022
*/
#include "PhysicsLayer.h"
#include "Navigator/Core/SpectrumManager.h"
#include "NavData.h"

namespace Navigator {

	PhysicsLayer::PhysicsLayer() :
		m_activeFlag(false), m_source(nullptr), m_eventBuilder(0), m_physThread(nullptr)
	{
	}

	PhysicsLayer::~PhysicsLayer() 
	{
		NAV_PROFILE_FUNCTION();
		if (m_activeFlag)
		{
			DetachDataSource();
			DestroyPhysThread();
		}
	}

	void PhysicsLayer::OnAttach()
	{
		/* For debugging
		NavParameter par("joseph");
		par.SetValue(8);
		NAV_INFO("Does the par exist? {0}", ParameterMap::GetInstance().IsParameterValid("joseph"));
		NAV_INFO("What is its value? {0}", ParameterMap::GetInstance().GetParameterValue("joseph"));
		*/
	}

	void PhysicsLayer::OnDetach()
	{
	}

	void PhysicsLayer::OnEvent(Event& event)
	{
		NAV_PROFILE_FUNCTION();
		EventDispatcher dispatch(event);
		dispatch.Dispatch<PhysicsStartEvent>(BIND_EVENT_FUNCTION(PhysicsLayer::OnPhysicsStartEvent));
		dispatch.Dispatch<PhysicsStopEvent>(BIND_EVENT_FUNCTION(PhysicsLayer::OnPhysicsStopEvent));
	}

	bool PhysicsLayer::OnPhysicsStartEvent(PhysicsStartEvent& event)
	{
		NAV_PROFILE_FUNCTION();
		//If the Physics thread is active, kill it
		if(m_activeFlag)
		{
			DetachDataSource();
			DestroyPhysThread();
		}

		AttachDataSource(event);

		//If we succesfully attached, fire up a new phys thread
		if(m_activeFlag)
		{
			NAV_INFO("Starting new analysis thread...");
			m_physThread = new std::thread(&PhysicsLayer::RunSource, std::ref(*this));
		}
		return true;
	}

	bool PhysicsLayer::OnPhysicsStopEvent(PhysicsStopEvent& event)
	{
		NAV_PROFILE_FUNCTION();
		if (m_activeFlag)
		{
			DetachDataSource();
			DestroyPhysThread();
		}
		return true;
	}

	void PhysicsLayer::PushStage(AnalysisStage* stage)
	{
		m_physStack.PushStage(stage);
	}

	void PhysicsLayer::OnUpdate() {}

	/*Threaded functions*/

	void PhysicsLayer::DestroyPhysThread()
	{
		NAV_PROFILE_FUNCTION();
		NAV_INFO("Destroying the analysis thread...");
		//Join the thread back to the parent (finish up the thread)
		if(m_physThread != nullptr && m_physThread->joinable())
		{
			m_physThread->join();
		}

		//Free the thread memory
		if(m_physThread != nullptr)
		{
			delete m_physThread;
			m_physThread = nullptr;
		}
		NAV_INFO("Thread destroyed.");
	}

	void PhysicsLayer::AttachDataSource(PhysicsStartEvent& event)
	{
		NAV_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_sourceMutex); //Shouldn't matter for this, but safety first
		m_source.reset(CreateDataSource(event.GetSourceLocation(), event.GetSourcePort(), event.GetBitFlags(), event.GetChannelsPerBoard(), event.GetSourceType()));
		m_eventBuilder.SetCoincidenceWindow(event.GetCoincidenceWindow());
		m_eventBuilder.SetSortFlag(event.GetSortFlag());
		m_eventBuilder.ClearAll(); //Protect against stopping mid-event
		if (m_source->IsValid())
		{
			NAV_INFO("Attach successful. Enabling data pull...");
			m_activeFlag = true;
		}
		else
		{
			NAV_ERROR("DataSource attach failed... check for error conditions.");
			m_source.reset(nullptr);
		}
	}

	void PhysicsLayer::DetachDataSource()
	{
		NAV_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> guard(m_sourceMutex);
		NAV_INFO("Detaching physics data source...");
		m_activeFlag = false;
		m_source.reset(nullptr);
		NAV_INFO("Detach succesful.");
	}

	void PhysicsLayer::RunSource()
	{
		NAV_PROFILE_FUNCTION();
		SpectrumManager& manager = SpectrumManager::GetInstance();

		std::vector<NavEvent> events;
		NavData datum;
		while(m_activeFlag)
		{
			//Scope to encapsulate access to the data source
			{
				std::scoped_lock<std::mutex> guard(m_sourceMutex);
				if (m_source == nullptr || !m_source->IsValid())
				{
					return;
				}
				/*
					Looks funny, but two conditions lead to !IsValid(). Either source prev. shutdown,
					OR we reached end of source, indicated after prev. data grab
				*/
				datum = m_source->GetData();
				if(!m_source->IsValid())
				{
					NAV_INFO("End of data source.");
					return;
				}
			}

			//Pass data from source to event builder. True from AddDatum indicates events are ready
			if (m_eventBuilder.AddDatum(datum))
			{
				events = m_eventBuilder.GetReadyEvents();
				for (auto& event : events)
				{
					for (auto& stage : m_physStack)
						stage->AnalyzePhysicsEvent(event);

					//Now that the analysis stack has filled all our NavParameters with data, update the histogram counts
					manager.UpdateHistograms();
					//Invalidate all parameters to get ready for next event
					manager.InvalidateParameters();
				}
			}
		}
	}
}
