#include "ScenePanel.h"

#include "Core/Utils/PlatformUtils.h"
#include "ImGui/ImGuiUtils.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>


// C++ Microsoft Compiler doesn't gets C++ standards, so this def. is to disable a warning for std::strncpy()
#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

namespace Kaimos {

	// ----------------------- Public Class Methods -------------------------------------------------------
	ScenePanel::ScenePanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}
	

	
	// ----------------------- Getters/Setters ------------------------------------------------------------
	void ScenePanel::SetContext(const Ref<Scene>& context)
	{
		m_SceneContext = context;
		m_SelectedEntity = {};
	}



	// ----------------------- Public Class Methods -------------------------------------------------------
	void ScenePanel::OnUIRender()
	{
		ImGui::Begin("Scene");

		// -- Entity Display --
		m_SceneContext->m_Registry.each([&](auto entity_id)
			{
				Entity entity{ entity_id , m_SceneContext.get() };
				DrawEntityNode(entity);
			});


		// -- Entity Unselection --
		if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
			m_SelectedEntity = {};

		// -- Right Click on a Blank Space --
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if(ImGui::MenuItem("Create Empty Entity"))
				m_SelectedEntity = m_SceneContext->CreateEntity("Empty Entity");

			if (ImGui::MenuItem("Create Camera"))
			{
				m_SelectedEntity = m_SceneContext->CreateEntity("Camera");
				m_SelectedEntity.AddComponent<CameraComponent>();
			}

			if (ImGui::MenuItem("Create 2D Sprite"))
			{
				m_SelectedEntity = m_SceneContext->CreateEntity("Sprite");
				m_SelectedEntity.AddComponent<SpriteRendererComponent>();
			}

			ImGui::EndPopup();
		}

		ImGui::End();

		// -- Properties Panel (Components Display) --
		ImGui::Begin("Properties");
		if (m_SelectedEntity)
			DrawComponents(m_SelectedEntity);

		ImGui::End();
	}



	// ----------------------- Private Scene Methods -----------------------------------------------------
	void ScenePanel::DrawEntityNode(Entity entity)
	{
		TagComponent& tag_comp = entity.GetComponent<TagComponent>();

		// -- Open Tree --
		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint)entity, flags, tag_comp.Tag.c_str());

		// -- Entity Selection --
		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		// -- Entity Right Click --
		bool entity_deleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Rename"))
				tag_comp.Rename = true;

			if (ImGui::MenuItem("Delete Entity"))
				entity_deleted = true;

			ImGui::EndPopup();
		}

		// -- Entity Rename --
		if (tag_comp.Rename)
		{
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));							// Set it all to 0
			std::strncpy(buffer, tag_comp.Tag.c_str(), sizeof(buffer));	// Copy tag to buffer

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag_comp.Tag = std::string(buffer);
			
			// Auto focus Input Text
			if (ImGui::IsItemHovered() || (ImGui::IsAnyWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
				ImGui::SetKeyboardFocusHere(-1);

			// Close Input Text
			if ((ImGui::IsMouseClicked(0) && ImGui::IsAnyWindowHovered()) || ImGui::IsKeyDown(KEY::ENTER) || ImGui::IsKeyDown(KEY::ESCAPE))
				tag_comp.Rename = false;
		}

		// -- Close Tree --
		if (opened)
			ImGui::TreePop();

		// -- Delete Entity --
		if (entity_deleted)
		{
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};

			m_SceneContext->DestroyEntity(entity);
		}
	}


	// ----- TODO: WTF? -- This requires of an entities/components rework
	template<typename T, typename UIFunction>
	static void DrawComponentUI(const std::string& name, Entity entity, UIFunction function)
	{
		if (entity.HasComponent<T>())
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding
										| ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;

			// -- New ID --
			static char popup_id[64];
			sprintf_s(popup_id, 64, "ComponentSettings_%s", typeid(T).name());
			ImGui::PushID(popup_id);

			// -- Content Spacing --
			float content_region = ImGui::GetContentRegionAvail().x;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4, 4 });

			float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::NewLine();
			ImGui::Separator();
			
			// -- Open Tree --
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name.c_str());
			ImGui::PopStyleVar();

			// -- Add Button with Content Spacing + Open Settings Popup --
			ImGui::SameLine(content_region + 2.0f);
			if (ImGui::Button("+", { line_height, line_height }))
				ImGui::OpenPopup("ComponentSettings");

			// -- Component Settings Popup --
			bool remove_component = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (!std::is_same<T, TransformComponent>::value)
				{
					if (ImGui::MenuItem("Remove Component"))
						remove_component = true;
				}
				ImGui::EndPopup();
			}

			// -- Close Tree --
			ImGui::PopID();
			if (open)
			{
				function(entity.GetComponent<T>());
				ImGui::TreePop();
			}

			// -- Remove Component --
			if (remove_component)
				entity.RemoveComponent<T>();
		}
	}


	// TODO: This requires of an entities/components rework + An expansion of the UIFunctionalities to reduce ImGui Code
	void ScenePanel::DrawComponents(Entity& entity)
	{
		// -- Tag Component --
		if (entity.HasComponent<TagComponent>())
		{
			// - Activation -
			if (entity.HasComponent<TransformComponent>())
			{
				TransformComponent& transf_comp = entity.GetComponent<TransformComponent>();

				bool active = transf_comp.EntityActive;
				if (ImGui::Checkbox("##ent_active", &active))
					transf_comp.EntityActive = active;
			}

			// - Tag Modification -
			std::string& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));					// Set it all to 0
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));	// Copy tag to buffer
			//strcpy_s(buffer, sizeof(buffer), tag.c_str());	// This is the same, but std::strncpy() is more like a C++ standard

			ImGui::SameLine();
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer);
		}

		// -- Add Component Button --
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		ImGui::Text("ID: %i", entity.GetID()); // ID Display
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

		// -- Transform Component --
		DrawComponentUI<TransformComponent>("Transform", entity, [](auto& component)
			{
				// - Transformation Control -
				glm::vec3 xcol = { 0.8f, 0.1f, 0.15f }, ycol = { 0.2f, 0.7f, 0.2f }, zcol = { 0.1f, 0.25f, 0.8f };
				UI::UIFunctionalities::DrawVec3UI("Position", component.Translation, xcol, ycol, zcol);

				glm::vec3 rot = glm::degrees(component.Rotation);
				UI::UIFunctionalities::DrawVec3UI("Rotation", rot, xcol, ycol, zcol);
				component.Rotation = glm::radians(rot);

				UI::UIFunctionalities::DrawVec3UI("Scale", component.Scale, xcol, ycol, zcol, 1.0f);
			});

		// -- Camera Component --
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
							camera.SetProjectionType((SceneCamera::PROJECTION_TYPE)i);
						}

						if (selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}


				if (camera.GetProjectionType() == SceneCamera::PROJECTION_TYPE::PERSPECTIVE)
				{
					float persp_FOV = camera.GetPerspectiveFOV();
					if (ImGui::DragFloat("FOV", &persp_FOV))
						camera.SetPerspectiveFOV(persp_FOV);

					float near_clip = camera.GetPerspectiveNearClip();
					float far_clip = camera.GetPerspectiveFarClip();

					if (ImGui::DragFloat("Near Clip", &near_clip))
						camera.SetPerspectiveClips(near_clip, far_clip);
					if (ImGui::DragFloat("Far Clip", &far_clip))
						camera.SetPerspectiveClips(near_clip, far_clip);
				}

				if (camera.GetProjectionType() == SceneCamera::PROJECTION_TYPE::ORTHOGRAPHIC)
				{
					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);

					float ortho_size = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &ortho_size))
						camera.SetOrthographicSize(ortho_size);

					float near_clip = camera.GetOrthographicNearClip();
					float far_clip = camera.GetOrthographicFarClip();

					if (ImGui::DragFloat("Near Clip", &near_clip))
						camera.SetOrthographicClips(near_clip, far_clip);
					if (ImGui::DragFloat("Far Clip", &far_clip))
						camera.SetOrthographicClips(near_clip, far_clip);
				}
			});		

		// -- Sprite Renderer Component --
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

				ImGui::Text("UV Offset");
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f - 1.0f);
				ImGui::SetNextItemWidth(100.0f);
				ImGui::DragFloat("##offsetx_dragfloat", &component.TextureUVOffset.x, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(100.0f);
				ImGui::DragFloat("##offsety_dragfloat", &component.TextureUVOffset.y, 0.1f, 0.0f, 0.0f, "%.2f");
			});
	}
}
