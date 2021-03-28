#include "kspch.h"
#include "SettingsPanel.h"

#include <ImGui/imgui.h>

namespace Kaimos {


	// ----------------------- Public Class Methods -------------------------------------------------------
	void SettingsPanel::OnUIRender(const Entity& hovered_entity, EditorCamera& editor_camera)
	{
		// -- World Settings --
		ImGui::Begin("World Settings");

		// Show Hovered Entity
		std::string entity_name = "None";
		if (hovered_entity)
			entity_name = hovered_entity.GetComponent<TagComponent>().Tag;

		ImGui::Text("Hovered Entity: %s", entity_name.c_str());

		// Lock Camera Rotation
		static bool camera_lock = false;
		ImGui::Checkbox("Lock Camera Rotation", &camera_lock);
		editor_camera.LockRotation(camera_lock);

		ImGui::End();
		

		// -- Performance --
		ImGui::Begin("Performance");
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth |ImGuiTreeNodeFlags_FramePadding;
		// Memory
		if (ImGui::CollapsingHeader("Memory", flags))
			DisplayMemoryMetrics();
		
		// Rendering
		ImGui::NewLine();
		if (ImGui::CollapsingHeader("Rendering", flags))
			DisplayRenderingMetrics();

		// -- End Panel --
		ImGui::End();
	}



	// ----------------------- Private Class Methods ------------------------------------------------------
	void SettingsPanel::SetMemoryMetrics()
	{
		m_MemoryMetrics = Application::Get().GetMemoryMetrics();
		if (m_MemoryAllocationsIndex == MEMORY_ALLOCATIONS_SAMPLES)
			m_MemoryAllocationsIndex = 0;

		m_MemoryAllocations[m_MemoryAllocationsIndex] = m_MemoryMetrics.GetCurrentAllocations();
		++m_MemoryAllocationsIndex;
	}


	void SettingsPanel::DisplayMemoryMetrics()
	{

		
		// -- Memory Metrics Gathering --
		static bool stop = false;
		ImGui::Checkbox("Stop", &stop);
		if (!stop)
			SetMemoryMetrics();
		
		float float_mem_allocs[MEMORY_ALLOCATIONS_SAMPLES];
		for (uint i = 0; i < MEMORY_ALLOCATIONS_SAMPLES; ++i)
			float_mem_allocs[i] = (float)m_MemoryAllocations[i];

		
		// -- Average Mem. Allocs. to Display --
		char overlay[50];
		sprintf(overlay, "Current Allocations: %i (%i MB)", m_MemoryMetrics.GetCurrentAllocations(), BTOMB(m_MemoryMetrics.GetCurrentMemoryUsage()));

		// -- Plots --
		ImGui::PlotLines("Memory Usage", float_mem_allocs, IM_ARRAYSIZE(float_mem_allocs), 0, overlay, 0.0f, 5000.0f, ImVec2(0.0f, 100.0f));
		ImGui::PlotHistogram("Memory Usage Histogram", float_mem_allocs, IM_ARRAYSIZE(float_mem_allocs), 0, overlay, 0.0f, 5000.0f, ImVec2(0.0f, 100.0f));

		// -- Printing --
		ImGui::NewLine();
		ImGui::Text("Allocations: %i (%i MB)",						m_MemoryMetrics.GetAllocations(),			BTOMB(m_MemoryMetrics.GetAllocationsSize()));
		ImGui::Text("Deallocations: %i (%i MB)",					m_MemoryMetrics.GetDeallocations(),			BTOMB(m_MemoryMetrics.GetDeallocationsSize()));
		ImGui::Text("Current Memory Usage: %i Allocated (%i MB)",	m_MemoryMetrics.GetCurrentAllocations(),	BTOMB(m_MemoryMetrics.GetCurrentMemoryUsage()));

	}


	void SettingsPanel::DisplayRenderingMetrics()
	{
		auto stats = Renderer2D::GetStats();

		//ImGui::AlignTextToFramePadding();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads Drawn: %d", stats.QuadCount);
		ImGui::Text("Max Quads per Draw Call: %d", Renderer2D::GetMaxQuads());

		ImGui::NewLine();
		ImGui::Text("Vertices: %d", stats.GetTotalVerticesCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndicesCount());
		ImGui::Text("Tris: %d", stats.GetTotalTrianglesCount());
	}
}
