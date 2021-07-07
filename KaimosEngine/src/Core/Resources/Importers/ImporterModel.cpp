#include "kspch.h"
#include "ImporterModel.h"

#include "Core/Resources/ResourceModel.h"
#include "Renderer/Renderer.h"
#include "Renderer/Resources/Material.h"
#include "Renderer/Resources/Buffer.h"

#include <glm/gtc/type_ptr.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Kaimos::Importers
{
	// ----------------------- Protected Importer Methods -------------------------------------------------
	Ref<Resources::ResourceModel> ImporterModel::LoadModel(const std::string& filepath)
	{
		std::filesystem::path fpath = filepath;
		if (!fpath.is_absolute())
			fpath = std::filesystem::current_path().string() + "/" + filepath;

		if (!std::filesystem::exists(fpath))
		{
			KS_ENGINE_WARN("Tried to load an unexisting file!");
			return nullptr;
		}

		// -- Load Scene --
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals //| aiProcess_FlipUVs // FlipUVs gives problem with UVs, I think because STB already flips them
			| aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices | aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes | aiProcess_SortByPType);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode || scene->mNumMeshes == 0)
		{
			KS_ENGINE_WARN("Error importing model with AssimpLoader: {0}", importer.GetErrorString());
			return nullptr;
		}

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
			// -- Create Root Mesh & Process its Node --
			Ref<Resources::ResourceModel> model = CreateRef<Resources::ResourceModel>(new Resources::ResourceModel(fpath.string(), 0, root_mesh));
			root_mesh->SetParentModel(model.get());
			model->m_RootMesh->m_MaterialID = first_mesh->mMaterialIndex == 0 ? Renderer::GetDefaultMaterialID() : materials[first_mesh->mMaterialIndex - 1];
			ProcessAssimpNode(scene, scene->mRootNode, model->m_RootMesh.get(), materials);

			// -- Return Model --
			return model;
		}
		else
		{
			KS_ENGINE_ERROR("Failed to load Root Mesh with AssimpLoader");
		}
		
		return nullptr;
	}


	
	// ----------------------- Private Importer Methods ---------------------------------------------------
	void ImporterModel::ProcessAssimpNode(const aiScene* ai_scene, aiNode* ai_node, Kaimos::Mesh* mesh, const std::vector<uint> loaded_materials)
	{
		// -- Process Node Meshes --
		std::vector<Ref<Kaimos::Mesh>> node_meshes;
		for (uint i = 0; i < ai_node->mNumMeshes; ++i)
		{
			if (ai_node->mMeshes[i] != 0)
			{
				aiMesh* ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
				node_meshes.push_back(ProcessAssimpMesh(ai_scene, ai_mesh));
				if (ai_mesh->mMaterialIndex != 0)
					node_meshes[i]->m_MaterialID = loaded_materials[ai_mesh->mMaterialIndex - 1]; // -1 Because we are not loading assimp's default material
				else
					node_meshes[i]->m_MaterialID = Renderer::GetDefaultMaterialID();
			}
		}

		// -- Fill Mesh Submeshes --
		for (uint i = 0; i < node_meshes.size(); ++i)
			if (node_meshes[i] != nullptr)
				mesh->AddSubmesh(node_meshes[i]);

		// -- Process Node Children Meshes --
		for (uint i = 0; i < ai_node->mNumChildren; i++)
			ProcessAssimpNode(ai_scene, ai_node->mChildren[i], mesh, loaded_materials);
	}


	Ref<Kaimos::Mesh> ImporterModel::ProcessAssimpMesh(const aiScene* ai_scene, aiMesh* ai_mesh)
	{
		if (ai_mesh->mNumVertices == 0 || ai_mesh->mNumFaces == 0)
			return nullptr;

		// -- Process Vertices --
		std::vector<float> vertices;
		for (uint i = 0; i < ai_mesh->mNumVertices; ++i)
		{
			// Positions & Normals
			glm::vec3 positions = glm::vec3(0.0f), normals = glm::vec3(0.0f);
			glm::vec2 texture_coords = glm::vec2(0.0f);

			if (ai_mesh->HasPositions())
				positions = { ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z };

			if (ai_mesh->HasNormals())
				normals = { ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z };

			if (ai_mesh->mTextureCoords[0])
				texture_coords = { ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y };

			vertices.insert(vertices.end(), 3, *glm::value_ptr(positions));
			vertices.insert(vertices.end(), 3, *glm::value_ptr(normals));
			vertices.insert(vertices.end(), 2, *glm::value_ptr(texture_coords));

			// Tangents & Bitangents
			//glm::vec3 tangents = glm::vec3(0.0f), bitangents = glm::vec3(0.0f); // TODO: Flipped?
			//if (ai_mesh->HasTangentsAndBitangents())
			//{
			//	tangents = { ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z };
			//	bitangents = { ai_mesh->mBitangents[i].x, ai_mesh->mBitangents[i].y, ai_mesh->mBitangents[i].z };
			//}
			//
			//vertices.insert(vertices.end(), 3, *glm::value_ptr(tangents));
			//vertices.insert(vertices.end(), 3, *glm::value_ptr(bitangents));
		}

		// -- Process Indices --
		std::vector<uint> indices;
		for (uint i = 0; i < ai_mesh->mNumFaces; ++i)
		{
			aiFace face = ai_mesh->mFaces[i];
			for (uint j = 0; j < face.mNumIndices; ++j)
				indices.push_back(face.mIndices[j]);
		}

		// -- Create Buffers --
		BufferLayout layout = {
			{ SHADER_DATATYPE::FLOAT3,	"a_Position" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Normal" },
			{ SHADER_DATATYPE::FLOAT2,	"a_TexCoord" },
			{ SHADER_DATATYPE::FLOAT4,	"a_Color" },
			{ SHADER_DATATYPE::FLOAT ,	"a_TexIndex" },
			{ SHADER_DATATYPE::FLOAT ,	"a_TilingFactor" },
			{ SHADER_DATATYPE::INT ,	"a_EntityID" }
		};

		Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(float));
		Ref<IndexBuffer> ibo = IndexBuffer::Create(indices.data(), indices.size());
		Ref<VertexArray> vao = VertexArray::Create();

		vbo->SetLayout(layout);
		vao->AddVertexBuffer(vbo);
		vao->SetIndexBuffer(ibo);
		vao->Unbind(); vbo->Unbind(); ibo->Unbind();

		// -- Create & Return Mesh --
		std::string mesh_name = ai_mesh->mName.length > 0 ? ai_mesh->mName.C_Str() : "unnamed";
		Ref<Mesh> mesh = CreateRef<Mesh>(vao, mesh_name);
		Renderer::CreateMesh(mesh);
		return mesh;
	}


	uint ImporterModel::ProcessAssimpMaterial(aiMaterial* ai_material, const std::string& directory)
	{
		// -- Ignore Assimp Default Material --
		aiString name = aiString("unnamed");
		ai_material->Get(AI_MATKEY_NAME, name);
		if (name.C_Str() == std::string(AI_DEFAULT_MATERIAL_NAME))
			return 0;

		// -- Load Material Variables --
		aiColor3D diffuse = aiColor3D(1.0);
		ai_real opacity = 1.0;

		ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);	// Kd
		ai_material->Get(AI_MATKEY_OPACITY, opacity);		// d

		// Also:
		// AI_MATKEY_COLOR_SPECULAR (Ks), AI_MATKEY_COLOR_EMISSIVE (Ke), AI_MATKEY_SHININESS (Ns), AI_MATKEY_BUMPSCALING (?),
		// AI_MATKEY_TWOSIDED (?), AI_MATKEY_COLOR_AMBIENT (Ka), AI_MATKEY_COLOR_TRANSPARENT (?), AI_MATKEY_REFRACTI (Ni -> Maybe the "Metallic" value?),
		// AI_MATKEY_REFLECTIVITY (? -> Also a reflectivity color, wtf), AI_MATKEY_SHININESS_STRENGTH (?)

		// -- Create Material & Set Variables --
		const Ref<Material>& mat = Renderer::CreateMaterial(name.C_Str());
		mat->Color = glm::vec4(diffuse.r, diffuse.g, diffuse.b, opacity);

		if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			mat->SetTexture(LoadMaterialTexture(ai_material, aiTextureType_DIFFUSE, directory));

		// Also:
		// aiTextureType_EMISSIVE, aiTextureType_SPECULAR, aiTextureType_NORMALS, aiTextureType_HEIGHT, aiTextureType_DISPLACEMENT

		// -- Return Material --
		return mat->GetID();
	}
	

	const std::string ImporterModel::LoadMaterialTexture(aiMaterial* ai_material, aiTextureType texture_type, const std::string& directory)
	{
		aiString texture_filename;
		ai_material->GetTexture(texture_type, 0, &texture_filename);
		return std::string(directory + "/" + texture_filename.C_Str());
	}
}
