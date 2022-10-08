#ifndef _SETTINGSPANEL_H_
#define _SETTINGSPANEL_H_

#include "Kaimos.h"
#include "Core/Utils/Time/Timer.h"

#define METRICS_ALLOCATIONS_SAMPLES 90

namespace Kaimos {
	class Scene;

	class SettingsPanel
	{
	public:

		// --- Public Class Methods ---
		SettingsPanel();
		void OnUIRender(const Ref<Scene>& current_scene, const Entity& hov_entity, bool& closing_settings, bool& closing_performance);

	private:

		// --- Private Scene Methods ---
		void SetCameraDisplayOption(uint display_option);

		void SetFPSMetrics();
		void SetRenderTimeMetrics(const Ref<Scene>& current_scene);
		void DisplayFPSMetrics(const Ref<Scene>& current_scene);
		void SetMemoryMetrics();
		void DisplayMemoryMetrics();
		void DisplayRenderingMetrics(bool display_3Dmetrics);

	public:

		bool ShowCameraMiniScreen = true;
		bool ShowCameraWhenSelected = false;

	private:

		// Performance (FPS) Metrics
		uint m_FPSAllocations[METRICS_ALLOCATIONS_SAMPLES] = {0};
		uint m_FPSAllocationsIndex = 0;

		// Rendering Time Metrics
		float m_RenderingAllocations[METRICS_ALLOCATIONS_SAMPLES] = { 0 };
		uint m_RenderingAllocationsIndex = 0;
		Timer m_RenderingAvgMeasureUpdateTimer = {};

		// Memory Metrics
		uint m_MemoryAllocations[METRICS_ALLOCATIONS_SAMPLES] = {0};
		uint m_MemoryAllocationsIndex = 0;

		MemoryMetrics m_MemoryMetrics = {};
	};
}

#endif //_SETTINGSPANEL_H_
