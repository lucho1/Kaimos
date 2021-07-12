#ifndef _SETTINGSPANEL_H_
#define _SETTINGSPANEL_H_

#include "Kaimos.h"

#define MEMORY_ALLOCATIONS_SAMPLES 90

namespace Kaimos {

	class SettingsPanel
	{
	public:

		// --- Public Class Methods ---
		SettingsPanel() = default;
		void OnUIRender(const Entity& hov_entity, bool& closing_settings, bool& closing_performance);

	private:

		// --- Private Scene Methods ---
		void SetMemoryMetrics();
		void DisplayMemoryMetrics();
		void DisplayRenderingMetrics(bool display_3Dmetrics);

	public:

		bool ShowCameraMiniScreen = true;
		bool ShowCameraWhenSelected = false;

	private:

		uint m_MemoryAllocations[MEMORY_ALLOCATIONS_SAMPLES] = { 0 };
		uint m_MemoryAllocationsIndex = 0;

		MemoryMetrics m_MemoryMetrics = {};
	};
}

#endif //_SETTINGSPANEL_H_
