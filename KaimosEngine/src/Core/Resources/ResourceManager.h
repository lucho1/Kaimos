#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

namespace Kaimos { class Mesh; }
namespace Kaimos::Resources {

	class ResourceModel;
	class ResourceManager
	{
	public:

		// --- Public Class Methods ---
		static void CleanUp();

		// --- Public Resources Methods ---
		static Ref<ResourceModel> CreateModel(const std::string& filepath);

		// --- Getters ---
		static uint GetModelResourceReferences(uint resource_id);
		static bool CheckIfModelExists(uint model_id);

	private:

		// --- Stored Resources ---
		static std::unordered_map<std::string, Ref<ResourceModel>> m_ModelResources;
	};
}

#endif //_RESOURCEMANAGER_H_
