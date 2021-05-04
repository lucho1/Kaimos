#include "kspch.h"
#include "SceneSerializer.h"

#include "Renderer/Renderer.h"

#include "ECS/Entity.h"
#include "ECS/Components.h"

#include <yaml-cpp/yaml.h>


// ---------------------------------------------------------------------------------------------------
// ----------------------- YAML Additions Static Methods ---------------------------------------------
// TODO: Make this in another place ???
namespace YAML {
	
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& vec)
		{
			Node node;
			node.push_back(vec.x);
			node.push_back(vec.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& vec)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			vec.x = node[0].as<float>();
			vec.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& vec)
		{
			Node node;
			node.push_back(vec.x);
			node.push_back(vec.y);
			node.push_back(vec.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& vec)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			vec.x = node[0].as<float>();
			vec.y = node[1].as<float>();
			vec.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& vec)
		{
			Node node;
			node.push_back(vec.x);
			node.push_back(vec.y);
			node.push_back(vec.z);
			node.push_back(vec.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& vec)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			vec.x = node[0].as<float>();
			vec.y = node[1].as<float>();
			vec.z = node[2].as<float>();
			vec.w = node[3].as<float>();
			return true;
		}
	};
}
// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------


namespace Kaimos {

	// ----------------------- YAML Operators Methods ----------------------------------------------------
	YAML::Emitter& operator<<(YAML::Emitter& output, const glm::vec2& vec)
	{
		output << YAML::Flow;
		output << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
		return output;
	}

	YAML::Emitter& operator<<(YAML::Emitter& output, const glm::vec3& vec)
	{
		// Flow, instead of serializing each value separatedly (x; y; z) will put them together as [x, y, z]
		output << YAML::Flow;
		output << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return output;
	}

	YAML::Emitter& operator<<(YAML::Emitter& output, const glm::vec4& vec)
	{
		output << YAML::Flow;
		output << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return output;
	}



	// ----------------------- Global Static Serialization Method ----------------------------------------
	// TODO: Move this to another place ??? In the entities/Comps rework
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

			std::string& tag = entity.GetComponent<TagComponent>().Tag;
			output << YAML::Key << "Tag" << YAML::Value << tag;
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
			output << YAML::Key << "Camera" << YAML::Value;

			// -- Begin Cam Map --
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

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			SpriteRendererComponent& sprite_comp = entity.GetComponent<SpriteRendererComponent>();

			output << YAML::Key << "SpriteRendererComponent";
			output << YAML::BeginMap;
			output << YAML::Key << "Color" << YAML::Value << sprite_comp.SpriteMaterial->Color;
			output << YAML::Key << "TextureFile" << YAML::Value << sprite_comp.SpriteMaterial->GetTexturePath();
			output << YAML::Key << "TextureTiling" << YAML::Value << sprite_comp.SpriteMaterial->TextureTiling;
			output << YAML::Key << "TextureUVOffset" << YAML::Value << sprite_comp.SpriteMaterial->TextureUVOffset;
			output << YAML::EndMap;
		}
		
		
		// -- End Entity Map --
		output << YAML::EndMap;
	}



	// ----------------------- Public Serialization Methods ----------------------------------------------
	void SceneSerializer::Serialize(const std::string& filepath) const
	{
		KS_PROFILE_FUNCTION();
		
		YAML::Emitter output;
		output << YAML::BeginMap;
		output << YAML::Key << "KaimosScene" << YAML::Value << m_Scene->GetName().c_str();	// Save Scene as Key + SceneName as value
		output << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;					// Save Entites as a sequence (like an array)

		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;
				
				SerializeEntity(output, entity);
			});

		output << YAML::EndSeq;
		output << YAML::EndMap;		// Map is like the whole file

		std::ofstream file(filepath);
		file << output.c_str();
		m_Scene->SetPath(filepath);
	}



	// ----------------------- Public Deserialization Methods --------------------------------------------
	bool SceneSerializer::Deserialize(const std::string& filepath) const
	{
		KS_PROFILE_FUNCTION();

		// -- File Load --
		YAML::Node data;
		try { data = YAML::LoadFile(filepath); }
		catch (const YAML::ParserException& exception)
		{
			KS_ENGINE_ERROR("Error Loading '{0}' scene file\nError: {1}", filepath.c_str(), exception.what());
			return false;
		}

		if (!data["KaimosScene"])
		{
			KS_ENGINE_ERROR("Error Loading '{0}' scene file\nError: Wrong File, it has no 'KaimosScene' node", filepath.c_str());
			return false;
		}

		// -- Scene Setup --
		std::string scene_name = data["KaimosScene"].as<std::string>();
		KS_ENGINE_TRACE("Deserializing scene '{0}'", scene_name);
		m_Scene->SetName(scene_name);
		m_Scene->SetPath(filepath);

		// -- Entities Load --
		YAML::Node entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint entity_id = entity["Entity"].as<uint>();
				
				std::string name;
				auto tag_component = entity["TagComponent"];
				if (tag_component)
					name = tag_component["Tag"].as<std::string>();

				KS_ENGINE_TRACE("Deserialized Entity '{0}' (ID: {1})", name.c_str(), entity_id);
				Entity deserialized_entity = m_Scene->CreateEntity(name, entity_id);

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

					if (camera_node["ProjectionType"].as<int>() == (int)Kaimos::CAMERA_PROJECTION::ORTHOGRAPHIC)
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

				YAML::Node sprite_node = entity["SpriteRendererComponent"];
				if (sprite_node)
				{
					Ref<Material>* mat = &Renderer::CreateMaterial();
					(*mat)->Color = sprite_node["Color"].as<glm::vec4>();

					auto file_node = sprite_node["TextureFile"];
					if (file_node && !file_node.as<std::string>().empty())
						(*mat)->SetTexture(file_node.as<std::string>());

					auto tile_node = sprite_node["TextureTiling"];
					if (tile_node)
						(*mat)->TextureTiling = tile_node.as<float>();

					auto offset_node = sprite_node["TextureUVOffset"];
					if (offset_node)
						(*mat)->TextureUVOffset = offset_node.as<glm::vec2>();
					
					SpriteRendererComponent& sprite_comp = deserialized_entity.AddComponent<SpriteRendererComponent>();
					sprite_comp.SetMaterial(*mat);
					(*mat)->SyncGraphValuesWithMaterial();
				}
			}
		}

		return true;
	}
}
