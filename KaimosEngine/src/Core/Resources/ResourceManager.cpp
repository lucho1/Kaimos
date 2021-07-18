#include "kspch.h"
#include "ResourceManager.h"
#include "ResourceModel.h"
#include "Importers/ImporterModel.h"
#include "Renderer/Renderer.h"

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
				KS_ENGINE_WARN("Deleting a reference with +1 references loaded!");
		
			model.second.reset();
		}

		for (auto& mesh : m_MeshesResources)
		{
			if (mesh.second.use_count() > 1)
				KS_ENGINE_WARN("Deleting a reference with +1 references loaded!");

			mesh.second.reset();
		}
		
		m_ModelResources.clear();
		m_MeshesResources.clear();
	}



	// ----------------------- Public Resources Methods ---------------------------------------------------
	Ref<ResourceModel> ResourceManager::CreateModel(const std::string& filepath)
	{
		size_t rel_pos = filepath.find("assets");
		if (rel_pos != std::string::npos)
		{
			const std::string relative_path = filepath.substr(rel_pos, filepath.size());
			auto& it = m_ModelResources.find(relative_path);
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
			KS_ERROR("Error Creating model: Cannot load an out-of-project resource, try moving it inside 'assets/' folder.\nCurrent Filepath: {0}", filepath);

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
		KS_TRACE("Serializing Kaimos Resources");

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
		KS_PROFILE_FUNCTION();
		KS_TRACE("Deserializing Kaimos Resources");

		// -- File Load --
		YAML::Node data;

		std::string filename = INTERNAL_SETTINGS_PATH + std::string("KaimosResources.kaimossave");
		std::ifstream f(filename.c_str());

		if (!f.good())
		{
			KS_WARN("Couldn't Deserialize Resources, invalid or non-existent filepath (if it's the first time, the file might not exist yet)");
			return;
		}

		try { data = YAML::LoadFile(filename); }
		catch (const YAML::ParserException& exception)
		{
			KS_ERROR("Error Deserializing Resources\nError: {0}", exception.what());
			return;
		}

		if (!data["KaimosSaveFile"]) //TODO: This "KaimosSaveFile" should be something global... In general, serialization/deserialization should be carried by a resources mgr
		{
			KS_ERROR("Error Deserializing Resources\nError: Wrong File (no 'KaimosSaveFile' node within save file)");
			return;
		}

		// -- Setup --
		YAML::Node models_node = data["Models"];

		if (models_node)
		{
			for (auto model_subnode : models_node)
			{
				if (model_subnode["Model"])
				{
					uint model_id = model_subnode["Model"].as<uint>();
					const std::string model_path = model_subnode["Path"].as<std::string>();

					if (m_ModelResources.find(model_path) == m_ModelResources.end())
					{
						if (model_subnode["RootMesh"])
						{
							Ref<Mesh> root_mesh = DeserializeMesh(model_subnode["RootMesh"]);
							DeserializeModel(model_path, model_id, root_mesh);
						}
					}
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


	const Ref<Mesh> ResourceManager::DeserializeMesh(YAML::Node& yaml_node)
	{
		// -- Get Mesh Data --
		uint mesh_id = yaml_node["Mesh"].as<uint>();
		uint mat_id = yaml_node["Material"].as<uint>();

		uint mesh_material = Renderer::GetMaterialIfExists(mat_id);
		if (mesh_material == 0)
			mesh_material = Renderer::GetDefaultMaterialID();

		// -- Add Mesh to Resources --
		std::string mesh_name = yaml_node["Name"] ? yaml_node["Name"].as<std::string>() : "MESH_NONAME_ONLOAD";
		Ref<Mesh> mesh = CreateRef<Mesh>(mesh_name, mesh_id, mesh_material);
		AddMesh(mesh);

		// -- Deserialize Submeshes --
		for (auto submesh_node : yaml_node["Submeshes"])
			mesh->AddSubmesh(DeserializeMesh(submesh_node));

		// -- Return Root Mesh --
		return mesh;
	}


	void ResourceManager::DeserializeModel(const std::string& model_path, uint model_id, Ref<Mesh>& root_mesh)
	{
		if (model_path.find("assets") != std::string::npos)
		{
			Ref<ResourceModel> model = Kaimos::Importers::ImporterModel::LoadDeserializedModel(model_path, model_id, root_mesh);
			if (model)
				m_ModelResources.insert({ model->GetFilepath(), model });
			else
				KS_ERROR("Error Deserializing model: Couldn't Import Model");
		}
		else
			KS_ERROR("Error Deserializing model: Cannot load an out-of-project resource, try moving it inside 'assets/' folder.\nCurrent Filepath: {0}", model_path);
	}
}
