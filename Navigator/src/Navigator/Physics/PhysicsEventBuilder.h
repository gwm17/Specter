#ifndef PHYSICS_EVENT_BUILDER_H
#define PHYSICS_EVENT_BUILDER_H

#include "Navigator/NavCore.h"
#include "Navigator/LayerStack.h"
#include "Navigator/Layer.h"
#include "PhysicsHitSort.h"
#include <mutex>
#include <atomic>

namespace Navigator {

	class PhysicsEventBuilder
	{
	public:
		PhysicsEventBuilder();
		virtual ~PhysicsEventBuilder();

		void Run();

		void AttachDataSource();
		void DetachDataSource();

		void SetCoincidenceWindow(uint64_t window) { m_rawSort.SetCoincidenceWindow(window); }
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		bool IsRunning() { return m_runFlag; }

		inline static PhysicsEventBuilder& Get() { return *s_instance; }

	private:
		LayerStack m_physStack;
		std::atomic<bool> m_runFlag; //ensures defined read/write across threads
		static PhysicsEventBuilder* s_instance;
		PhysicsHitSort m_rawSort;

	};

	PhysicsEventBuilder* CreatePhysicsEventBuilder();

}

#endif