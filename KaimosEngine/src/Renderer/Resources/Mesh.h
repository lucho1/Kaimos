#ifndef	_MESH_H_
#define _MESH_H_

#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"
#include "Renderer/Renderer3D.h"
#include "Buffer.h"

namespace Kaimos {

	namespace Resources { class ResourceModel; }
	namespace Importers { class ImporterModel; }

	class Mesh
	{
		friend class Importers::ImporterModel;
		friend class Renderer3D;
	public:

		// --- Public Class Methods ---
		Mesh(const Ref<VertexArray>& vertex_array, const std::string& name = "Unnamed", uint id = 0, uint material_id = 0)
			: m_VertexArray(vertex_array), m_Name(name), m_MaterialID(material_id), m_ID(id == 0 ? (uint)Kaimos::Random::GetRandomInt() : id)
		{
		}

		~Mesh();

	public:

		// --- Setters ---
		void SetMaterial(uint material_id)		{ m_MaterialID = material_id; }

		// --- Getters ---
		uint GetID()					const { return m_ID; }
		uint GetMaterialID()			const { return m_MaterialID; }
		const std::string& GetName()	const { return m_Name; }

		const std::string& GetParentMeshName()				const;
		const std::string& GetParentModelName()				const;
		const std::vector<Ref<Mesh>>& GetSubmeshes()		const { return m_Submeshes; }
		const Ref<VertexArray>& GetVertexArray()			const { return m_VertexArray; }
		const std::vector<Vertex>& GetVertices()			const { return m_Vertices; }

		// --- Public Mesh Methods ---
		void AddSubmesh(const Ref<Mesh>& mesh);
		

	private:

		// --- Private Mesh Methods ---
		void DeleteSubmesh(Mesh* submesh_to_delete);
		void SetParentModel(Resources::ResourceModel* model);
		void SetMeshVertices(const std::vector<Vertex>& mesh_vertices) { m_Vertices = mesh_vertices; }
		void SetMeshIndices(const std::vector<uint>& mesh_indices) { m_Indices = mesh_indices; }

	private:

		// --- Private Variables ---
		uint m_ID = 0, m_MaterialID = 0;
		std::string m_Name = "Unnamed";

		std::vector<Vertex> m_Vertices;
		std::vector<uint> m_Indices;
		uint m_MaxIndex = 0;

		std::vector<Ref<Mesh>> m_Submeshes;
		Ref<VertexArray> m_VertexArray = nullptr;
		
		Mesh* m_ParentMesh = nullptr;
		Resources::ResourceModel* m_ParentModel = nullptr;
	};
}

#endif //_MESH_H_
