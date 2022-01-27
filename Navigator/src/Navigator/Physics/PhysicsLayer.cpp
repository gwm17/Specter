#include "PhysicsLayer.h"
#include "Navigator/ParameterMap.h"

//temp
#include "CompassHit.h"

namespace Navigator {

	PhysicsLayer::PhysicsLayer() :
		m_activeFlag(false), m_source(nullptr)
	{
	}

	PhysicsLayer::~PhysicsLayer() 
	{
	}

	void PhysicsLayer::OnAttach()
	{
		NavParameter par("joseph");
		par.SetValue(8);
		NAV_INFO("Does the par exist? {0}", ParameterMap::GetInstance().IsParameterValid("joseph"));
		NAV_INFO("What is its value? {0}", ParameterMap::GetInstance().GetParameterValue("joseph"));
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
		m_rawSort.SetCoincidenceWindow(event.GetCoincidenceWindow());
		m_source.reset(CreateDataSource(event.GetSourceLocation(), event.GetSourcePort(), event.GetSourceType()));
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
		return true;
	}

	bool PhysicsLayer::OnPhysicsStopEvent(PhysicsStopEvent& event)
	{
		NAV_INFO("Detaching physics data source...");
		m_activeFlag = false;
		m_source.reset(nullptr);
		NAV_INFO("Detach succesful.");
		return true;
	}

	void PhysicsLayer::PushStage(AnalysisStage* stage)
	{
		m_physStack.PushStage(stage);
	}

	void PhysicsLayer::OnUpdate()
	{
		HistogramMap& histMap = HistogramMap::GetInstance();

		if(!m_activeFlag)
		{
			return;
		}
		else if (m_source == nullptr || !m_source->IsValid())
		{
			NAV_WARN("Internal state of PhysicsEventBuilder not set properly! Either histogram map or data source not initialized!");
			return;
		}

		CompassHit hit;
		hit = m_source->GetData();
	
		/*
			Looks funny, but two conditions lead to !IsValid(). Either source prev. shutdown,
			OR we reached end of source, indicated after prev. data grab
		*/
		if(!m_source->IsValid())
		{
			NAV_INFO("End of data source.");
			m_activeFlag = false;
			return;
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
			histMap.UpdateHistograms();

			//Cleanup to be ready for next event
			ParameterMap::GetInstance().InvalidateParameters();
			m_rawSort.ClearRawPhysicsEvent();
			//Need to add hit in hand, start new event
			m_rawSort.AddHit(hit);
		}
			
	}

}
