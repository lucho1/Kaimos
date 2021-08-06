#include "ScenePanel.h"

#include "Core/Utils/PlatformUtils.h"
#include "Core/Resources/ResourceManager.h"
#include "ImGui/ImGuiUtils.h"

#include "Renderer/Resources/Mesh.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>


// C++ Microsoft Compiler doesn't gets C++ standards, so this def. is to disable a warning for std::strncpy()
#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

namespace Kaimos {

	// ----------------------- Public Class Methods -------------------------------------------------------
	ScenePanel::ScenePanel(const Ref<Scene>& context, MaterialEditorPanel* material_editor_panel)
	{
		m_SelectedEntity = {};
		m_KMEPanel = material_editor_panel;
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
		ImGui::SameLine();
		if(m_SceneContext->GetPrimaryCamera())
			ImGui::Text("Current camera: %i", m_SceneContext->GetPrimaryCamera().GetID());
		else
			ImGui::Text("Current camera: NULL");

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

			if (ImGui::BeginMenu("Create Geometry"))
			{
				if (ImGui::MenuItem("2D Sprite"))
				{
					m_SelectedEntity = m_SceneContext->CreateEntity("Sprite");
					m_SelectedEntity.AddComponent<SpriteRendererComponent>();
				}

				if (ImGui::MenuItem("3D Mesh"))
				{
					m_SelectedEntity = m_SceneContext->CreateEntity("Mesh");
					m_SelectedEntity.AddComponent<MeshRendererComponent>();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Create Light"))
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					m_SelectedEntity = m_SceneContext->CreateEntity("Directional Light");
					m_SelectedEntity.AddComponent<DirectionalLightComponent>();
				}

				if (ImGui::MenuItem("Point Light"))
				{
					m_SelectedEntity = m_SceneContext->CreateEntity("Point Light");
					m_SelectedEntity.AddComponent<PointLightComponent>();
				}

				ImGui::EndMenu();
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
	void ScenePanel::DrawComponentUI(const std::string& name, Entity entity, UIFunction function)
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

			if (ImGui::MenuItem("Mesh Renderer"))
			{
				if (!m_SelectedEntity.HasComponent<MeshRendererComponent>())
					m_SelectedEntity.AddComponent<MeshRendererComponent>();
				else
					KS_EDITOR_WARN("MeshRendererComponent already exists in the Entity!");

				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Directional Light"))
			{
				if (!m_SelectedEntity.HasComponent<DirectionalLightComponent>())
					m_SelectedEntity.AddComponent<DirectionalLightComponent>();
				else
					KS_EDITOR_WARN("DirectionalLightComponent already exists in the Entity!");

				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Point Light"))
			{
				if (!m_SelectedEntity.HasComponent<PointLightComponent>())
					m_SelectedEntity.AddComponent<PointLightComponent>();
				else
					KS_EDITOR_WARN("PointLightComponent already exists in the Entity!");

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


		// -- Light Components --
		DrawComponentUI<DirectionalLightComponent>("Directional Light", entity, [](auto& component)
			{
				// Light Base Stuff
				Light* light = component.Light.get();

				ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs;
				ImGui::ColorEdit4("Irradiance", glm::value_ptr(light->Irradiance), flags);
				Kaimos::KaimosUI::UIFunctionalities::DrawInlineSlider("Intensity", "###light_intensity", &light->Intensity);

			});

		DrawComponentUI<PointLightComponent>("Light", entity, [](auto& component)
			{
				// Light Base Stuff
				PointLight* light = component.Light.get();

				ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs;
				ImGui::ColorEdit4("Irradiance", glm::value_ptr(light->Irradiance), flags);
				Kaimos::KaimosUI::UIFunctionalities::DrawInlineSlider("Intensity", "###light_intensity", &light->Intensity);

				// Light Type Selector

				// Point Light Stuff
				float plight_radius = light->GetRadius();
				if (Kaimos::KaimosUI::UIFunctionalities::DrawInlineDragFloat("Radius", "###plight_radius", &plight_radius))
					light->SetRadius(plight_radius);

				Kaimos::KaimosUI::UIFunctionalities::DrawInlineDragFloat("Falloff Intensity", "###plight_falloff", &light->FalloffMultiplier);
			});


		// -- Camera Component --
		DrawComponentUI<CameraComponent>("Camera", entity, [&](auto& component)
			{
				Camera& camera = component.Camera;
				
				// Common Parameters
				bool primary = component.Primary;
				if (ImGui::Checkbox("Primary", &primary))
				{
					if (primary)
						m_SceneContext->SetPrimaryCamera(entity);
					else
						m_SceneContext->UnsetPrimaryCamera();
				}


				ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);


				// Projection Type Dropdown
				uint current_proj_type = (uint)camera.GetProjectionType();
				const std::vector<std::string> projection_options = { "Perspective", "Orthographic" };
				std::string current_projection = projection_options[current_proj_type];

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

				float near_clip = camera.GetNearPlane(), far_clip = camera.GetFarPlane();
				if (KaimosUI::UIFunctionalities::DrawInlineDragFloat("Near Clip", "###nclip", &near_clip, 0.01f, ImGui::CalcItemWidth() / 4.0f, 2.0f, min_clip, far_clip - 0.1f, "%.3f", 1.5f))
					if (near_clip < far_clip)
						camera.SetNearPlane(near_clip);

				if (KaimosUI::UIFunctionalities::DrawInlineDragFloat("Far Clip", "###fclip", &far_clip, far_clip_speed, ImGui::CalcItemWidth() / 4.0f, 2.0f, near_clip + 0.1f, INFINITY), "%.3f", 1.5f)
					if (far_clip > near_clip)
						camera.SetFarPlane(far_clip);
			});


		// -- Sprite Renderer Component --
		DrawComponentUI<SpriteRendererComponent>("Sprite Renderer", entity, [&](auto& component)
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_FramePadding
					| ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;

				bool open = ImGui::TreeNodeEx("###mattreenode", flags, "Material");
				if (open)
				{
					Ref<Material> material = Renderer::GetMaterial(component.SpriteMaterialID);
					if (!material)
					{
						ImGui::TreePop();
						return;
					}

					ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

					// Materials Dropdown
					uint current_material_index = 0;
					std::string current_material_name = material->GetName();
					std::vector<std::string> material_names;
					uint mats_size = Renderer::GetMaterialsQuantity();

					for (uint i = 0; i < mats_size; ++i)
					{
						Ref<Material> mat = Renderer::GetMaterialFromIndex(i);
						if (mat)
						{
							material_names.push_back(mat->GetName());
							if (mat->GetID() == material->GetID())
								current_material_index = i;
						}
					}

					if (KaimosUI::UIFunctionalities::DrawDropDown("Material", material_names, material_names.size(), current_material_name, current_material_index, 135.5f, 2.45f))
					{
						// Set Material
						Ref<Material> selected_material = Renderer::GetMaterialFromIndex(current_material_index);
						if (selected_material)
						{
							component.SetMaterial(selected_material->GetID());
							material = selected_material;
						}
					}

					// Create Material Modal Screen
					ImGui::SameLine();
					if (ImGui::Button("New Material", ImVec2(100.0f, 28.25f)))
						ImGui::OpenPopup("Material Creation");
					if (ImGui::BeginPopupModal("Material Creation", NULL, ImGuiWindowFlags_NoResize))
					{
						static char mtl_name[128] = "";
						ImGui::Text("You are about to create a Material, Choose a Name for it:");						
						ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
						ImGui::InputTextWithHint("###MtlNameInputTxt", "Material Name", mtl_name, IM_ARRAYSIZE(mtl_name), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
						ImGui::PopItemWidth();

						//static int item = 1;
						//ImGui::Combo("Combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");

						ImGui::NewLine();
						if (ImGui::Button("Create", ImVec2(55.0f, 28.25f)))
							ImGui::OpenPopup("MaterialCreated");

						bool close_popup = false;
						if (ImGui::BeginPopupModal("MaterialCreated", NULL, ImGuiWindowFlags_NoResize))
						{
							const std::string mtl_name_str = mtl_name;
							if (mtl_name_str.empty())
							{
								ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "A Material without name cannot be created!");
								ImGui::NewLine();
								ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - 47.0f / 2.0f);
								if (ImGui::Button("Close"))
									ImGui::CloseCurrentPopup();
							}
							else
							{
								ImGui::Text("Material '%s' Created!", mtl_name);
								ImGui::NewLine(); ImGui::NewLine();
								ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - 47.0f / 2.0f);
								if (ImGui::Button("Close"))
								{
									close_popup = true;
									ImGui::CloseCurrentPopup();
									Renderer::CreateMaterial(mtl_name_str);
								}
							}

							ImGui::EndPopup();
						}
						
						ImGui::SameLine(ImGui::GetWindowSize().x - 75.0f);
						if (ImGui::Button("Cancel") || close_popup)
							ImGui::CloseCurrentPopup();

						ImGui::EndPopup();
					}


					// Texture Info
					if (const Ref<Texture2D>& texture = material->GetTexture())
					{
						std::string tex_path = material->GetTexturePath();
						std::string tex_name = tex_path;
						if (!tex_path.empty())
							tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1) + 1);

						ImGui::Text("Texture (ID %i):\t\t %s (%ix%i)", texture->GetTextureID(), tex_name.c_str(), texture->GetWidth(), texture->GetHeight());
					}
					else
						ImGui::Text("Texture:\t\t\t\t\tNone");

					// Tiling & UV Offset Info
					glm::ivec4 col = material->Color * 255.0f;
					ImGui::Text("Color:\t\t\t\t\t\tRGBA(%i, %i, %i, %i)", col.r, col.g, col.b, col.a);
					ImGui::Text("Material ID:\t\t\t %i", material->GetID());
					ImGui::Text("Material Graph ID: %i", material->GetAttachedGraphID());

					ImGui::PopFont();

					// Open in Material Editor Button
					ImGui::NewLine(); ImGui::NewLine();
					if (Renderer::IsDefaultMaterial(material->GetID()))
					{
						std::string no_def_mat_str = "Cannot Modify Default Material!";
						float text_size_x = ImGui::CalcTextSize(no_def_mat_str.c_str()).x;

						ImGui::SameLine(ImGui::GetWindowSize().x * 0.5f - text_size_x * 0.5f);
						ImGui::TextColored({ 0.8f, 0.8f, 0.2f, 1.0f }, no_def_mat_str.c_str());
					}
					else
					{
						float btn_width = 196.0f;
						ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.5f - btn_width * 0.5f);
						if (ImGui::Button("Open in Material Editor", ImVec2(btn_width, 25.0f)))
							m_KMEPanel->SetGraphToModifyFromMaterial(material->GetID());
					}

					ImGui::NewLine();

					ImGui::TreePop();
				}
			});


		// -- Mesh Renderer Component --
		DrawComponentUI<MeshRendererComponent>("Mesh Renderer", entity, [&](auto& component)
			{
				// Push Font & Get Mesh
				Ref<Mesh> mesh = Resources::ResourceManager::GetMesh(component.MeshID);

				// Set Meshes Dropdown
				const std::unordered_map<uint, Ref<Mesh>> meshes = Resources::ResourceManager::GetMeshesMap();

				uint current_mesh_index = 0;
				std::string current_mesh_name = mesh ? mesh->GetName() : "None";
				std::vector<std::string> meshes_names;

				uint ind = 0;
				for (auto& m : meshes)
				{
					meshes_names.push_back(m.second->GetName());
					if (mesh && m.second->GetID() == mesh->GetID())
						current_mesh_index = ind;

					++ind;
				}

				meshes_names.push_back("None");

				// Draw Meshes Dropdown
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
				if (KaimosUI::UIFunctionalities::DrawDropDown("Mesh", meshes_names, meshes_names.size(), current_mesh_name, current_mesh_index, 135.5f, 2.45f))
				{
					// Set Mesh
					if (current_mesh_index == (meshes_names.size() - 1))
						component.RemoveMesh();
					else
					{
						Ref<Mesh> selected_mesh = Resources::ResourceManager::GetMeshFromIndex(current_mesh_index);
						if (selected_mesh)
						{
							component.SetMesh(selected_mesh->GetID());
							mesh = selected_mesh;
						}
					}
				}

				// Draw Mesh Info
				if (!mesh)
				{
					ImGui::PopFont();
					return;
				}


				// Mesh Info.
				ImGui::Text("Mesh:\t\t\t\t\t  %s", mesh->GetName().c_str());
				ImGui::Text("Mesh ID:\t\t\t\t %i", mesh->GetID());
				ImGui::Text("Parent Model:\t  %s", mesh->GetParentModelName().c_str());
				ImGui::Text("Parent Mesh:\t\t%s", mesh->GetParentMeshName().c_str());

				// Get Material
				Ref<Material> material = Renderer::GetMaterial(component.MaterialID);
				if (!material)
				{
					ImGui::PopFont();
					return;
				}

				// Set Materials Dropdown
				uint mats_size = Renderer::GetMaterialsQuantity();

				uint current_material_index = 0;
				std::string current_material_name = material->GetName();
				std::vector<std::string> material_names;

				for (uint i = 0; i < mats_size; ++i)
				{
					Ref<Material> mat = Renderer::GetMaterialFromIndex(i);
					if (mat)
					{
						material_names.push_back(mat->GetName());
						if (mat->GetID() == material->GetID())
							current_material_index = i;
					}
				}

				// Draw Materials Dropdown
				if (KaimosUI::UIFunctionalities::DrawDropDown("Material", material_names, material_names.size(), current_material_name, current_material_index, 135.5f, 2.45f))
				{
					// Set Material
					Ref<Material> selected_material = Renderer::GetMaterialFromIndex(current_material_index);
					if (selected_material)
					{
						component.SetMaterial(selected_material->GetID());
						material = selected_material;
					}
				}

				// Create Material Modal Screen
				ImGui::SameLine();
				if (ImGui::Button("New Material", ImVec2(100.0f, 28.25f)))
					ImGui::OpenPopup("Material Creation");
				if (ImGui::BeginPopupModal("Material Creation", NULL, ImGuiWindowFlags_NoResize))
				{
					static char mtl_name[128] = "";
					ImGui::Text("You are about to create a Material, Choose a Name for it:");
					ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
					ImGui::InputTextWithHint("###MtlNameInputTxt", "Material Name", mtl_name, IM_ARRAYSIZE(mtl_name), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
					ImGui::PopItemWidth();

					//static int item = 1;
					//ImGui::Combo("Combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");

					ImGui::NewLine();
					if (ImGui::Button("Create", ImVec2(55.0f, 28.25f)))
						ImGui::OpenPopup("MaterialCreated");

					bool close_popup = false;
					if (ImGui::BeginPopupModal("MaterialCreated", NULL, ImGuiWindowFlags_NoResize))
					{
						const std::string mtl_name_str = mtl_name;
						if (mtl_name_str.empty())
						{
							ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "A Material without name cannot be created!");
							ImGui::NewLine(); ImGui::NewLine();
							ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - 47.0f / 2.0f);
							if (ImGui::Button("Close"))
								ImGui::CloseCurrentPopup();
						}
						else
						{
							ImGui::Text("Material '%s' Created!", mtl_name);
							ImGui::NewLine();
							ImGui::SameLine(ImGui::GetWindowSize().x / 2.0f - 47.0f / 2.0f);
							if (ImGui::Button("Close"))
							{
								close_popup = true;
								ImGui::CloseCurrentPopup();
								Renderer::CreateMaterial(mtl_name_str);
							}
						}

						ImGui::EndPopup();
					}

					ImGui::SameLine(ImGui::GetWindowSize().x - 75.0f);
					if (ImGui::Button("Cancel") || close_popup)
						ImGui::CloseCurrentPopup();

					ImGui::EndPopup();
				}

				// Texture Info
				if (const Ref<Texture2D>& texture = material->GetTexture())
				{
					std::string tex_path = material->GetTexturePath();
					std::string tex_name = tex_path;
					if (!tex_path.empty())
						tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1) + 1);

					ImGui::Text("Texture (ID %i):\t\t %s (%ix%i)", texture->GetTextureID(), tex_name.c_str(), texture->GetWidth(), texture->GetHeight());
				}
				else
					ImGui::Text("Texture:\t\t\t\t\tNone");

				// General Info
				glm::ivec4 col = material->Color * 255.0f;
				ImGui::Text("Color:\t\t\t\t\t\tRGBA(%i, %i, %i, %i)", col.r, col.g, col.b, col.a);
				ImGui::Text("Material ID:\t\t\t %i", material->GetID());
				ImGui::Text("Material Graph ID: %i", material->GetAttachedGraphID());

				ImGui::PopFont();

				// Open in Material Editor Button
				ImGui::NewLine(); ImGui::NewLine();
				if (Renderer::IsDefaultMaterial(material->GetID()))
				{
					std::string no_def_mat_str = "Cannot Modify Default Material!";
					float text_size_x = ImGui::CalcTextSize(no_def_mat_str.c_str()).x;

					ImGui::SameLine(ImGui::GetWindowSize().x * 0.5f - text_size_x * 0.5f);
					ImGui::TextColored({ 0.8f, 0.8f, 0.2f, 1.0f }, no_def_mat_str.c_str());
				}
				else
				{
					float btn_width = 196.0f;
					ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.5f - btn_width * 0.5f);
					if (ImGui::Button("Open in Material Editor", ImVec2(btn_width, 25.0f)))
						m_KMEPanel->SetGraphToModifyFromMaterial(material->GetID());
				}

				ImGui::NewLine();
			});
	}
}
