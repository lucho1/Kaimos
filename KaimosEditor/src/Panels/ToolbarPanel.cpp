#include "kspch.h"
#include "ToolbarPanel.h"

#include "ImGui/ImGuiUtils.h"

#include <ImGui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Kaimos {


	// ----------------------- Public Class Methods -------------------------------------------------------
	void Kaimos::ToolbarPanel::OnUIRender(Ref<Texture2D> icons_array[7])
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
		if (DrawToolbarButton("U", icons_array[0]->GetTextureID(), m_SelectedOperation == -1 ? active_color : btn_color))
			m_SelectedOperation = -1;

		ImGui::SameLine();
		if (DrawToolbarButton("T", icons_array[1]->GetTextureID(), m_SelectedOperation == 0 ? active_color : btn_color))
			m_SelectedOperation = 0;		

		ImGui::SameLine();
		if (DrawToolbarButton("R", icons_array[2]->GetTextureID(), m_SelectedOperation == 1 ? active_color : btn_color))
			m_SelectedOperation = 1;

		ImGui::SameLine();
		if (DrawToolbarButton("S", icons_array[3]->GetTextureID(), m_SelectedOperation == 2 ? active_color : btn_color))
			m_SelectedOperation = 2;
		
		// Snap Button
		ImGui::SameLine(335.0f);
		if (DrawToolbarButton("Sn", icons_array[4]->GetTextureID(), m_Snap ? active_color : btn_color))
			m_Snap = !m_Snap;

		// Guizmo Mode Buttons
		ImGui::SameLine(395.0f);
		if (DrawToolbarButton("L", icons_array[5]->GetTextureID(), m_WorldMode ? btn_color : active_color))
			m_WorldMode = false;

		ImGui::SameLine();
		if (DrawToolbarButton("W", icons_array[6]->GetTextureID(), m_WorldMode ? active_color : btn_color))
			m_WorldMode = true;


		// -- Window Pops --
		for (uint i = 0; i < 7; ++i)
			KaimosUI::UIFunctionalities::PopButton(false);

		ImGui::End();
	}



	// ----------------------- Private Class Methods ------------------------------------------------------
	bool ToolbarPanel::DrawToolbarButton(const char* label, uint texture_id, const glm::vec4& color)
	{
		return KaimosUI::UIFunctionalities::DrawTexturedButton(label, texture_id, glm::vec2(30.0f), color);
	}
}
