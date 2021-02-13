#include "kspch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <yaml-cpp/yaml.h>


namespace YAML {

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


namespace Kaimos {

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



	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& output, Entity entity)
	{
		// Begin Entity Map
		output << YAML::BeginMap;
		output << YAML::Key << "Entity" << YAML::Value << "123"; // TODO: Entity ID goes here

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
			SceneCamera& camera = cam_comp.Camera;
			output << YAML::Key << "Camera" << YAML::Value;

			// Begin Cam Map
			output << YAML::BeginMap;
			output << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			
			output << YAML::Key << "FOV" << YAML::Value << (int)camera.GetPerspectiveFOV();
			output << YAML::Key << "PerspFarClip" << YAML::Value << camera.GetPerspectiveFarClip();
			output << YAML::Key << "PerspNearClip" << YAML::Value <<camera.GetPerspectiveNearClip();

			output << YAML::Key << "OrthoSize" << YAML::Value << camera.GetOrthographicSize();
			output << YAML::Key << "OrthoFarClip" << YAML::Value << camera.GetOrthographicFarClip();
			output << YAML::Key << "OrthoNearClip" << YAML::Value << camera.GetOrthographicNearClip();
			output << YAML::EndMap;
			// End Cam Map

			output << YAML::Key << "PrimaryCamera" << YAML::Value << cam_comp.Primary;
			output << YAML::Key << "FixedAR" << YAML::Value << cam_comp.FixedAspectRatio;
			output << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			output << YAML::Key << "SpriteRendererComponent";
			output << YAML::BeginMap;
			output << YAML::Key << "Color" << YAML::Value << entity.GetComponent<SpriteRendererComponent>().Color;
			output << YAML::EndMap;
		}
		
		
		// End Entity Map
		output << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter output;

		output << YAML::BeginMap;
		output << YAML::Key << "KaimosScene" << YAML::Value << "UnnamedScene";	// Save Scene as Key + SceneName as value
		output << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;		// Save Entites as a sequence (like an array)

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
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		// Not implemented
		KS_ENGINE_ASSERT(false, "");
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data = YAML::LoadFile(filepath);
		if (!data["KaimosScene"])
			return false;

		std::string scene_name = data["KaimosScene"].as<std::string>();
		KS_ENGINE_TRACE("Deserializing scene '{0}'", scene_name);

		YAML::Node entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint uuid = entity["Entity"].as<uint>(); // TODO: Make UUIDs
				
				std::string name;
				auto tag_component = entity["TagComponent"];
				if (tag_component)
					name = tag_component["Tag"].as<std::string>();

				KS_ENGINE_TRACE("Deserialized Entity with ID = {0} and name = '{1}'", uuid, name.c_str());
				Entity deserialized_entity = m_Scene->CreateEntity(name); // Create also with uuid

				YAML::Node transform_node = entity["TransformComponent"];
				if (transform_node)
				{
					TransformComponent& transform_comp = deserialized_entity.GetComponent<TransformComponent>();
					transform_comp.Translation = transform_node["Translation"].as<glm::vec3>();
					transform_comp.Rotation = transform_node["Rotation"].as<glm::vec3>();
					transform_comp.Scale = transform_node["Scale"].as<glm::vec3>();
				}

				YAML::Node cameracomp_node = entity["CameraComponent"];
				if (cameracomp_node)
				{
					CameraComponent& cam_comp = deserialized_entity.AddComponent<CameraComponent>();
					YAML::Node& camera_node = cameracomp_node["Camera"];

					cam_comp.Camera.SetProjectionType((SceneCamera::ProjectionType) camera_node["ProjectionType"].as<int>());

					cam_comp.Camera.SetPerspectiveFOV(camera_node["FOV"].as<float>());
					cam_comp.Camera.SetPerspectiveClips(camera_node["PerspNearClip"].as<float>(), camera_node["PerspFarClip"].as<float>());

					cam_comp.Camera.SetOrthographicSize(camera_node["OrthoSize"].as<float>());
					cam_comp.Camera.SetOrthographicClips(camera_node["OrthoNearClip"].as<float>(), camera_node["OrthoFarClip"].as<float>());

					cam_comp.Primary = cameracomp_node["PrimaryCamera"].as<bool>();
					cam_comp.FixedAspectRatio = cameracomp_node["FixedAR"].as<bool>();
				}

				YAML::Node sprite_node = entity["SpriteRendererComponent"];
				if (sprite_node)
					deserialized_entity.AddComponent<SpriteRendererComponent>().Color = sprite_node["Color"].as<glm::vec4>();
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		// Not implemented
		KS_ENGINE_ASSERT(false, "");
		return false;
	}
}