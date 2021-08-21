#include "kspch.h"
#include "SettingsPanel.h"

#include "Renderer/Renderer.h"

#include "ImGui/ImGuiUtils.h"
#include "Core/Utils/PlatformUtils.h"

#include <ImGui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Kaimos {

	// ----------------------- Public Class Methods -------------------------------------------------------
	void SettingsPanel::OnUIRender(const Entity& hovered_entity, bool& closing_settings, bool& closing_performance)
	{
		// -- World Settings --
		if (closing_settings)
		{
			ImGui::Begin("World Settings", &closing_settings);

			// Show Hovered Entity
			//std::string entity_name = "None";
			//if (hovered_entity)
			//	entity_name = hovered_entity.GetComponent<TagComponent>().Tag;
			//ImGui::Text("Hovered Entity: %s", entity_name.c_str());

			// -- Camera Display Options --
			// Title
			static uint current_option_type = 0;
			const std::vector<std::string> camera_viewport_display_options = { "Always Show Primary Camera", "Show Only on Camera Selected", "Never Show" };
			std::string current_option = camera_viewport_display_options[current_option_type];

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			float font_size = ImGui::GetFontSize() * 17.5f; // 17.5 is half the characters of "Primary Camera Mini-Screen Viewport"
			ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size + (font_size / 2));
			
			ImGui::Text("Primary Camera Mini-Screen Viewport");
			ImGui::PopFont();

			// Display Options Dropdown
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

			// -- Renderer Options --
			// Title
			ImGui::NewLine(); ImGui::Separator();
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			font_size = ImGui::GetFontSize() * 8.5f; // 8.5 is half the characters of "Renderer Settings"
			ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size + (font_size / 2));

			ImGui::Text("Renderer Settings");
			ImGui::PopFont();

			// Scene Color
			KaimosUI::UIFunctionalities::SetTextCursorAndWidth("Scene Color");

			glm::vec3 scene_color = Renderer::GetSceneColor();
			if (ImGui::ColorEdit3("###scene_color", glm::value_ptr(scene_color), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs))
				Renderer::SetSceneColor(scene_color);

			// Environment Map
			ImGui::NewLine();
			uint enviromap_id = Renderer::GetEnvironmentMapID();
			glm::ivec2 enviromap_size = Renderer::GetEnvironmentMapSize();
			std::string enviromap_path = Renderer::GetEnvironmentMapFilepath();
			std::string enviromap_name = enviromap_path;

			if (!enviromap_path.empty())
				enviromap_name = enviromap_path.substr(enviromap_path.find_last_of("/\\" + 1, enviromap_path.size() - 1) + 1);

			ImGui::Text("Environment Map: %s", enviromap_name.c_str());
			ImGui::Text("Size (ID): %ix%i (%i)", enviromap_size.x, enviromap_size.y, enviromap_id);

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
			if (KaimosUI::UIFunctionalities::DrawTexturedButton("###enviromap", enviromap_id, glm::vec2(50.0f), glm::vec3(0.1f)))
			{
				std::string texture_file = FileDialogs::OpenFile("HDR Textures (*.hdr)\0*.hdr\0");
				if (!texture_file.empty())
					Renderer::SetEnvironmentMap(texture_file);
			}

			KaimosUI::UIFunctionalities::PopButton(false);
			ImGui::SameLine();

			// "X" Btn (to remove texture)
			if (KaimosUI::UIFunctionalities::DrawColoredButton("X", { 20.0f, 50.0f }, glm::vec3(0.15f), true))
				Renderer::RemoveEnvironmentMap();

			KaimosUI::UIFunctionalities::PopButton(true);
			ImGui::PopStyleVar();

			// Cubemap Resolution Picker
			ImGui::NewLine();
			static int res_ix = 2;
			const char* resolution_items[] = { "256", "512", "1024", "2048"/*, "4096", "8192"*/ };
			KaimosUI::UIFunctionalities::SetTextCursorAndWidth("Environment Map Resolution");
			ImGui::Combo("###environment_map_res", &res_ix, resolution_items, IM_ARRAYSIZE(resolution_items));

			if (ImGui::Button("Recompile Environment Map"))
			{
				uint environmentmap_resolutions[4] = { 256, 512, 1024, 2048/*, 4096, 8192*/ };
				uint enviromap_res = (res_ix >= 0 && res_ix < 4) ? environmentmap_resolutions[res_ix] : 1024;

				Renderer::ForceEnvironmentMapRecompile(enviromap_res);
			}

			// End
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
			if (ImGui::CollapsingHeader("2D Rendering", flags))
				DisplayRenderingMetrics(false);

			ImGui::NewLine();
			if (ImGui::CollapsingHeader("3D Rendering", flags))
				DisplayRenderingMetrics(true);

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

		m_MemoryAllocations[m_MemoryAllocationsIndex] = static_cast<uint>(BTOMB(m_MemoryMetrics.GetCurrentMemoryUsage()) * 10.0f);
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
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		float font_size = ImGui::GetFontSize() * 6.0f; // 6 is half the characters of "Memory Usage"
		ImGui::SameLine(ImGui::GetWindowSize().x / 2 - font_size + (font_size / 2));
		ImGui::Text("Memory Usage");
		ImGui::PopFont();

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


	void SettingsPanel::DisplayRenderingMetrics(bool display_3Dmetrics)
	{
		// -- Get Metrics --
		uint draw_calls = 0, quad_count = 0, tris_count = 0, vertices_count = 0, indices_count = 0;
		float text_separation = ImGui::GetContentRegionAvailWidth() / 3.0f + 25.0f;

		if (display_3Dmetrics)
		{
			auto stats = Renderer3D::GetStats();

			tris_count = stats.GetTotalTrianglesCount();
			vertices_count = stats.VerticesCount;
			indices_count = stats.IndicesCount;
			draw_calls = stats.DrawCalls;

			ImGui::Text("Max Faces x Draw Call"); ImGui::SameLine(text_separation);
			ImGui::Text("%i", Renderer3D::GetMaxFaces());
		}
		else
		{
			auto stats = Renderer2D::GetStats();

			quad_count = stats.QuadCount;
			tris_count = stats.GetTotalTrianglesCount();
			vertices_count = stats.GetTotalVerticesCount();
			indices_count = stats.GetTotalIndicesCount();
			draw_calls = stats.DrawCalls;

			ImGui::Text("Max Quads x Draw Call"); ImGui::SameLine(text_separation);
			ImGui::Text("%i", Renderer2D::GetMaxQuads());
		}


		// -- Draw Calls --
		ImGui::Text("Draw Calls"); ImGui::SameLine(text_separation);
		ImGui::Text("%i", draw_calls);


		// -- Geometry --
		ImGui::NewLine(); ImGui::NewLine();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 icons_color = ImColor(1.0f, 0.7f, 0.0f);
		float icons_size = 12.0f;
		float icons_indent = icons_size + 25.0f;
		
		// Icons Positioning & Size
		glm::vec2 position = KaimosUI::ConvertToVec2(ImGui::GetCursorScreenPos()) - glm::vec2(0.0f, 20.0f);
		
		// Quads
		if (!display_3Dmetrics)
		{
			draw_list->AddRectFilled(KaimosUI::ConvertToImVec2(position), KaimosUI::ConvertToImVec2(position + glm::vec2(icons_size)), icons_color);

			ImGui::SameLine(icons_indent);
			ImGui::Text("Quads Drawn"); ImGui::SameLine(text_separation);
			ImGui::Text("%i", quad_count);
			ImGui::NewLine();
		}

		// Triangles
		position = KaimosUI::ConvertToVec2(ImGui::GetCursorScreenPos()) + glm::vec2(icons_size/2.0f, -20.0f);
		draw_list->AddTriangleFilled(KaimosUI::ConvertToImVec2(position), KaimosUI::ConvertToImVec2(position + glm::vec2(-icons_size/2.0f, icons_size)), KaimosUI::ConvertToImVec2(position + glm::vec2(icons_size/2.0f, icons_size)), icons_color);
				
		ImGui::SameLine(icons_indent);
		ImGui::Text("Triangles Drawn"); ImGui::SameLine(text_separation);
		ImGui::Text("%i", tris_count);

		// Circle
		ImGui::NewLine();
		position = KaimosUI::ConvertToVec2(ImGui::GetCursorScreenPos()) + glm::vec2(icons_size/2.0f, -14.0f);
		draw_list->AddCircleFilled(KaimosUI::ConvertToImVec2(position), 2.5f, icons_color);
		
		ImGui::SameLine(icons_indent);
		ImGui::Text("Vertices Drawn"); ImGui::SameLine(text_separation);
		ImGui::Text("%i	(%i Indices)", vertices_count, indices_count);
	}
}
