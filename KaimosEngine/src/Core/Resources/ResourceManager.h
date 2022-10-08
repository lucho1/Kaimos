#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

namespace YAML { class Emitter; class Node; }
namespace Kaimos { class Mesh; }

namespace Kaimos::Resources {

	class ResourceModel;
	class ResourceManager
	{
	public:

		// --- Public Class Methods ---
		static void CleanUp();

	public:

		// --- Public Generic Resources Methods ---
		static bool CheckValidPathForHDRTexture(const std::string& filepath);

	public:

		// --- Public Resources Methods ---
		static Ref<ResourceModel> CreateModel(const std::string& filepath);
		static bool ModelExists(uint model_id);

		static void AddMesh(const Ref<Mesh>& mesh);
		static bool MeshExists(uint mesh_id);		

		// --- Getters ---
		static Ref<Mesh> GetMesh(uint mesh_id);
		static Ref<Mesh> GetMeshFromIndex(uint index);
		static const std::unordered_map<uint, Ref<Mesh>>& GetMeshesMap() { return m_MeshesResources; }

		static uint GetModelResourceReferences(uint resource_id);
		static uint GetMeshResourceReferences(uint resource_id);

		// --- Public Resources Serialization Methods ---
		static void SerializeResources();
		static void DeserializeResources();

	private:

		// --- Public Resources Serialization Methods ---
		static void SerializeMesh(const Ref<Mesh>& mesh, YAML::Emitter& output_emitter);
		static const Ref<Mesh> DeserializeMesh(YAML::Node& yaml_node);
		static void DeserializeModel(const std::string& model_path, uint model_id, Ref<Mesh>& root_mesh);

	private:

		// --- Stored Resources ---
		static std::unordered_map<std::string, Ref<ResourceModel>> m_ModelResources;
		static std::unordered_map<uint, Ref<Mesh>> m_MeshesResources;
	};
}

#endif //_RESOURCEMANAGER_H_
