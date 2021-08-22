#ifndef _SETTINGSPANEL_H_
#define _SETTINGSPANEL_H_

#include "Kaimos.h"

#define METRICS_ALLOCATIONS_SAMPLES 90

namespace Kaimos {

	class SettingsPanel
	{
	public:

		// --- Public Class Methods ---
		SettingsPanel() = default;
		void OnUIRender(const Entity& hov_entity, bool& closing_settings, bool& closing_performance);

	private:

		// --- Private Scene Methods ---
		void SetFPSMetrics();
		void DisplayFPSMetrics();
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


		// Memory Metrics
		uint m_MemoryAllocations[METRICS_ALLOCATIONS_SAMPLES] = {0};
		uint m_MemoryAllocationsIndex = 0;

		MemoryMetrics m_MemoryMetrics = {};
	};
}

#endif //_SETTINGSPANEL_H_
