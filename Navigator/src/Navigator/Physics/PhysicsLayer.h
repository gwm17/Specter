/*
	PhysicsLayer.h
	The layer of the application representing all physics. This includes the data source and the thread upon which data is piped, event built, and analyzed.
	PhysicsLayer receives a PhysicsStartEvent or PhysicsStopEvent to handle changes to the state of the secondary thread. As it handles the thread, it must have synchronization.
	This is handled on two levels. The first is a mutex that synchronizes access to the DataSource. The second is an atomic boolean flag which is used to control the state of the physics thread.
	PhysicsLayer also owns the AnalysisStack for the application.

	GWM -- Feb 2022
*/
#ifndef PHYSICS_LAYER_H
#define PHYSICS_LAYER_H

#include "Navigator/Core/NavCore.h"
#include "Navigator/Core/Layer.h"
#include "Navigator/Events/PhysicsEvent.h"
#include "AnalysisStack.h"
#include "AnalysisStage.h"
#include "DataSource.h"
#include "PhysicsEventBuilder.h"

#include <thread>
#include <mutex>
#include <atomic>

namespace Navigator {

	class NAV_API PhysicsLayer : public Layer
	{
	public:
		PhysicsLayer();
		virtual ~PhysicsLayer();

		virtual void OnAttach() override;
		virtual void OnUpdate(Timestep& step) override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override {};
		virtual void OnEvent(Event& event) override;

		bool OnPhysicsStartEvent(PhysicsStartEvent& event);
		bool OnPhysicsStopEvent(PhysicsStopEvent& event);

		void PushStage(AnalysisStage* stage);

	private:
		void DestroyPhysThread();
		void AttachDataSource(PhysicsStartEvent& event);
		void DetachDataSource();
		void RunSource();

		AnalysisStack m_physStack;
		std::atomic<bool> m_activeFlag; //safe read/write across thread, but more expensive

		std::mutex m_sourceMutex;

		std::unique_ptr<DataSource> m_source;
		PhysicsEventBuilder m_eventBuilder;

		std::thread* m_physThread;

	};

}

#endif
