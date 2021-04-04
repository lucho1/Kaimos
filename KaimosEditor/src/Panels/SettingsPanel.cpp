#include "kspch.h"
#include "SettingsPanel.h"
#include "ImGui/ImGuiUtils.h"

#include <ImGui/imgui.h>

namespace Kaimos {

	// ----------------------- Public Class Methods -------------------------------------------------------
	void SettingsPanel::OnUIRender(const Entity& hovered_entity, bool& closing_settings, bool& closing_performance)
	{
		// -- World Settings --
		if (closing_settings)
		{
			ImGui::Begin("World Settings", &closing_settings);

			// Show Hovered Entity
			std::string entity_name = "None";
			if (hovered_entity)
				entity_name = hovered_entity.GetComponent<TagComponent>().Tag;

			ImGui::Text("Hovered Entity: %s", entity_name.c_str());

			// Camera Display Options
			ImGui::NewLine(); ImGui::Separator();
			static uint current_option_type = 0;
			const char* camera_viewport_display_options[] = { "Always Show Primary Camera", "Show Only on Camera Selected", "Never Show" };
			const char* current_option = camera_viewport_display_options[current_option_type];

			ImGui::Text("Primary Camera Mini-Screen Viewport");
			if (KaimosUI::UIFunctionalities::DrawDropDown("Display Options", camera_viewport_display_options, 3, current_option, current_option_type, ImGui::CalcItemWidth() / 2.0f))
			{
				switch (current_option_type)
				{
					case 0:		ShowCameraMiniScreen = true; ShowCameraWhenSelected = false;	break;
					case 1:		ShowCameraMiniScreen = true; ShowCameraWhenSelected = true;		break;
					case 2:		ShowCameraMiniScreen = false; ShowCameraWhenSelected = false;	break;
					default:	ShowCameraMiniScreen = true; ShowCameraWhenSelected = false;	break;
				}
			}

			ImGui::End();
		}
		

		// -- Performance --
		if (closing_performance)
		{
			ImGui::Begin("Performance", &closing_performance);
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
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
		float float_mem_allocs[MEMORY_ALLOCATIONS_SAMPLES];
		for (uint i = 0; i < MEMORY_ALLOCATIONS_SAMPLES; ++i)
			float_mem_allocs[i] = (float)m_MemoryAllocations[i];		
		

		// -- Plots --
		// Plots Text
		ImGui::NewLine();
		float font_size = ImGui::GetFontSize() * 6.0f; // 6 is half the characters of "Memory Usage"
		ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size + (font_size / 2));
		ImGui::Text("Memory Usage");

		// Avg. Allocs. to Display
		char overlay[50];
		sprintf(overlay, "Current Allocations: %i (%i MB)", m_MemoryMetrics.GetCurrentAllocations(), BTOMB(m_MemoryMetrics.GetCurrentMemoryUsage()));

		// Plots
		float plots_width = ImGui::GetContentRegionAvailWidth();
		ImGui::PlotLines("###MemoryUsage", float_mem_allocs, IM_ARRAYSIZE(float_mem_allocs), 0, overlay, 0.0f, 5000.0f, ImVec2(plots_width, 100.0f));
		ImGui::PlotHistogram("###MemoryUsageHistogram", float_mem_allocs, IM_ARRAYSIZE(float_mem_allocs), 0, overlay, 0.0f, 5000.0f, ImVec2(plots_width, 100.0f));


		// -- Memory Metrics Gathering Stop --
		static bool stop = false;
		ImGui::Checkbox("Stop Metrics Count", &stop);
		if (!stop)
			SetMemoryMetrics();


		// -- Printing --
		float text_separation = ImGui::GetContentRegionAvailWidth() / 3.0f + 25.0f;
		ImGui::NewLine();

		ImGui::Text("Allocations"); ImGui::SameLine(text_separation);
		ImGui::Text("%i (%i MB)", m_MemoryMetrics.GetAllocations(), BTOMB(m_MemoryMetrics.GetAllocationsSize()));

		ImGui::Text("Deallocations"); ImGui::SameLine(text_separation);
		ImGui::Text("%i (%i MB)", m_MemoryMetrics.GetDeallocations(), BTOMB(m_MemoryMetrics.GetDeallocationsSize()));

		ImGui::Text("Current Memory Usage"); ImGui::SameLine(text_separation);
		ImGui::Text("%i (%i MB)", m_MemoryMetrics.GetCurrentAllocations(), BTOMB(m_MemoryMetrics.GetCurrentMemoryUsage()));
	}


	void SettingsPanel::DisplayRenderingMetrics()
	{
		auto stats = Renderer2D::GetStats();
		float text_separation = ImGui::GetContentRegionAvailWidth() / 3.0f + 25.0f;

		// -- Draw Calls --
		ImGui::Text("Draw Calls"); ImGui::SameLine(text_separation);
		ImGui::Text("%i", stats.DrawCalls);

		ImGui::Text("Max Quads x Draw Call"); ImGui::SameLine(text_separation);
		ImGui::Text("%i", Renderer2D::GetMaxQuads());


		// -- Geometry --
		ImGui::NewLine(); ImGui::NewLine();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 icons_color = ImColor(1.0f, 0.7f, 0.0f);
		float icons_size = 12.0f;
		float icons_indent = icons_size + 25.0f;
		
		// Icons Positioning & Size
		glm::vec2 position = KaimosUI::ConvertToVec2(ImGui::GetCursorScreenPos()) - glm::vec2(0.0f, 20.0f);
		
		// Quads
		draw_list->AddRectFilled(KaimosUI::ConvertToImVec2(position), KaimosUI::ConvertToImVec2(position + glm::vec2(icons_size)), icons_color);
		
		ImGui::SameLine(icons_indent);
		ImGui::Text("Quads Drawn"); ImGui::SameLine(text_separation);
		ImGui::Text("%i", stats.QuadCount);
		
		// Triangles
		ImGui::NewLine();
		position = KaimosUI::ConvertToVec2(ImGui::GetCursorScreenPos()) + glm::vec2(icons_size/2.0f, -20.0f);
		draw_list->AddTriangleFilled(KaimosUI::ConvertToImVec2(position), KaimosUI::ConvertToImVec2(position + glm::vec2(-icons_size/2.0f, icons_size)), KaimosUI::ConvertToImVec2(position + glm::vec2(icons_size/2.0f, icons_size)), icons_color);
				
		ImGui::SameLine(icons_indent);
		ImGui::Text("Triangles Drawn"); ImGui::SameLine(text_separation);
		ImGui::Text("%i", stats.GetTotalTrianglesCount());

		// Circle
		ImGui::NewLine();
		position = KaimosUI::ConvertToVec2(ImGui::GetCursorScreenPos()) + glm::vec2(icons_size/2.0f, -14.0f);
		draw_list->AddCircleFilled(KaimosUI::ConvertToImVec2(position), 2.5f, icons_color);
		
		ImGui::SameLine(icons_indent);
		ImGui::Text("Vertices Drawn"); ImGui::SameLine(text_separation);
		ImGui::Text("%i	(%i Indices)", stats.GetTotalVerticesCount(), stats.GetTotalIndicesCount());
	}
}
