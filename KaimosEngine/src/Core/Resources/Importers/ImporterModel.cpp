#include "kspch.h"
#include "ImporterModel.h"

#include "Core/Resources/ResourceManager.h"
#include "Core/Resources/ResourceModel.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Buffer.h"

#include <glm/gtc/type_ptr.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Kaimos::Importers
{
	static const uint s_ImportingFlags = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_FlipUVs // FlipUVs gives problem with UVs, I think because STB already flips them
		| aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices | aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes | aiProcess_SortByPType;

	// ----------------------- Protected Importer Methods -------------------------------------------------
	Ref<Resources::ResourceModel> ImporterModel::LoadModel(const std::string& filepath)
	{
		// -- Check & Set Path --
		std::filesystem::path fpath;
		if (!CheckPath(filepath, fpath)) // fpath passed by ref, will be returned absolute if not
			return nullptr;

		// -- Load Scene --
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, s_ImportingFlags);

		if (!CheckScene(scene, importer))
			return nullptr;

		// -- Load Materials --
		std::vector<uint> materials;
		for (uint i = 0; i < scene->mNumMaterials; ++i)
		{
			uint mat_id = ProcessAssimpMaterial(scene->mMaterials[i], fpath.parent_path().string());
			if (mat_id != 0)
				materials.push_back(mat_id);
		}

		// -- Load Meshes --
		aiMesh* first_mesh = scene->mMeshes[0];
		Ref<Kaimos::Mesh> root_mesh = ProcessAssimpMesh(scene, first_mesh);

		if (root_mesh)
		{
			// -- Create Root Mesh --
			Ref<Resources::ResourceModel> model = CreateRef<Resources::ResourceModel>(new Resources::ResourceModel(fpath.string(), 0, root_mesh));
			root_mesh->SetParentModel(model.get());

			// -- Pick Material for Root Mesh --
			uint mat_id = Renderer::GetDefaultMaterialID();
			if (materials.size() > 0)
			{
				if (first_mesh->mMaterialIndex == 0)
				{
					if (materials[0] != mat_id)
						mat_id = materials[0];
				}
				else
					mat_id = materials[first_mesh->mMaterialIndex - 1];
			}
			
			// -- Set Root Mesh Material & Process Nodes From Root Mesh --
			root_mesh->SetMaterial(mat_id);			
			ProcessAssimpNode(scene, scene->mRootNode, materials, root_mesh.get());
			
			// -- Return Model --
			return model;
		}
		else
			KS_ERROR("Failed to load Root Mesh with AssimpLoader");
		
		return nullptr;
	}


	Ref<Resources::ResourceModel> ImporterModel::LoadDeserializedModel(const std::string& filepath, uint model_id, Ref<Mesh> root_mesh)
	{
		// -- Check & Set Path --
		std::filesystem::path fpath;
		if (!CheckPath(filepath, fpath)) // fpath passed by ref, will be returned absolute if not
			return nullptr;

		// -- Load Scene --
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, s_ImportingFlags);

		if (!CheckScene(scene, importer))
			return nullptr;

		// -- Create Model --
		Ref<Resources::ResourceModel> model = CreateRef<Resources::ResourceModel>(new Resources::ResourceModel(fpath.string(), model_id, root_mesh));

		// -- Process Root Mesh & Nodes --
		root_mesh->SetParentModel(model.get());
		ProcessDeserializedMesh(root_mesh, scene->mMeshes[0]);
		ProcessDeserializedNode(scene, scene->mRootNode, root_mesh->GetSubmeshes());

		// -- Return Model --
		return model;
	}


	
	// ----------------------- Private Importer Methods ---------------------------------------------------
	void ImporterModel::ProcessAssimpNode(const aiScene* ai_scene, const aiNode* ai_node, const std::vector<uint>& loaded_materials, Kaimos::Mesh* mesh)
	{
		// -- Process Node Meshes --
		std::vector<Ref<Kaimos::Mesh>> node_meshes;
		for (uint i = 0; i < ai_node->mNumMeshes; ++i)
		{
			if (ai_node->mMeshes[i] != 0)
			{
				// Process Mesh
				aiMesh* ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
				node_meshes.push_back(ProcessAssimpMesh(ai_scene, ai_mesh));

				// Pick Material
				uint mat_id = Renderer::GetDefaultMaterialID();
				if (ai_mesh->mMaterialIndex == 0)
				{
					if (loaded_materials[0] != mat_id)
						mat_id = loaded_materials[0];
				}
				else
					mat_id = loaded_materials[ai_mesh->mMaterialIndex - 1];
				
				// Set Mesh Material
				node_meshes[i]->SetMaterial(mat_id);
			}
		}

		// -- Fill Mesh Submeshes --
		for (uint i = 0; i < node_meshes.size(); ++i)
			if (node_meshes[i] != nullptr)
				mesh->AddSubmesh(node_meshes[i]);

		// -- Process Node Children Meshes --
		for (uint i = 0; i < ai_node->mNumChildren; i++)
			ProcessAssimpNode(ai_scene, ai_node->mChildren[i], loaded_materials, mesh);
	}


	Ref<Kaimos::Mesh> ImporterModel::ProcessAssimpMesh(const aiScene* ai_scene, const aiMesh* ai_mesh)
	{
		if (ai_mesh->mNumVertices == 0 || ai_mesh->mNumFaces == 0)
			return nullptr;

		// -- Process Vertices & Indices --
		uint max_index = 0;
		const std::vector<Vertex> mesh_vertices = ProcessMeshVertices(ai_mesh);
		const std::vector<uint> indices = ProcessMeshIndices(ai_mesh, max_index);

		// -- Create Mesh --
		std::string mesh_name = ai_mesh->mName.length > 0 ? ai_mesh->mName.C_Str() : "unnamed";
		Ref<Mesh> mesh = CreateRef<Mesh>(mesh_name);
		Kaimos::Resources::ResourceManager::AddMesh(mesh);

		// -- Set Mesh Data --
		mesh->SetMeshVertices(mesh_vertices);
		mesh->SetMeshIndices(indices);
		mesh->SetMaxIndex(max_index + 1);

		// -- Return Created Mesh --
		return mesh;
	}


	uint ImporterModel::ProcessAssimpMaterial(const aiMaterial* ai_material, const std::string& directory)
	{
		// -- Ignore Assimp Default Material --
		aiString name = aiString("unnamed");
		ai_material->Get(AI_MATKEY_NAME, name);
		if (name.C_Str() == std::string(AI_DEFAULT_MATERIAL_NAME))
			return 0;

		// -- Load Material Variables --
		aiColor3D diffuse = aiColor3D(1.0f);
		ai_real opacity = 1.0f, bumpiness = 1.0f;											// Mtl Base Values
		ai_real shininess = 1.0f, specularity = 1.0f, metallic = 0.5f, ambient_occ = 0.0f;	// Mtl PBR/NonPBR Values

		ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);				// Kd
		ai_material->Get(AI_MATKEY_OPACITY, opacity);					// d
		ai_material->Get(AI_MATKEY_BUMPSCALING, bumpiness);				// ?

		ai_material->Get(AI_MATKEY_SHININESS, shininess);				// Ns
		ai_material->Get(AI_MATKEY_SHININESS_STRENGTH, specularity);	// ?

		// This is just a guess, not sure if these should be the values
		ai_material->Get(AI_MATKEY_REFLECTIVITY, metallic);				// ? - By now, metallic will be reflectivity
		ai_material->Get(AI_MATKEY_SHININESS_STRENGTH, ambient_occ);	// ?

		// Also:
		// AI_MATKEY_COLOR_SPECULAR (Ks), AI_MATKEY_COLOR_EMISSIVE (Ke)
		// AI_MATKEY_TWOSIDED (?), AI_MATKEY_COLOR_AMBIENT (Ka), AI_MATKEY_COLOR_TRANSPARENT (?)
		// AI_MATKEY_REFRACTI (Ni -> Index of Refraction)

		// -- Create Material & Set Variables --
		const Ref<Material>& mat = Renderer::CreateMaterial(name.C_Str());

		mat->Color = glm::vec4(diffuse.r, diffuse.g, diffuse.b, opacity);
		mat->Bumpiness = bumpiness;
		mat->Smoothness = shininess / 256.0f;
		mat->Metallic = metallic;
		mat->AmbientOcclusion = ambient_occ;

		// Phong Shininess -> Beckmann BRDF Roughness conversion
		// https://simonstechblog.blogspot.com/2011/12/microfacet-brdf.html
		// https://computergraphics.stackexchange.com/questions/1515/what-is-the-accepted-method-of-converting-shininess-to-roughness-and-vice-versa
		mat->Roughness = sqrtf(2.0f / (2.0f + shininess));
		mat->SyncGraphValuesWithMaterial();

		if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			mat->SetTexture(MATERIAL_TEXTURES::ALBEDO, GetMaterialTextureFilename(ai_material, aiTextureType_DIFFUSE, directory));

		if (ai_material->GetTextureCount(aiTextureType_NORMALS) > 0)
			mat->SetTexture(MATERIAL_TEXTURES::NORMAL, GetMaterialTextureFilename(ai_material, aiTextureType_NORMALS, directory));

		if (ai_material->GetTextureCount(aiTextureType_SPECULAR) > 0)
			mat->SetTexture(MATERIAL_TEXTURES::SPECULAR, GetMaterialTextureFilename(ai_material, aiTextureType_SPECULAR, directory));
		
		if (ai_material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
			mat->SetTexture(MATERIAL_TEXTURES::ROUGHNESS, GetMaterialTextureFilename(ai_material, aiTextureType_DIFFUSE_ROUGHNESS, directory));
		
		if (ai_material->GetTextureCount(aiTextureType_METALNESS) > 0)
			mat->SetTexture(MATERIAL_TEXTURES::METALLIC, GetMaterialTextureFilename(ai_material, aiTextureType_METALNESS, directory));

		if (ai_material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
			mat->SetTexture(MATERIAL_TEXTURES::AMBIENT_OC, GetMaterialTextureFilename(ai_material, aiTextureType_AMBIENT_OCCLUSION, directory));

		// Also:
		// aiTextureType_EMISSIVE, aiTextureType_HEIGHT, aiTextureType_DISPLACEMENT

		// -- Return Material --
		return mat->GetID();
	}



	// ----------------------- Private Deserialization Methods --------------------------------------------
	void ImporterModel::ProcessDeserializedNode(const aiScene* ai_scene, const aiNode* ai_node, const std::vector<Ref<Mesh>>& submeshes)
	{
		// -- Process Node Meshes --
		std::vector<Ref<Mesh>> processed_submeshes;
		for (uint i = 0; i < ai_node->mNumMeshes; ++i)
		{
			if (ai_node->mMeshes[i] != 0 && i < submeshes.size())
			{
				// Process Mesh
				aiMesh* ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
				processed_submeshes.push_back(ProcessDeserializedMesh(submeshes[i], ai_mesh));
			}
		}

		// -- Process Node Children Meshes --
		for (uint i = 0; i < ai_node->mNumChildren; i++)
			ProcessDeserializedNode(ai_scene, ai_node->mChildren[i], processed_submeshes);
	}

	Ref<Kaimos::Mesh> ImporterModel::ProcessDeserializedMesh(Ref<Mesh> mesh, const aiMesh* ai_mesh)
	{
		if (ai_mesh->mNumVertices == 0 || ai_mesh->mNumFaces == 0)
			return nullptr;

		// -- Process Vertices & Indices --
		uint max_index = 0;
		const std::vector<Vertex> mesh_vertices = ProcessMeshVertices(ai_mesh);
		const std::vector<uint> indices = ProcessMeshIndices(ai_mesh, max_index);

		// -- Set Mesh Data --
		mesh->SetMeshVertices(mesh_vertices);
		mesh->SetMeshIndices(indices);
		mesh->SetMaxIndex(max_index + 1);

		// -- Return Mesh --
		return mesh;
	}
	


	// ----------------------- Private Helper Methods -----------------------------------------------------
	bool ImporterModel::CheckPath(const std::string& filepath, std::filesystem::path& ret_path)
	{
		std::filesystem::path fpath = filepath;
		if (!fpath.is_absolute())
			fpath = std::filesystem::current_path().string() + "/" + filepath;

		if (!std::filesystem::exists(fpath))
		{
			KS_WARN("Tried to load an unexisting file!");
			return false;
		}

		ret_path = fpath;
		return true;
	}

	bool ImporterModel::CheckScene(const aiScene* scene, const Assimp::Importer& importer)
	{
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode || scene->mNumMeshes == 0)
		{
			KS_WARN("Error importing model with AssimpLoader: {0}", importer.GetErrorString());
			return false;
		}

		return true;
	}


	const std::string ImporterModel::GetMaterialTextureFilename(const aiMaterial* ai_material, aiTextureType texture_type, const std::string& directory)
	{
		aiString texture_filename;
		ai_material->GetTexture(texture_type, 0, &texture_filename);
		return std::string(directory + "/" + texture_filename.C_Str());
	}


	const std::vector<Kaimos::Vertex> ImporterModel::ProcessMeshVertices(const aiMesh* ai_mesh)
	{
		std::vector<Vertex> ret;
		for (uint i = 0; i < ai_mesh->mNumVertices; ++i)
		{
			// Positions, Normals & Tangents
			glm::vec3 positions = glm::vec3(0.0f), normals = glm::vec3(0.0f), tangent = glm::vec3(0.0f); // TODO: Tangents Flipped?
			glm::vec2 texture_coords = glm::vec2(0.0f);

			if (ai_mesh->HasPositions())
				positions = { ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z };

			if (ai_mesh->HasNormals())
				normals = { ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z };

			if (ai_mesh->mTextureCoords[0])
				texture_coords = { ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y };
			
			if (ai_mesh->HasTangentsAndBitangents())
				tangent = { ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z };

			// Set Vertices Data
			Kaimos::Vertex vertex;
			vertex.Pos = positions;
			vertex.Normal = normals;
			vertex.TexCoord = texture_coords;
			vertex.Tangent = tangent;

			ret.push_back(vertex);
		}

		return ret;
	}


	const std::vector<uint> ImporterModel::ProcessMeshIndices(const aiMesh* ai_mesh, uint& max_index)
	{
		std::vector<uint> ret;
		for (uint i = 0; i < ai_mesh->mNumFaces; ++i)
		{
			aiFace face = ai_mesh->mFaces[i];
			for (uint j = 0; j < face.mNumIndices; ++j)
			{
				uint index = face.mIndices[j];
				ret.push_back(index);

				if (index > max_index)
					max_index = index;
			}
		}

		return ret;
	}
}
