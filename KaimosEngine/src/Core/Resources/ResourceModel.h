#ifndef _RESOURCEMODEL_H_
#define _RESOURCEMODEL_H_

#include "Resource.h"
#include "Renderer/Resources/Buffer.h"

namespace Kaimos
{
	class Mesh;

	class ResourceModel : public Resource
	{
	public:

		ResourceModel(const std::string& filepath, uint id = 0) : Resource(RESOURCE_TYPE::MODEL, filepath, id) {}
		~ResourceModel() = default;

	private:
		Mesh* m_RootMesh = nullptr;
	};


	class Mesh
	{
	public:

		//Mesh();
		//~Mesh();

		uint GetID() const { return m_ID; }
		const std::string& GetName() const { return m_Name; }

		uint GetMaterialID() const { return m_MaterialID; }

	private:

		uint m_ID = 0;
		std::string m_Name = "Unnamed";

		uint m_MaterialID = 0;
		std::vector<Ref<Mesh>> m_Submeshes;

		Mesh* m_ParentMesh = nullptr;
		ResourceModel* m_ParentModel = nullptr;

		Ref<VertexArray> m_VertexArray = nullptr;
	};
}

#endif //_RESOURCEMODEL_H_
