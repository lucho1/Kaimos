#include "kspch.h"
#include "ToolbarPanel.h"

#include "ImGui/ImGuiUtils.h"

#include <ImGui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Kaimos {


	// ----------------------- Public Class Methods -------------------------------------------------------
	void Kaimos::ToolbarPanel::OnUIRender()
	{
		// -- Window Flags --
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus;

		ImGuiWindowClass wnd_class;
		wnd_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;
		ImGui::SetNextWindowClass(&wnd_class);

		// -- Toolbar --
		ImGui::Begin("Toolbar", nullptr, flags);
		ImGui::SetCursorPos({ 150.0f, 6.0f });


		// Buttons Colors
		glm::vec4 btn_color = KaimosUI::ConvertToVec4(ImGui::GetStyle().Colors[ImGuiCol_Button]);
		glm::vec4 active_color = KaimosUI::ConvertToVec4(ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
		
		// Guizmo Operation Buttons
		if (DrawToolbarButton("U", 0, m_SelectedOperation == -1 ? active_color : btn_color))
			m_SelectedOperation = -1;

		ImGui::SameLine();
		if (DrawToolbarButton("T", 0, m_SelectedOperation == 0 ? active_color : btn_color))
			m_SelectedOperation = 0;

		ImGui::SameLine();
		if (DrawToolbarButton("R", 0, m_SelectedOperation == 1 ? active_color : btn_color))
			m_SelectedOperation = 1;

		ImGui::SameLine();
		if (DrawToolbarButton("S", 0, m_SelectedOperation == 2 ? active_color : btn_color))
			m_SelectedOperation = 2;
		
		// Snap Button
		ImGui::SameLine(305.0f);
		if (DrawToolbarButton("Sn", 0, m_Snap ? active_color : btn_color))
			m_Snap = !m_Snap;

		// Guizmo Mode Buttons
		ImGui::SameLine(358.0f);
		if (DrawToolbarButton("L", 0, m_WorldMode ? btn_color : active_color))
			m_WorldMode = false;

		ImGui::SameLine();
		if (DrawToolbarButton("W", 0, m_WorldMode ? active_color : btn_color))
			m_WorldMode = true;


		// -- Window Pops --
		for (uint i = 0; i < 7; ++i)
			KaimosUI::UIFunctionalities::PopButton(false);

		ImGui::End();
	}



	// ----------------------- Private Class Methods ------------------------------------------------------
	bool ToolbarPanel::DrawToolbarButton(const char* label, uint texture_id, const glm::vec4& color)
	{
		return KaimosUI::UIFunctionalities::DrawTexturedButton(label, texture_id, glm::vec2(23.0f), color);
	}
}
