#include "PhysicsEventBuilder.h"

//temp
#include "CompassHit.h"

namespace Navigator {

	PhysicsEventBuilder* PhysicsEventBuilder::s_instance = nullptr;

	PhysicsEventBuilder::PhysicsEventBuilder() :
		m_runFlag(false)
	{
		if(s_instance != nullptr)
		{
			NAV_ERROR("Trying to create a second PhysicsEventBuilder, this is not allowed!");
			return;
		}
		s_instance = this;
	}

	PhysicsEventBuilder::~PhysicsEventBuilder() {}

	void PhysicsEventBuilder::AttachDataSource()
	{
		m_runFlag = true;
	}

	void PhysicsEventBuilder::DetachDataSource()
	{
		m_runFlag = false;
	}

	void PhysicsEventBuilder::PushLayer(Layer* layer)
	{
		m_physStack.PushLayer(layer);
	}

	void PhysicsEventBuilder::PushOverlay(Layer* layer)
	{
		m_physStack.PushOverlay(layer);
	}

	void PhysicsEventBuilder::Run()
	{
		if(!m_runFlag)
			NAV_WARN("Trying to Run PhysicsEventBuilder without a Data Source, nothing will happen!");

		//temp
		CompassHit hit;
		m_rawSort.SetCoincidenceWindow(2);
		while(m_runFlag)
		{
			//NAV_INFO("Doing a physics");
			hit.timestamp++;
			if(m_rawSort.IsHitInWindow(hit))
			{
				NAV_INFO("Adding hit to event...");
				m_rawSort.AddHit(hit);
			}
			else
			{
				NAV_INFO("Obtaining built event...");
				auto event = m_rawSort.GetRawPhysicsEvent();
				NAV_INFO("Built event size: {0}", event.size());
				m_rawSort.ClearRawPhysicsEvent();
			}
		}
	}

}