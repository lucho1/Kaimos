#ifndef _RESOURCEMODEL_H_
#define _RESOURCEMODEL_H_

#include "Resource.h"
#include "Renderer/Resources/Mesh.h"

namespace Kaimos::Importers { class ImporterModel; }
namespace Kaimos::Resources {

	class ResourceModel : public Resource
	{
		friend class Kaimos::Importers::ImporterModel;
	public:
		~ResourceModel() = default;
		const Ref<Kaimos::Mesh>& GetRootMesh() const { return m_RootMesh; }

	protected:
		ResourceModel(const std::string& filepath, uint id = 0, Ref<Kaimos::Mesh> root_mesh = nullptr)
			: Resource(RESOURCE_TYPE::MODEL, filepath, id), m_RootMesh(root_mesh) {}

		Ref<Kaimos::Mesh> m_RootMesh = nullptr;
	};
}

#endif //_RESOURCEMODEL_H_
