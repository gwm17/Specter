#ifndef PHYSICS_LAYER_H
#define PHYSICS_LAYER_H

#include "Navigator/NavCore.h"
#include "Navigator/HistogramMap.h"
#include "Navigator/Layer.h"
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
		virtual void OnUpdate() override;
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
		std::atomic<bool> m_activeFlag;

		std::mutex m_sourceMutex;

		std::unique_ptr<DataSource> m_source;
		PhysicsEventBuilder m_eventBuilder;

		std::thread* m_physThread;

	};

}

#endif
