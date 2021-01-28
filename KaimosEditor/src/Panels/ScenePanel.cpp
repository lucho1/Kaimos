#include "ScenePanel.h"

#include "Scene/Components.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace Kaimos {

	ScenePanel::ScenePanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void ScenePanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void ScenePanel::OnUIRender()
	{
		ImGui::Begin("Scene");

		m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID , m_Context.get() };
				DrawEntityNode(entity);				
			});

		if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
			m_SelectedEntity = {};

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectedEntity)
			DrawComponents(m_SelectedEntity);

		ImGui::End();
	}

	void ScenePanel::DrawEntityNode(Entity entity)
	{
		std::string& tag = entity.GetComponent<TagComponent>().Tag;		

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		bool opened = ImGui::TreeNodeEx((void*)(uint)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		if (opened)
			ImGui::TreePop();
	}


	// -----
	static void DrawVec3Control(const std::string& name, glm::vec3& value, float reset_value = 0.0f, float column_width = 100.0f)
	{
		// To say to ImGui that this is kind of a new "namespace" a new ID, so we don't have problems of values modifying other values
		ImGui::PushID(name.c_str());

		// Name Column
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		// Vector Columns
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		// Internal Imgui code, on how ImGui calculates line height
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		// X Button and DragFloat
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });

		if (ImGui::Button("X", buttonSize))
			value.x = reset_value;

		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Y Button and DragFloat
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
			value.y = reset_value;

		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Z Button and DragFloat
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });

		if (ImGui::Button("Z", buttonSize))
			value.z = reset_value;

		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void ScenePanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			std::string& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));				// Set it all to 0
			strcpy_s(buffer, sizeof(buffer), tag.c_str());	// Copy tag to buffer

			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer);
		}

		if (entity.HasComponent<TransformComponent>())
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), flags, "Transform"))
			{
				TransformComponent& transform = entity.GetComponent<TransformComponent>();
				DrawVec3Control("Position", transform.Translation);

				glm::vec3 rot = glm::degrees(transform.Rotation);
				DrawVec3Control("Rotation", rot);
				transform.Rotation = glm::radians(rot);
				
				DrawVec3Control("Scale", transform.Scale, 1.0f);
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<CameraComponent>())
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), flags, "Transform"))
			{
				CameraComponent& camera_comp = entity.GetComponent<CameraComponent>();
				SceneCamera& camera = camera_comp.Camera;

				ImGui::Checkbox("Primary", &camera_comp.Primary);

				const char* projection_options[] = { "Perspective", "Orthographic" };
				const char* current_projection = projection_options[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", current_projection))
				{
					for (uint i = 0; i < 2; ++i)
					{
						bool selected = current_projection == projection_options[i];
						if (ImGui::Selectable(projection_options[i], selected))
						{
							current_projection = projection_options[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (selected)
							ImGui::SetItemDefaultFocus();
					}
					
					ImGui::EndCombo();
				}


				if (camera.GetProjectionType() == SceneCamera::ProjectionType::PERSPECTIVE)
				{
					float perspFOV = camera.GetPerspectiveFOV();
					if (ImGui::DragFloat("FOV", &perspFOV))
						camera.SetPerspectiveFOV(perspFOV);

					float nearClip = camera.GetPerspectiveNearClip();
					float farClip = camera.GetPerspectiveFarClip();

					if (ImGui::DragFloat("Near Clip", &nearClip))
						camera.SetPerspectiveClips(nearClip, farClip);
					if (ImGui::DragFloat("Far Clip", &farClip))
						camera.SetPerspectiveClips(nearClip, farClip);
				}
				
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::ORTHOGRAPHIC)
				{
					ImGui::Checkbox("Fixed Aspect Ratio", &camera_comp.FixedAspectRatio);

					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float nearClip = camera.GetOrthographicNearClip();
					float farClip = camera.GetOrthographicFarClip();

					if (ImGui::DragFloat("Near Clip", &nearClip))
						camera.SetOrthographicClips(nearClip, farClip);
					if(ImGui::DragFloat("Far Clip", &farClip))
						camera.SetOrthographicClips(nearClip, farClip);
				}

				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), flags, "Sprite Renderer"))
			{
				glm::vec4& color = entity.GetComponent<SpriteRendererComponent>().Color;
				ImGui::ColorEdit4("Color", glm::value_ptr(color), flags);
				ImGui::TreePop();
			}
		}
	}
}