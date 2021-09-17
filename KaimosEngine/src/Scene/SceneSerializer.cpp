#include "kspch.h"
#include "SceneSerializer.h"

#include "Core/Resources/ResourceManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/Cameras/CameraController.h"

#include "ECS/Entity.h"
#include "ECS/Components.h"

#include <yaml-cpp/yaml.h>
#include "KaimosYAMLExtension.h"


namespace Kaimos {

	// ----------------------- Global Static Serialization Method ----------------------------------------
	static void SerializeEntity(YAML::Emitter& output, Entity entity)
	{
		KS_PROFILE_FUNCTION();

		// -- Begin Entity Map --
		output << YAML::BeginMap;
		output << YAML::Key << "Entity" << YAML::Value << entity.GetID();

		if (entity.HasComponent<TagComponent>())
		{
			output << YAML::Key << "TagComponent";
			output << YAML::BeginMap;

			TagComponent& tag = entity.GetComponent<TagComponent>();
			output << YAML::Key << "Tag" << YAML::Value << tag.Tag;
			output << YAML::Key << "DuplicationCount" << YAML::Value << tag.DuplicationCount;
			output << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			output << YAML::Key << "TransformComponent";
			output << YAML::BeginMap;

			TransformComponent& transform = entity.GetComponent<TransformComponent>();

			output << YAML::Key << "EntityActive" << YAML::Value << transform.EntityActive;
			output << YAML::Key << "Translation" << YAML::Value << transform.Translation;
			output << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
			output << YAML::Key << "Scale" << YAML::Value << transform.Scale;

			output << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			output << YAML::Key << "CameraComponent";
			output << YAML::BeginMap;
			
			CameraComponent& cam_comp = entity.GetComponent<CameraComponent>();
			Camera& camera = cam_comp.Camera;

			// -- Begin Cam Map --
			output << YAML::Key << "Camera" << YAML::Value;
			output << YAML::BeginMap;
			output << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();

			output << YAML::Key << "ViewSize" << YAML::Value << camera.GetViewportSize();
			output << YAML::Key << "FOV" << YAML::Value << (int)camera.GetFOV();
			output << YAML::Key << "FarClip" << YAML::Value << camera.GetFarPlane();
			output << YAML::Key << "NearClip" << YAML::Value <<camera.GetNearPlane();
			output << YAML::Key << "OrthoSize" << YAML::Value << camera.GetSize();
			output << YAML::EndMap;
			// -- End Cam Map --

			output << YAML::Key << "PrimaryCamera" << YAML::Value << cam_comp.Primary;
			output << YAML::Key << "FixedAR" << YAML::Value << cam_comp.FixedAspectRatio;
			output << YAML::EndMap;
		}

		if (entity.HasComponent<DirectionalLightComponent>())
		{
			DirectionalLightComponent& light_comp = entity.GetComponent<DirectionalLightComponent>();

			output << YAML::Key << "DirectionalLightComponent";
			output << YAML::BeginMap;
			output << YAML::Key << "Visible" << YAML::Value << light_comp.Visible;
			output << YAML::Key << "StoredLightMinRadius" << YAML::Value << light_comp.StoredLightMinRadius;
			output << YAML::Key << "StoredLightMaxRadius" << YAML::Value << light_comp.StoredLightMaxRadius;
			output << YAML::Key << "StoredLightFalloff" << YAML::Value << light_comp.StoredLightFalloff;

			// -- Begin Light Map --
			output << YAML::Key << "Light" << YAML::Value;
			output << YAML::BeginMap;
			output << YAML::Key << "Radiance" << YAML::Value << light_comp.Light->Radiance;
			output << YAML::Key << "Intensity" << YAML::Value << light_comp.Light->Intensity;
			output << YAML::Key << "SpecularStrength" << YAML::Value << light_comp.Light->SpecularStrength;
			output << YAML::EndMap;
			// -- End Light Map --

			output << YAML::EndMap;
		}

		if (entity.HasComponent<PointLightComponent>())
		{
			PointLightComponent& light_comp = entity.GetComponent<PointLightComponent>();

			output << YAML::Key << "PointLightComponent";
			output << YAML::BeginMap;
			output << YAML::Key << "Visible" << YAML::Value << light_comp.Visible;

			// -- Begin Light Map --
			output << YAML::Key << "Light" << YAML::Value;
			output << YAML::BeginMap;
			output << YAML::Key << "Radiance" << YAML::Value << light_comp.Light->Radiance;
			output << YAML::Key << "Intensity" << YAML::Value << light_comp.Light->Intensity;
			output << YAML::Key << "SpecularStrength" << YAML::Value << light_comp.Light->SpecularStrength;

			output << YAML::Key << "FalloffMultiplier" << YAML::Value << light_comp.Light->FalloffMultiplier;
			output << YAML::Key << "MinRadius" << YAML::Value << light_comp.Light->GetMinRadius();
			output << YAML::Key << "MaxRadius" << YAML::Value << light_comp.Light->GetMaxRadius();
			output << YAML::EndMap;
			// -- End Light Map --

			output << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			SpriteRendererComponent& sprite_comp = entity.GetComponent<SpriteRendererComponent>();
			output << YAML::Key << "SpriteRendererComponent";
			output << YAML::BeginMap;
			output << YAML::Key << "Material" << YAML::Value << sprite_comp.SpriteMaterialID;
			output << YAML::EndMap;
		}

		if (entity.HasComponent<MeshRendererComponent>())
		{
			MeshRendererComponent& mesh_comp = entity.GetComponent<MeshRendererComponent>();
			output << YAML::Key << "MeshRendererComponent";
			output << YAML::BeginMap;
			output << YAML::Key << "Material" << YAML::Value << mesh_comp.MaterialID;
			output << YAML::Key << "Mesh" << YAML::Value << mesh_comp.MeshID;
			output << YAML::EndMap;
		}
		
		
		// -- End Entity Map --
		output << YAML::EndMap;
	}



	// ----------------------- Public Serialization Methods ----------------------------------------------
	void SceneSerializer::Serialize(const std::string& filepath) const
	{
		KS_PROFILE_FUNCTION();
		KS_INFO("\n\n--- SERIALIZING KAIMOS SCENE ---");
		
		const CameraController& camera_control = m_Scene->GetEditorCamera();
		const Camera& camera = m_Scene->GetEditorCamera().GetCamera();

		YAML::Emitter output;
		output << YAML::BeginMap;
		output << YAML::Key << "KaimosScene" << YAML::Value << m_Scene->GetName().c_str();							// Save Scene as Key + SceneName as value
		output << YAML::Key << "SceneColor" << YAML::Value << Renderer::GetSceneColor();							// Save Scene Color
		output << YAML::Key << "PBRPipeline" << YAML::Value << Renderer::IsSceneInPBRPipeline();					// Save if scene is PBR or not
		output << YAML::Key << "EnvironmentMapTexture" << YAML::Value << Renderer::GetEnvironmentMapFilepath();		// Save Scene Camera Orientation
		output << YAML::Key << "EnviroMapRes" << YAML::Value << Renderer::GetEnvironmentMapResolution();

		// Save editor camera as a sequence (like an array)
		output << YAML::Key << "EditorCamera" << YAML::Value << YAML::BeginSeq;
		output << YAML::BeginMap;

		output << YAML::Key << "CameraPos" << YAML::Value << camera_control.GetPosition();
		output << YAML::Key << "CameraRot" << YAML::Value << camera_control.GetOrientationAngles();

		output << YAML::Key << "CameraMovSpeed" << YAML::Value << camera_control.m_MoveSpeed;
		output << YAML::Key << "CameraSpeedMulti" << YAML::Value << camera_control.GetSpeedMultiplier();
		output << YAML::Key << "CameraRotSpeed" << YAML::Value << camera_control.m_RotationSpeed;
		output << YAML::Key << "CameraRotLock" << YAML::Value << camera_control.IsRotationLocked();
		output << YAML::Key << "CameraPanSpeed" << YAML::Value << camera_control.m_PanSpeed;
		output << YAML::Key << "CameraZoom" << YAML::Value << camera_control.m_ZoomLevel;
		output << YAML::Key << "CameraMaxZoom" << YAML::Value << camera_control.m_MaxZoomSpeed;
		output << YAML::Key << "CameraFOV" << YAML::Value << camera.GetFOV();
		output << YAML::Key << "CameraNPlane" << YAML::Value << camera.GetNearPlane();
		output << YAML::Key << "CameraFPlane" << YAML::Value << camera.GetFarPlane();
		output << YAML::EndMap;
		output << YAML::EndSeq;

		// Save Entities as a sequence (like an array)
		output << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		uint entities_deserialized = 0;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;
				
				SerializeEntity(output, entity);
				++entities_deserialized;
			});

		output << YAML::EndSeq;
		output << YAML::EndMap;		// Map is like the whole file

		std::ofstream file(filepath);
		file << output.c_str();
		m_Scene->SetPath(filepath);
		KS_TRACE("Finished Serializing {0} Entities in '{1}' Scene", entities_deserialized, m_Scene->GetName());
	}



	// ----------------------- Public Deserialization Methods --------------------------------------------
	bool SceneSerializer::Deserialize(const std::string& filepath) const
	{
		KS_PROFILE_FUNCTION();
		KS_INFO("\n\n--- DESERIALIZING KAIMOS SCENE ---");

		// -- Fail Safe --
		std::filesystem::path spath = filepath;
		if (!std::filesystem::exists(spath) && filepath.find("assets") != std::string::npos)
		{
			KS_ERROR("Error Loading '{0}' scene file\nError: Invalid Scene File", filepath);
			return false;
		}

		// -- File Load --
		YAML::Node data;
		try { data = YAML::LoadFile(filepath); }
		catch (const YAML::ParserException& exception)
		{
			KS_ERROR("Error Loading '{0}' scene file\nError: {1}", filepath, exception.what());
			return false;
		}

		if (!data["KaimosScene"])
		{
			KS_ERROR("Error Loading '{0}' scene file\nError: Wrong File, it has no 'KaimosScene' node", filepath);
			return false;
		}

		// -- Scene Setup --
		std::string scene_name = data["KaimosScene"].as<std::string>();
		KS_TRACE("Deserializing scene '{0}'", scene_name);
		m_Scene->SetName(scene_name);
		m_Scene->SetPath(filepath);

		if (data["SceneColor"])
			Renderer::SetSceneColor(data["SceneColor"].as<glm::vec3>());

		if (data["PBRPipeline"])
			Renderer::SetPBRPipeline(data["PBRPipeline"].as<bool>());
		else
			Renderer::SetPBRPipeline(false);

		if (data["EnvironmentMapTexture"])
		{
			uint map_res = data["EnviroMapRes"] ? data["EnviroMapRes"].as<uint>() : 1024;
			std::string filepath = data["EnvironmentMapTexture"].as<std::string>();

			if (!filepath.empty())
				Renderer::SetEnvironmentMapFilepath(filepath, map_res);
			else
				KS_ENGINE_TRACE("Scene has no Environment Map to load");
		}
		else
			Renderer::RemoveEnvironmentMap();

		// -- Deserialize Editor Camera --
		YAML::Node camera_node = data["EditorCamera"];
		if (camera_node)
		{
			auto cam_values = camera_node[0];

			m_Scene->GetEditorCamera().SetPosition(cam_values["CameraPos"].as<glm::vec3>());
			m_Scene->GetEditorCamera().SetOrientation(cam_values["CameraRot"].as<glm::vec2>());

			m_Scene->GetEditorCamera().SetMoveSpeed(cam_values["CameraMovSpeed"].as<float>());
			m_Scene->GetEditorCamera().SetSpeedMultiplier(cam_values["CameraSpeedMulti"].as<float>());
			m_Scene->GetEditorCamera().SetRotationSpeed(cam_values["CameraRotSpeed"].as<float>());

			m_Scene->GetEditorCamera().LockRotation(cam_values["CameraRotLock"].as<bool>());
			m_Scene->GetEditorCamera().m_PanSpeed = cam_values["CameraPanSpeed"].as<float>();

			m_Scene->GetEditorCamera().SetZoomLevel(cam_values["CameraZoom"].as<float>());
			m_Scene->GetEditorCamera().SetMaxZoomSpeed(cam_values["CameraMaxZoom"].as<float>()); //TODO: change name

			m_Scene->GetEditorCamera().m_Camera.SetFOV(cam_values["CameraFOV"].as<float>());
			m_Scene->GetEditorCamera().m_Camera.SetNearPlane(cam_values["CameraNPlane"].as<float>());
			m_Scene->GetEditorCamera().m_Camera.SetFarPlane(cam_values["CameraFPlane"].as<float>());
		}
		

		// -- Entities Load --
		uint entities_deserialized = 0;
		YAML::Node entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				++entities_deserialized;
				uint entity_id = entity["Entity"].as<uint>();
				
				std::string name;
				uint duplication_count = 1;
				auto tag_component = entity["TagComponent"];
				if (tag_component)
				{
					name = tag_component["Tag"].as<std::string>();
					duplication_count = tag_component["DuplicationCount"].as<uint>();
				}

				KS_TRACE("Deserialized Entity '{0}' (ID: {1})", name, entity_id);
				Entity deserialized_entity = m_Scene->CreateEntity(name, entity_id);
				deserialized_entity.GetComponent<TagComponent>().DuplicationCount = duplication_count;

				YAML::Node transform_node = entity["TransformComponent"];
				if (transform_node)
				{
					TransformComponent& transform_comp = deserialized_entity.GetComponent<TransformComponent>();

					transform_comp.EntityActive = transform_node["EntityActive"].as<bool>();
					transform_comp.Translation = transform_node["Translation"].as<glm::vec3>();
					transform_comp.Rotation = transform_node["Rotation"].as<glm::vec3>();
					transform_comp.Scale = transform_node["Scale"].as<glm::vec3>();
				}

				YAML::Node cameracomp_node = entity["CameraComponent"];
				if (cameracomp_node)
				{
					CameraComponent& cam_comp = deserialized_entity.AddComponent<CameraComponent>();
					YAML::Node& camera_node = cameracomp_node["Camera"];

					if (camera_node["ProjectionType"].as<int>() == static_cast<int>(Kaimos::CAMERA_PROJECTION::ORTHOGRAPHIC))
						cam_comp.Camera.SetOrthographicParameters();
					else
						cam_comp.Camera.SetPerspectiveParameters();

					glm::vec2 cam_view_size = camera_node["ViewSize"].as<glm::vec2>();
					cam_comp.Camera.SetViewport(cam_view_size.x, cam_view_size.y);

					cam_comp.Camera.SetFOV(camera_node["FOV"].as<float>());
					cam_comp.Camera.SetNearPlane(camera_node["NearClip"].as<float>());
					cam_comp.Camera.SetFarPlane(camera_node["FarClip"].as<float>());
					cam_comp.Camera.SetSize(camera_node["OrthoSize"].as<float>());

					cam_comp.Primary = cameracomp_node["PrimaryCamera"].as<bool>();
					cam_comp.FixedAspectRatio = cameracomp_node["FixedAR"].as<bool>();

					if(cam_comp.Primary)
						m_Scene->SetPrimaryCamera(deserialized_entity);
				}

				YAML::Node dirlightcomp_node = entity["DirectionalLightComponent"];
				if (dirlightcomp_node)
				{
					DirectionalLightComponent& light_comp = deserialized_entity.AddComponent<DirectionalLightComponent>();
					YAML::Node& light_node = dirlightcomp_node["Light"];

					glm::vec4 radiance = light_node["Radiance"].as<glm::vec4>();
					float intensity = light_node["Intensity"].as<float>();
					float specular_strength = light_node["SpecularStrength"].as<float>();

					float min_rad = dirlightcomp_node["StoredLightMinRadius"].as<float>();
					float max_rad = dirlightcomp_node["StoredLightMaxRadius"].as<float>();
					float falloff = dirlightcomp_node["StoredLightFalloff"].as<float>();

					light_comp.Visible = dirlightcomp_node["Visible"].as<bool>();
					light_comp.SetComponentValues(falloff, min_rad, max_rad);
					light_comp.SetLightValues(radiance, intensity, specular_strength);
				}

				YAML::Node pointlightcomp_node = entity["PointLightComponent"];
				if (pointlightcomp_node)
				{
					PointLightComponent& light_comp = deserialized_entity.AddComponent<PointLightComponent>();
					YAML::Node& light_node = pointlightcomp_node["Light"];

					glm::vec4 radiance = light_node["Radiance"].as<glm::vec4>();
					float intensity = light_node["Intensity"].as<float>();
					float specular_strength = light_node["SpecularStrength"].as<float>();
					float falloff = light_node["FalloffMultiplier"].as<float>();
					float min_rad = light_node["MinRadius"].as<float>();
					float max_rad = light_node["MaxRadius"].as<float>();

					light_comp.Visible = pointlightcomp_node["Visible"].as<bool>();
					light_comp.SetLightValues(radiance, intensity, specular_strength);
					light_comp.SetPointLightValues(falloff, min_rad, max_rad);
				}

				YAML::Node sprite_node = entity["SpriteRendererComponent"];
				if (sprite_node)
				{
					uint mat_node = sprite_node["Material"].as<uint>();
					uint mat_id = Renderer::GetMaterialIfExists(mat_node);					
					if (mat_id == 0)
					{
						uint def_mat_id = Renderer::GetDefaultMaterialID();
						if (def_mat_id != 0)
							mat_id = def_mat_id;
						else
							mat_id = Renderer::CreateMaterial("Unnamed")->GetID();
					}
					
					SpriteRendererComponent& sprite_comp = deserialized_entity.AddComponent<SpriteRendererComponent>();
					sprite_comp.SetMaterial(mat_id);
				}

				YAML::Node mesh_node = entity["MeshRendererComponent"];
				if (mesh_node)
				{
					uint mat_node = mesh_node["Material"].as<uint>();
					uint mesh_id_node = mesh_node["Mesh"].as<uint>();
					if (!Resources::ResourceManager::MeshExists(mesh_id_node))
						mesh_id_node = 0;

					uint mat_id = Renderer::GetMaterialIfExists(mat_node);
					if (mat_id == 0)
					{
						uint def_mat_id = Renderer::GetDefaultMaterialID();
						if (def_mat_id != 0)
							mat_id = def_mat_id;
						else
							mat_id = Renderer::CreateMaterial("Unnamed")->GetID();
					}

					MeshRendererComponent& mesh_comp = deserialized_entity.AddComponent<MeshRendererComponent>();
					mesh_comp.SetMesh(mesh_id_node);
					mesh_comp.SetMaterial(mat_id);
				}
			}
		}

		KS_TRACE("Finished Deserializing {0} Entities in '{1}' Scene", entities_deserialized, m_Scene->GetName());
		return true;
	}
}
