#include "kspch.h"
#include "ToolbarPanel.h"

#include "ImGui/ImGuiUtils.h"

#include <ImGui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Kaimos {


	// ----------------------- Public Class Methods -------------------------------------------------------
	void Kaimos::ToolbarPanel::OnUIRender(Ref<Texture2D> icons_array[8], CameraController& editor_camera, float viewport_endpos)
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

		// -- Editor Camera Settings --
		ImGui::SameLine(viewport_endpos);
		DrawCameraSettings(editor_camera, btn_color, icons_array[7]->GetTextureID());

		// -- Window Pops --
		for (uint i = 0; i < 8; ++i)
			KaimosUI::UIFunctionalities::PopButton(false);

		ImGui::End();
	}


	
	// ----------------------- Private Toolbar Methods ----------------------------------------------------
	bool ToolbarPanel::DrawToolbarButton(const char* label, uint texture_id, const glm::vec4& color)
	{
		return KaimosUI::UIFunctionalities::DrawTexturedButton(label, texture_id, glm::vec2(30.0f), color);
	}

	void ToolbarPanel::DrawCameraSettings(CameraController& editor_camera, const glm::vec4& btn_color, uint btn_texture_id)
	{
		if(DrawToolbarButton("EdCamSettings", btn_texture_id, btn_color))
			ImGui::OpenPopup("Editor Camera Settings");
		
		ImGui::SetNextWindowSize({400.0f, 480.0f});
		if (ImGui::BeginPopup("Editor Camera Settings"))
		{
			// -- Lock Camera Rotation --
			static bool camera_lock = false;
			ImGui::Checkbox("Lock Camera Rotation", &camera_lock);
			editor_camera.LockRotation(camera_lock);

			// -- Camera Movement --
			float width = ImGui::GetContentRegionAvailWidth() / 2.0f;
			KaimosUI::UIFunctionalities::DrawInlineSlider("Movement Speed", "###edcammovespeed", &editor_camera.m_MoveSpeed, width, 2.0f, 10.0f, 1.0f);
			KaimosUI::UIFunctionalities::DrawInlineSlider("Rotation Speed", "###edcamrotspeed", &editor_camera.m_RotationSpeed, width, 2.0f, 5.0f, 0.1f);
			KaimosUI::UIFunctionalities::DrawInlineSlider("Panning Speed", "###edcampanspeed", &editor_camera.m_PanSpeed, width, 2.0f, 200.0f, 1.0f);
			KaimosUI::UIFunctionalities::DrawInlineSlider("Advance Camera Speed", "###edcamadvspeed", &editor_camera.m_AdvanceCameraSpeed, width, 2.0f, 100.0f, 1.0f);
			KaimosUI::UIFunctionalities::DrawInlineSlider("Max Speed Multiplier", "###edcamspm", &editor_camera.m_MaxSpeedMultiplier, width, 2.0f, 200.0f, 0.01f);
			KaimosUI::UIFunctionalities::DrawInlineSlider("Max Zoom Speed", "###edcamzspeed", &editor_camera.m_MaxZoomSpeed, width, 2.0f, 300.0f, 1.0f);

			// -- Camera Parameters --
			float fov = editor_camera.m_Camera.GetFOV();
			if (KaimosUI::UIFunctionalities::DrawInlineDragFloat("FOV", "###edcamfov", &fov, 0.05f, width, 2.0f, 15.0f, 180.0f))
				editor_camera.m_Camera.SetFOV(fov);

			float near_clip = editor_camera.m_Camera.GetNearPlane(), far_clip = editor_camera.m_Camera.GetFarPlane();
			if (KaimosUI::UIFunctionalities::DrawInlineDragFloat("Near Clip", "###edcamnclip", &near_clip, 0.01f, width, 2.0f, 0.01f, far_clip - 0.1f, "%.3f", 1.5f))
				if (near_clip < far_clip)
					editor_camera.m_Camera.SetNearPlane(near_clip);

			if (KaimosUI::UIFunctionalities::DrawInlineDragFloat("Far Clip", "###edcamfclip", &far_clip, 1.0f, width, 2.0f, near_clip + 0.1f, INFINITY, "%.3f", 1.5f))
				if (far_clip > near_clip)
					editor_camera.m_Camera.SetFarPlane(far_clip);


			// -- AR & Resolution (Read-Only) --
			glm::ivec2 view_size = editor_camera.m_Camera.GetViewportSize();
			ImGui::Text("Camera Viewport");
			ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f - 1.0f);
			ImGui::Text("%ix%i (AR: %.2f)", view_size.x, view_size.y, editor_camera.m_Camera.GetAspectRato());


			// -- Pos & Rot --
			ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine();

			glm::vec2 rot = { editor_camera.m_Pitch, editor_camera.m_Yaw };
			ImGui::Text("Camera Rotation (Pitch & Yaw)"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f - 1.0f);
			if (ImGui::DragFloat2("###edcamrotation", &rot[0], 0.01f))
				editor_camera.SetOrientation(rot.x, rot.y);

			glm::vec3 pos = editor_camera.m_Position;
			ImGui::Text("Camera Position"); ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f - 1.0f);
			if (ImGui::DragFloat3("###edcampos", &pos[0], 0.05f))
				editor_camera.SetPosition(pos);


			// -- End Menu --
			ImGui::EndPopup();
		}
	}
}
