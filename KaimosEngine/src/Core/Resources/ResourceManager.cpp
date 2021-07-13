#include "kspch.h"
#include "ResourceManager.h"
#include "ResourceModel.h"
#include "Importers/ImporterModel.h"

#include <yaml-cpp/yaml.h>


namespace Kaimos::Resources {
		
	// ----------------------- Variables Initialization ---------------------------------------------------
	std::unordered_map<std::string, Ref<ResourceModel>> ResourceManager::m_ModelResources = {};
	std::unordered_map<uint, Ref<Mesh>> ResourceManager::m_MeshesResources = {};



	// ----------------------- Public Class Methods -------------------------------------------------------
	void ResourceManager::CleanUp()
	{
		for (auto& model : m_ModelResources)
		{
			if (model.second.use_count() > 1)
			{
				KS_ENGINE_WARN("Deleting a reference with +1 references loaded!");
			}
		
			model.second.reset();
		}

		for (auto& mesh : m_MeshesResources)
		{
			if (mesh.second.use_count() > 1)
			{
				KS_ENGINE_WARN("Deleting a reference with +1 references loaded!");
			}

			mesh.second.reset();
		}
		
		m_ModelResources.clear();
		m_MeshesResources.clear();
	}



	// ----------------------- Public Resources Methods ---------------------------------------------------
	Ref<ResourceModel> ResourceManager::CreateModel(const std::string& filepath)
	{
		if (filepath.find("assets") != std::string::npos)
		{
			auto& it = m_ModelResources.find(filepath);
			if(it != m_ModelResources.end())
				return (*it).second;
		
			Ref<ResourceModel> model = Kaimos::Importers::ImporterModel::LoadModel(filepath);
			if (model)
			{
				m_ModelResources.insert({ model->GetFilepath(), model });
				return model;
			}
		}
		else
		{
			KS_ENGINE_ERROR("Filepath is not within project folders");
		}

		return nullptr;
	}


	bool ResourceManager::ModelExists(uint model_id)
	{
		for (auto& model : m_ModelResources)
			if (model.second->GetID() == model_id)
				return true;

		return false;
	}


	void ResourceManager::AddMesh(const Ref<Mesh>& mesh)
	{
		uint mesh_id = mesh->GetID();
		if (m_MeshesResources.find(mesh_id) == m_MeshesResources.end())
			m_MeshesResources.insert({ mesh_id, mesh });
	}


	bool ResourceManager::MeshExists(uint mesh_id)
	{
		if (m_MeshesResources.find(mesh_id) != m_MeshesResources.end())
			return true;

		return false;
	}

	

	// ----------------------- Getters --------------------------------------------------------------------
	Ref<Mesh> ResourceManager::GetMesh(uint mesh_id)
	{
		if (MeshExists(mesh_id))
			return m_MeshesResources[mesh_id];

		return nullptr;
	}

	Ref<Mesh> ResourceManager::GetMeshFromIndex(uint index)
	{
		if (index < m_MeshesResources.size())
		{
			uint i = 0;
			for (auto& mesh : m_MeshesResources)
			{
				if (i == index)
					return mesh.second;

				++i;
			}
		}

		return nullptr;
	}

	uint ResourceManager::GetModelResourceReferences(uint resource_id)
	{
		for (auto& model : m_ModelResources)
			if (model.second->GetID() == resource_id)
				return model.second.use_count();

		return 0;
	}

	uint ResourceManager::GetMeshResourceReferences(uint resource_id)
	{
		auto& mesh = m_MeshesResources.find(resource_id);
		if (mesh != m_MeshesResources.end())
			return mesh->second.use_count();

		return 0;
	}

	
	
	// ----------------------- Public Resources Serialization Methods -------------------------------------
	void ResourceManager::SerializeResources()
	{
		KS_PROFILE_FUNCTION();

		// -- Begin Serialization Map --
		YAML::Emitter output;
		output << YAML::BeginMap;
		output << YAML::Key << "KaimosSaveFile" << YAML::Value << "KaimosResources";

		// -- Serialize Models (as Sequence) --
		output << YAML::Key << "Models" << YAML::Value << YAML::BeginSeq;
		for (auto& res : m_ModelResources)
		{
			output << YAML::BeginMap;
			output << YAML::Key << "Model" << YAML::Value << res.second->GetID();
			output << YAML::Key << "Path" << YAML::Value << res.first;
			output << YAML::Key << "RootMesh";
			SerializeMesh(res.second->GetRootMesh(), output);
			output << YAML::EndMap;
		}

		// -- End Materials Sequence & Serialization Map --
		output << YAML::EndSeq;
		output << YAML::EndMap;

		// -- Save File --
		std::string filepath = INTERNAL_SETTINGS_PATH + std::string("KaimosResources.kaimossave");
		std::ofstream file(filepath.c_str());
		file << output.c_str();
	}


	void ResourceManager::DeserializeResources()
	{
		// -- File Load --
		YAML::Node data;

		std::string filename = INTERNAL_SETTINGS_PATH + std::string("KaimosResources.kaimossave");
		std::ifstream f(filename.c_str());

		if (!f.good())
		{
			KS_ENGINE_WARN("Error Deserializing Resources, invalid filepath");
			return;
		}

		try { data = YAML::LoadFile(filename); }
		catch (const YAML::ParserException& exception)
		{
			KS_ENGINE_ERROR("Error Deserializing Resources\nError: {0}", exception.what());
			return;
		}

		if (!data["KaimosSaveFile"]) //TODO: This "KaimosSaveFile" should be something global... In general, serialization/deserialization should be carried by a resources mgr
		{
			KS_ENGINE_ERROR("Error Deserializing Resources\nError: Wrong File, it has no 'KaimosSaveFile' node");
			return;
		}

		// -- Setup --
		KS_ENGINE_TRACE("Deserializing Kaimos Resources");
		YAML::Node models_node = data["Models"];

		if (models_node)
		{
			for (auto model_subnode : models_node)
			{
				if (model_subnode["Model"])
				{
					uint model_id = model_subnode["Model"].as<uint>();
					const std::string model_path = model_subnode["Path"].as<std::string>();

					if (model_subnode["RootMesh"])
					{
						DeserializeMesh(model_subnode["RootMesh"]);
					}

					//CreateModel(model_path, model_id);
				}
			}
		}
	}



	// ----------------------- Private Resources Serialization Methods ------------------------------------
	void ResourceManager::SerializeMesh(const Ref<Mesh>& mesh, YAML::Emitter& output_emitter)
	{
		// -- Serialize Mesh --
		output_emitter << YAML::BeginMap;
		output_emitter << YAML::Key << "Mesh" << YAML::Value << mesh->GetID();
		output_emitter << YAML::Key << "Name" << YAML::Value << mesh->GetName();
		output_emitter << YAML::Key << "Material" << YAML::Value << mesh->GetMaterialID();

		// -- Serialize Submeshes (as Sequence) --
		output_emitter << YAML::Key << "Submeshes" << YAML::Value << YAML::BeginSeq;
		for (const Ref<Mesh>& m : mesh->GetSubmeshes())
		{
			output_emitter << YAML::BeginMap;
			SerializeMesh(m, output_emitter);
			output_emitter << YAML::EndMap;
		}

		// -- End Submeshes Sequence & Mesh Map --
		output_emitter << YAML::EndSeq;
		output_emitter << YAML::EndMap;
	}

	void ResourceManager::DeserializeMesh(YAML::Node& yaml_node)
	{
		uint mesh_id = yaml_node["Mesh"].as<uint>();
		uint mat_id = yaml_node["Material"].as<uint>();
		std::string mesh_name = yaml_node["Name"].as<std::string>();

		// -- Create Mesh & Set Material --
		Ref<Mesh> mesh = CreateRef<Mesh>(mesh_name, mesh_id);

		uint mesh_material = Renderer::GetMaterialIfExists(mat_id);
		if (mesh_material == 0)
			mesh_material = Renderer::GetDefaultMaterialID();

		mesh->SetMaterial(mesh_material);

		// -- Add Mesh to Resources --
		AddMesh(mesh);

		// -- Deserialize Submeshes --
		for (auto submesh_node : yaml_node["Submeshes"])
			DeserializeMesh(submesh_node);
	}
}
