#ifndef PHYSICS_EVENT_BUILDER_H
#define PHYSICS_EVENT_BUILDER_H

#include "Navigator/NavCore.h"
#include "Navigator/HistogramMap.h"
#include "Navigator/Layer.h"
#include "Navigator/Events/PhysicsEvent.h"
#include "AnalysisStack.h"
#include "AnalysisStage.h"
#include "PhysicsHitSort.h"
#include "DataSource.h"

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
		AnalysisStack m_physStack;
		bool m_activeFlag;
		PhysicsHitSort m_rawSort;

		std::unique_ptr<DataSource> m_source;

	};

}

#endif
