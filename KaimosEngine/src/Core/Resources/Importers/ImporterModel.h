#ifndef _IMPORTERMODEL_H_
#define _IMPORTERMODEL_H_

#include <assimp/Importer.hpp>

class aiScene;
class aiMesh;
class aiMaterial;
struct aiNode;
enum aiTextureType;

namespace Assimp { class Importer; }

namespace Kaimos {
	
	class Vertex;
	class Mesh;
	class Material;

	namespace Resources {
		class ResourceManager;
		class ResourceModel;
	}

	namespace Importers {

		class ImporterModel
		{
			friend class Kaimos::Resources::ResourceManager;
		protected:

			// --- Protected Importer Methods ---
			static Ref<Resources::ResourceModel> LoadModel(const std::string& filepath);
			static Ref<Resources::ResourceModel> LoadDeserializedModel(const std::string& filepath, uint model_id, Ref<Mesh> root_mesh);

		private:

			// --- Private Importer Methods ---
			static void ProcessAssimpNode(const aiScene* ai_scene, const aiNode* ai_node, const std::vector<uint>& loaded_materials, Kaimos::Mesh* mesh);
			static Ref<Kaimos::Mesh> ProcessAssimpMesh(const aiScene* ai_scene, const aiMesh* ai_mesh);
			static uint ProcessAssimpMaterial(const aiMaterial* ai_material, const std::string& directory);
			
			// --- Private Deserialization Methods ---
			static void ProcessDeserializedNode(const aiScene* ai_scene, const aiNode* ai_node, const std::vector<Ref<Mesh>>& submeshes);
			static Ref<Kaimos::Mesh> ProcessDeserializedMesh(Ref<Mesh> mesh, const aiMesh* ai_mesh);


			// --- Private Helper Methods ---
			static bool CheckPath(const std::string& filepath, std::filesystem::path& ret_path);
			static bool CheckScene(const aiScene* scene, const Assimp::Importer& importer);

			static const std::string GetMaterialTextureFilename(const aiMaterial* ai_material, aiTextureType texture_type, const std::string& directory);
			static const std::vector<Kaimos::Vertex> ProcessMeshVertices(const aiMesh* ai_mesh);
			static const std::vector<uint> ProcessMeshIndices(const aiMesh* ai_mesh, uint& max_index);
		};
	}
}

#endif //_IMPORTERMODEL_H_
