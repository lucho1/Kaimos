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
	void ScenePanel::OnUIRender(bool& closing_bool)
	{
		// -- Scene Tab --
		ImGui::Begin("Scene", &closing_bool);
		ImGui::TextColored({0.65f, 0.65f, 0.65f, 1.0f}, "%s", m_SceneContext->GetName().c_str());

		// -- Entity Display --
		m_SceneContext->m_Registry.each([&](auto entity_id)
			{
				Entity entity{ entity_id , m_SceneContext.get() };

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 1.0f));
				DrawEntityNode(entity);
				ImGui::PopStyleVar();
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
		ImGui::Begin("Entity Properties", &closing_bool);
		if (m_SelectedEntity)
			DrawComponents(m_SelectedEntity);

		ImGui::End();
	}



	// ----------------------- Private Scene Methods -----------------------------------------------------
	void ScenePanel::DrawEntityNode(Entity entity)
	{
		TagComponent& tag_comp = entity.GetComponent<TagComponent>();

		// -- Open Tree --
		ImGui::AlignTextToFramePadding();
		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)entity.GetID(), flags, tag_comp.Tag.c_str());

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
			// Input Text
			KaimosUI::UIFunctionalities::DrawInputText("##Tag", tag_comp.Tag);
			
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
			ImGui::NewLine(); ImGui::Separator();
			
			// -- Open Tree --
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name.c_str());
			ImGui::PopStyleVar();

			// -- Add Button with Content Spacing + Open Settings Popup --
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			ImGui::SameLine(content_region + 2.0f);
			if (ImGui::Button("+", { line_height, line_height }))
				ImGui::OpenPopup("ComponentSettings");

			ImGui::PopFont();

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
				ImGui::Checkbox("##EntActive", &entity.GetComponent<TransformComponent>().EntityActive);

			// - Tag Modification -
			ImGui::SameLine();
			KaimosUI::UIFunctionalities::DrawInputText("##Tag", entity.GetComponent<TagComponent>().Tag, ImGui::GetContentRegionAvailWidth() / 1.5f);
		}

		// -- Add Component Button + Popup --
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component", { ImGui::GetContentRegionAvail().x, ImGui::GetItemRectSize().y }))
			ImGui::OpenPopup("AddComponent");

		ImGui::Text("ID: %i", entity.GetID()); // ID Display
		if (ImGui::BeginPopup("AddComponent"))
		{
			// Different Entities to Add (Camera, Sprite...)
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

				// Position
				KaimosUI::UIFunctionalities::DrawVec3UI("Position", component.Translation, xcol, ycol, zcol);

				// Rotation
				glm::vec3 rot = glm::degrees(component.Rotation);
				KaimosUI::UIFunctionalities::DrawVec3UI("Rotation", rot, xcol, ycol, zcol);
				component.Rotation = glm::radians(rot);

				// Scale
				KaimosUI::UIFunctionalities::DrawVec3UI("Scale", component.Scale, xcol, ycol, zcol, 1.0f);
			});


		// -- Camera Component --
		DrawComponentUI<CameraComponent>("Camera", entity, [](auto& component) // TODO: Clean this
			{
				// Common Parameters
				Camera& camera = component.Camera;
				ImGui::Checkbox("Primary", &component.Primary); // TODO: Camera Rework, Primary camera should be handled in scene
				ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);


				// Projection Type Dropdown
				uint current_proj_type = (uint)camera.GetProjectionType();
				const char* projection_options[] = { "Perspective", "Orthographic" };				
				const char* current_projection = projection_options[current_proj_type];

				if (KaimosUI::UIFunctionalities::DrawDropDown("Projection", projection_options, 2, current_projection, current_proj_type, ImGui::CalcItemWidth() / 4.0f))
				{
					// Set Projection Values
					glm::vec2 stored_planes = camera.GetStoredPlanes((CAMERA_PROJECTION)current_proj_type);
					if (current_proj_type == (uint)Kaimos::CAMERA_PROJECTION::PERSPECTIVE)
						camera.SetPerspectiveParameters(camera.GetFOV(), stored_planes.x, stored_planes.y);
					else
						camera.SetOrthographicParameters(camera.GetSize(), stored_planes.x, stored_planes.y);
				}			
				

				// Camera Values UI
				if (camera.GetProjectionType() == Kaimos::CAMERA_PROJECTION::PERSPECTIVE)
				{
					float FOV = camera.GetFOV();
					if (KaimosUI::UIFunctionalities::DrawInlineDragFloat("FOV", "###fov", &FOV, 0.05f, ImGui::CalcItemWidth() / 4.0f, 2.0f, 15.0f, 180.0f, "%.1f"))
						camera.SetFOV(FOV);
				}

				float min_clip = 0.001f, far_clip_speed = 1.0f;
				if (camera.GetProjectionType() == Kaimos::CAMERA_PROJECTION::ORTHOGRAPHIC)
				{
					min_clip = -INFINITY; far_clip_speed = 0.01f;
					float ortho_size = camera.GetSize();
					if (KaimosUI::UIFunctionalities::DrawInlineDragFloat("Size", "###size", &ortho_size, 0.05f, ImGui::CalcItemWidth() / 4.0f, 2.0f, 1.0f, INFINITY, "%.1f"))
						camera.SetSize(ortho_size);
				}

				glm::vec2 resolution = camera.GetViewportSize();
				if (KaimosUI::UIFunctionalities::DrawInlineDragFloat2("Width & Height", "###camresolutionwh", resolution, 1.0f, ImGui::CalcItemWidth() / 4.0f, 2.0f, 120.0f, 4096.0f, "%.0f"))
					camera.SetViewport((uint)resolution.x, (uint)resolution.y);

				ImGui::SameLine(); ImGui::Text("(AR: %.2f)", camera.GetAspectRato());


				//TODO: This should keep the values of both types of cameras
				float near_clip = camera.GetNearPlane(), far_clip = camera.GetFarPlane();
				if (KaimosUI::UIFunctionalities::DrawInlineDragFloat("Near Clip", "###nclip", &near_clip, 0.01f, ImGui::CalcItemWidth() / 4.0f, 2.0f, min_clip, far_clip - 0.1f, "%.3f", 1.5f))
					if (near_clip < far_clip)
						camera.SetNearPlane(near_clip);

				if (KaimosUI::UIFunctionalities::DrawInlineDragFloat("Far Clip", "###fclip", &far_clip, far_clip_speed, ImGui::CalcItemWidth() / 4.0f, 2.0f, near_clip + 0.1f, INFINITY), "%.3f", 1.5f)
					if (far_clip > near_clip)
						camera.SetFarPlane(far_clip);
			});


		// -- Sprite Renderer Component --
		DrawComponentUI<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				// Color Picker for Sprite Color
				ImGuiColorEditFlags color_flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs;
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color), color_flags);

				// Texture Button for Sprite Texture
				const uint id = component.SpriteTexture == nullptr ? 0 : component.SpriteTexture->GetTextureID();
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

				ImGui::NewLine();
				ImGui::Text("Texture");
				ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f);

				if (KaimosUI::UIFunctionalities::DrawTexturedButton("###sprite_texture_btn", id, glm::vec2(80.0f), glm::vec3(0.1f)))
				{
					std::string texture_file = FileDialogs::OpenFile("Texture (*.png)\0*.png\0");
					if (!texture_file.empty())
						component.SetTexture(texture_file);
				}

				KaimosUI::UIFunctionalities::PopButton(false);

				// Remove Texture Button
				ImGui::SameLine();
				if (KaimosUI::UIFunctionalities::DrawColoredButton("X", { 20.0f, 80.0f }, glm::vec3(0.2f), true))
					component.RemoveTexture();
				
				KaimosUI::UIFunctionalities::PopButton(true);				
				ImGui::PopStyleVar();

				// Tiling & UV Offset Drag Floats
				ImGui::NewLine();
				KaimosUI::UIFunctionalities::DrawInlineDragFloat("Tiling", "##tiling", &component.TextureTiling, 0.1f, 100.0f);
				KaimosUI::UIFunctionalities::DrawInlineDragFloat2("UV Offset", "##uv_offset", component.TextureUVOffset, 0.1f, 208.0f);
			});
	}
}
