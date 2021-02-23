#include "ScenePanel.h"

#include "Core/Utils/PlatformUtils.h"
#include "Scene/Components.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

// C++ Microsoft Compiler doesn't gets C++ standards, so this def. is to disable a warning for std::strncpy()
#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

namespace Kaimos {

	ScenePanel::ScenePanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void ScenePanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectedEntity = {};
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

		// Right Click on a Blank Space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if(ImGui::MenuItem("Create Empty Entity"))
				m_SelectedEntity = m_Context->CreateEntity("Empty Entity");

			if (ImGui::MenuItem("Create Camera"))
			{
				m_SelectedEntity = m_Context->CreateEntity("Camera");
				m_SelectedEntity.AddComponent<CameraComponent>();
			}

			if (ImGui::MenuItem("Create 2D Sprite"))
			{
				m_SelectedEntity = m_Context->CreateEntity("Sprite");
				m_SelectedEntity.AddComponent<SpriteRendererComponent>();
			}

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectedEntity)
			DrawComponents(m_SelectedEntity);

		ImGui::End();
	}

	void ScenePanel::DrawEntityNode(Entity entity)
	{
		TagComponent& tag_comp = entity.GetComponent<TagComponent>();
		std::string& tag = tag_comp.Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Rename"))
				tag_comp.Rename = true;

			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (tag_comp.Rename)
		{
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));							// Set it all to 0
			std::strncpy(buffer, tag_comp.Tag.c_str(), sizeof(buffer));	// Copy tag to buffer

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer);
			
			// Auto focus Input Text
			if (ImGui::IsItemHovered() || (ImGui::IsAnyWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
				ImGui::SetKeyboardFocusHere(-1);

			// Close Input Text
			if ((ImGui::IsMouseClicked(0) && ImGui::IsAnyWindowHovered()) || ImGui::IsKeyDown(KEY::ENTER) || ImGui::IsKeyDown(KEY::ESCAPE))
				tag_comp.Rename = false;
		}

		if (opened)
			ImGui::TreePop();

		if (entityDeleted)
		{
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};

			m_Context->DestroyEntity(entity);
		}
	}


	// ----- TODO WTF?
	static void DrawVec3Control(const std::string& name, glm::vec3& value, float reset_value = 0.0f, float column_width = 100.0f)
	{
		auto bold_font = ImGui::GetIO().Fonts->Fonts[1];

		// To say to ImGui that this is kind of a new "namespace" a new ID, so we don't have problems of values modifying other values
		ImGui::PushID(name.c_str());

		// Name Column
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		// Vector Columns
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 4 });

		// Internal Imgui code, on how ImGui calculates line height
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		// X Button and DragFloat
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushFont(bold_font);

		if (ImGui::Button("X", buttonSize))
			value.x = reset_value;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		
		ImGui::SameLine();
		ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();


		// Y Button and DragFloat
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushFont(bold_font);

		if (ImGui::Button("Y", buttonSize))
			value.y = reset_value;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		// Z Button and DragFloat
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushFont(bold_font);

		if (ImGui::Button("Z", buttonSize))
			value.z = reset_value;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		// Pop Initial Pushes
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	// ----- TODO: WTF?
	template<typename T, typename UIFunction>
	static void DrawComponentUI(const std::string& name, Entity entity, UIFunction function)
	{
		if (entity.HasComponent<T>())
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding
										| ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;

			static char popup_id[64];
			sprintf_s(popup_id, 64, "ComponentSettings_%s", typeid(T).name());
			ImGui::PushID(popup_id);

			float content_region = ImGui::GetContentRegionAvail().x;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4, 4 });
			float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::NewLine();
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(content_region + 2.0f);
			if (ImGui::Button("+", { line_height, line_height }))
				ImGui::OpenPopup("ComponentSettings");

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			ImGui::PopID();
			if (open)
			{
				function(entity.GetComponent<T>());
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}


	void ScenePanel::DrawComponents(Entity entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;
		
		// Tag Component
		if (entity.HasComponent<TagComponent>())
		{
			std::string& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));					// Set it all to 0
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));	// Copy tag to buffer
			//strcpy_s(buffer, sizeof(buffer), tag.c_str());	// This is the same, but std::strncpy() is more like a C++ standard

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer);
		}

		// Add Component Button
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				if (!m_SelectedEntity.HasComponent<CameraComponent>())
					m_SelectedEntity.AddComponent<CameraComponent>();
				else
					KS_EDITOR_WARN("CameraComponent already exists in the Entity!");

				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer"))
			{
				if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
					m_SelectedEntity.AddComponent<SpriteRendererComponent>();
				else
					KS_EDITOR_WARN("SpriteRendererComponent already exists in the Entity!");

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		// Transform Component
		DrawComponentUI<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Position", component.Translation);

				glm::vec3 rot = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rot);
				component.Rotation = glm::radians(rot);

				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		// Camera Component
		DrawComponentUI<CameraComponent>("Camera", entity, [](auto& component)
			{
				SceneCamera& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);

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
					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);

					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float nearClip = camera.GetOrthographicNearClip();
					float farClip = camera.GetOrthographicFarClip();

					if (ImGui::DragFloat("Near Clip", &nearClip))
						camera.SetOrthographicClips(nearClip, farClip);
					if (ImGui::DragFloat("Far Clip", &farClip))
						camera.SetOrthographicClips(nearClip, farClip);
				}
			});		

		// Sprite Renderer Component
		DrawComponentUI<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGuiColorEditFlags color_flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs;
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color), color_flags);

				const uint id = component.SpriteTexture == nullptr ? 0 : component.SpriteTexture->GetTextureID();
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

				ImGui::NewLine();
				ImGui::Text("Texture");
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));

				if (ImGui::ImageButton((ImTextureID)id, { 80.0f, 80.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f }, 0, { 1.0f, 0.0f, 1.0f, 1.0f }))
				{
					std::string texture_file = FileDialogs::OpenFile("Texture (*.png)\0*.png\0");
					if (!texture_file.empty())
						component.SetTexture(texture_file);
				}

				ImGui::PopStyleColor(3);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });

				if (ImGui::Button("x", { 20.0f, 80.0f }))
					component.RemoveTexture();

				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar();

				ImGui::NewLine();
				ImGui::Text("Tiling");
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f - 1.0f);
				ImGui::SetNextItemWidth(100.0f);
				ImGui::DragFloat("##tiling_dragfloat", &component.TextureTiling, 0.1f, 0.0f, 0.0f, "%.2f");
			});
	}
}
