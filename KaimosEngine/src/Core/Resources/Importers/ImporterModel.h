#ifndef _IMPORTERMODEL_H_
#define _IMPORTERMODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//class aiScene; // TODO: Solve this
//class aiMesh;
//class aiMaterial;
//struct aiNode;
//enum aiTextureType;

namespace Kaimos {

	namespace Resources { class ResourceModel; }
	class Mesh;
	class Material;

	namespace Importers {

		class ImporterModel
		{
		public:

			// --- Public Importer Methods ---
			static Ref<Resources::ResourceModel> LoadModel(const std::string& filepath);

		private:

			// --- Private Importer Methods ---
			static void ProcessAssimpNode(const aiScene* ai_scene, aiNode* ai_node, Kaimos::Mesh* mesh, const std::vector<uint> loaded_materials);
			static Ref<Kaimos::Mesh> ProcessAssimpMesh(const aiScene* ai_scene, aiMesh* ai_mesh);
			static uint ProcessAssimpMaterial(aiMaterial* ai_material, const std::string& directory);

			static const std::string LoadMaterialTexture(aiMaterial* ai_material, aiTextureType texture_type, const std::string& directory);
		};
	}
}

#endif //_IMPORTERMODEL_H_
