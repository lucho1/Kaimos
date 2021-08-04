#include "kspch.h"
#include "Mesh.h"
#include "Core/Resources/ResourceModel.h"

namespace Kaimos {


	// ----------------------- Public Class Methods ------------------------------------------------------
	Mesh::~Mesh()
	{
		for (int i = m_Submeshes.size() - 1; i >= 0; --i)
			m_Submeshes[i].reset();

		if (m_ParentMesh)
			m_ParentMesh->DeleteSubmesh(this);

		m_Submeshes.clear();
		m_ParentMesh = nullptr;
	}



	// ----------------------- Getters -------------------------------------------------------------------
	const std::string Mesh::GetParentMeshName()	const
	{
		if (m_ParentMesh)
			return m_ParentMesh->GetName();
		
		return "None";
	}

	const std::string Mesh::GetParentModelName() const
	{
		if (m_ParentModel)
			return m_ParentModel->GetName();

		return "None";
	}



	// ----------------------- Public Mesh Methods --------------------------------------------
	void Mesh::AddSubmesh(const Ref<Mesh>& mesh)
	{
		if (!mesh || !mesh.get())
			return;

		m_Submeshes.push_back(mesh);
		mesh->m_ParentMesh = this;
		mesh->m_ParentModel = m_ParentModel;
	}

	

	// ----------------------- Private Mesh Methods ------------------------------------------------------
	void Mesh::DeleteSubmesh(Mesh* submesh_to_delete)
	{
		for (auto& it = m_Submeshes.begin(); it != m_Submeshes.end(); ++it) //URGENT TODO: Change this to a reverse iterator, it's crashing sometimes
		{
			if ((*it)->m_ID == submesh_to_delete->m_ID)
				m_Submeshes.erase(it);
		}
	}

	void Mesh::SetParentModel(Resources::ResourceModel* model)
	{
		if (model)
		{
			m_ParentModel = model;
			for (Ref<Mesh>& mesh : m_Submeshes)
				mesh->m_ParentModel = model;
		}
	}
}
