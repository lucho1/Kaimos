#ifndef	_MESH_H_
#define _MESH_H_

#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"
#include "Core/Resources/ResourceModel.h"
#include "Buffer.h"

namespace Kaimos {

	namespace Importers { class ImporterModel; }
	class Mesh
	{
		friend class Importers::ImporterModel;
	public:

		// --- Public Class Methods ---
		Mesh(const Ref<VertexArray>& vertex_array, const std::string& name = "Unnamed", uint id = 0, uint material_index = 0)
			: m_VertexArray(vertex_array), m_Name(name), m_MaterialIndex(material_index), m_ID(id == 0 ? (uint)Kaimos::Random::GetRandomInt() : id)
		{
		}

		~Mesh()
		{
			for (int i = m_Submeshes.size() - 1; i >= 0; --i)
				m_Submeshes[i].reset();

			if (m_ParentMesh)
				m_ParentMesh->DeleteSubmesh(this);

			m_Submeshes.clear();
			m_VertexArray.reset();
			m_ParentMesh = nullptr;
		}

	public:

		// --- Setters ---
		void SetName(const std::string& name) { m_Name = name; }
		void SetMaterial(uint material_index) { m_MaterialIndex = material_index; }

		// --- Getters ---
		const std::vector<Ref<Mesh>>& GetSubmeshes() const { return m_Submeshes; }

		uint GetID()					const { return m_ID; }
		uint GetMaterialIndex()			const { return m_MaterialIndex; }
		const std::string& GetName()	const { return m_Name; }

		// --- Public Mesh Methods ---
		void AddSubmesh(const Ref<Mesh>& mesh)
		{
			if (!mesh || !mesh.get())
				return;

			m_Submeshes.push_back(mesh);
			mesh->m_ParentMesh = this;
			mesh->m_ParentModel = m_ParentModel;
		}
		

	private:

		// --- Private Mesh Methods ---
		void DeleteSubmesh(Mesh* submesh_to_delete)
		{
			for (auto& it = m_Submeshes.begin(); it != m_Submeshes.end(); ++it)
			{
				if ((*it)->m_ID == submesh_to_delete->m_ID)
					m_Submeshes.erase(it);
			}
		}

		void SetParentModel(Resources::ResourceModel* model)
		{
			if (model)
			{
				m_ParentModel = model;
				for (Ref<Mesh>& mesh : m_Submeshes)
					mesh->m_ParentModel = model;
			}
		}

	public:

		// --- Private Variables ---
		uint m_ID = 0, m_MaterialIndex = 0;
		std::string m_Name = "Unnamed";

		std::vector<Ref<Mesh>> m_Submeshes;
		Ref<VertexArray> m_VertexArray = nullptr;
		
		Mesh* m_ParentMesh = nullptr;
		Resources::ResourceModel* m_ParentModel = nullptr;
	};
}

#endif //_MESH_H_
