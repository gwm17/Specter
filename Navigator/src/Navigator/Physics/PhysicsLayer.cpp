#include "PhysicsLayer.h"
#include "Navigator/SpectrumManager.h"
#include "NavData.h"

namespace Navigator {

	PhysicsLayer::PhysicsLayer() :
		m_activeFlag(false), m_source(nullptr), m_eventBuilder(0), m_physThread(nullptr)
	{
	}

	PhysicsLayer::~PhysicsLayer() 
	{
		DetachDataSource();
		DestroyPhysThread();
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
		EventDispatcher dispatch(event);
		dispatch.Dispatch<PhysicsStartEvent>(BIND_EVENT_FUNCTION(PhysicsLayer::OnPhysicsStartEvent));
		dispatch.Dispatch<PhysicsStopEvent>(BIND_EVENT_FUNCTION(PhysicsLayer::OnPhysicsStopEvent));
	}

	bool PhysicsLayer::OnPhysicsStartEvent(PhysicsStartEvent& event)
	{
		if(m_activeFlag)
		{
			DetachDataSource();
			DestroyPhysThread();
		}

		AttachDataSource(event);

		if(m_activeFlag)
		{
			NAV_INFO("Starting new analysis thread...");
			m_physThread = new std::thread(&PhysicsLayer::RunSource, std::ref(*this));
		}
		return true;
	}

	bool PhysicsLayer::OnPhysicsStopEvent(PhysicsStopEvent& event)
	{
		DetachDataSource();
		DestroyPhysThread();
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
		NAV_INFO("Destroying the analysis thread...");
		if(m_physThread != nullptr && m_physThread->joinable())
		{
			m_physThread->join();
		}

		if(m_physThread != nullptr)
		{
			delete m_physThread;
			m_physThread = nullptr;
		}
		NAV_INFO("Thread destroyed.");
	}

	void PhysicsLayer::AttachDataSource(PhysicsStartEvent& event)
	{
		std::lock_guard<std::mutex> guard(m_sourceMutex);
		m_source.reset(CreateDataSource(event.GetSourceLocation(), event.GetSourcePort(), event.GetSourceType()));
		m_eventBuilder.SetCoincidenceWindow(event.GetCoincidenceWindow());
		m_eventBuilder.ClearAll();
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
		std::lock_guard<std::mutex> guard(m_sourceMutex);
		NAV_INFO("Detaching physics data source...");
		m_activeFlag = false;
		m_source.reset(nullptr);
		NAV_INFO("Detach succesful.");
	}

	void PhysicsLayer::RunSource()
	{
		SpectrumManager& manager = SpectrumManager::GetInstance();

		NavEvent event;
		NavData datum;
		while(m_activeFlag)
		{
			{
				std::lock_guard<std::mutex> guard(m_sourceMutex);
				if (m_source == nullptr || !m_source->IsValid())
				{
					m_activeFlag = false;
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
					m_activeFlag = false;
					return;
				}
			}

			if (m_eventBuilder.AddDatumToEvent(datum))
			{
				event = m_eventBuilder.GetReadyEvent();
				for (auto& stage : m_physStack)
					stage->AnalyzePhysicsEvent(event);
				
				manager.UpdateHistograms();
				manager.InvalidateParameters();
			}
		}
	}
}
