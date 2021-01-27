#include "ScenePanel.h"

#include "Scene/Components.h"

#include <imgui.h>
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
				glm::mat4& transform = entity.GetComponent<TransformComponent>().Transform;
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f, -INFINITY, INFINITY, "%.2f");
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
	}
}