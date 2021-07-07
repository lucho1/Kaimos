#include "kspch.h"
#include "ResourceManager.h"
#include "ResourceModel.h"
#include "Importers/ImporterModel.h"


namespace Kaimos::Resources {
		
	// ----------------------- Variables Initialization ---------------------------------------------------
	std::unordered_map<std::string, Ref<ResourceModel>> ResourceManager::m_ModelResources = {};



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
		
		m_ModelResources.clear();
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
				m_ModelResources.insert({ filepath, model });
				return model;
			}
		}
		else
		{
			KS_ENGINE_ERROR("Filepath is not within project folders");
		}

		return nullptr;
	}

	

	// ----------------------- Getters --------------------------------------------------------------------
	uint ResourceManager::GetModelResourceReferences(uint resource_id)
	{
		for (auto& model : m_ModelResources)
		{
			if (model.second->GetID() == resource_id)
				return model.second.use_count();
		}

		return 0;
	}

	bool ResourceManager::CheckIfModelExists(uint model_id)
	{
		for (auto& model : m_ModelResources)
		{
			if (model.second->GetID() == model_id)
				return true;
		}

		return false;
	}

}
