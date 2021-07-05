#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"

#include "Renderer/MaterialEditor/MaterialGraph.h"
#include "Renderer/Resources/Texture.h"


namespace Kaimos {

	class Material
	{
		friend class MaterialEditorPanel;
		friend class Renderer;
	public:

		// --- Public Class Methods ---
		Material(const std::string& name) : m_Name(name)
		{
			m_ID = (uint)Kaimos::Random::GetRandomInt();
			m_AttachedGraph = CreateScopePtr<MaterialEditor::MaterialGraph>(this);
		}

		~Material()
		{
			RemoveTexture();
			if (m_AttachedGraph)
				m_AttachedGraph.reset();
		}

	protected:

		// This constructor requires its attached graph to be created and assigned just after this material
		// As is a bit "unsecure" is protected so only renderer (and other friends tho) can access.
		// It is thought to deserialize a material
		Material(uint id, const std::string& name) : m_Name(name) { m_ID = id; }

	public:

		// --- Public Texture Methods ---
		void SetTexture(const std::string& filepath)
		{
			Ref<Texture2D> texture = Texture2D::Create(filepath);
			if (texture)
			{
				RemoveTexture();
				m_Texture = texture;
				m_TextureFilepath = filepath.substr(filepath.find("assets"), filepath.size());
			}
			else
				KS_EDITOR_WARN("Couldn't Load Texture from '{0}'", filepath.c_str());
		}

		void RemoveTexture()
		{
			if(m_Texture)
				m_Texture.reset();

			m_TextureFilepath.clear();
		}

		

	public:

		// --- Public Graph Methods ---
		template<typename T>
		inline T& GetVertexAttributeResult(MaterialEditor::VertexParameterNodeType vtxpm_node_type)
		{
			return m_AttachedGraph->GetVertexParameterResult<T>(vtxpm_node_type);
		}

		inline void UpdateVertexParameter(MaterialEditor::VertexParameterNodeType vtxpm_node_type, float* value) const
		{
			m_AttachedGraph->SyncVertexParameterNodes(vtxpm_node_type, value);
		}

		inline void SyncGraphValuesWithMaterial()
		{
			m_AttachedGraph->SyncMainNodeValuesWithMaterial();
		}

		void RemoveGraph()
		{
			if (m_AttachedGraph)
				m_AttachedGraph.reset();
		}

		// This will set the material graph to the passed one and delete the passed one
		void SetGraphUniqueRef(ScopePtr<MaterialEditor::MaterialGraph>& new_graph)
		{
			if (m_AttachedGraph)
				m_AttachedGraph.reset();

			m_AttachedGraph = std::move(new_graph);
			m_AttachedGraph->SyncMaterialValuesWithGraph();
			new_graph.reset();
		}


	public:

		// --- Getters ---
		const Ref<Texture2D>& GetTexture()	const { return m_Texture; }
		const std::string& GetTexturePath()	const { return m_TextureFilepath; }

		uint GetID()						const { return m_ID; }
		uint GetAttachedGraphID()			const { return m_AttachedGraph->GetID(); }
		const std::string& GetName()		const { return m_Name; }

		
	public:

		// --- Public Variables ---
		glm::vec4 Color = glm::vec4(1.0f);
		float TextureTiling = 1.0f;
		glm::vec2 TextureUVOffset = glm::vec2(0.0f);

	private:

		// --- Private Variables ---
		uint m_ID = 0;
		ScopePtr<MaterialEditor::MaterialGraph> m_AttachedGraph = nullptr;

		Ref<Texture2D> m_Texture = nullptr;
		std::string m_TextureFilepath = "";
		std::string m_Name = "Unnamed";
	};
}

#endif //_MATERIAL_H_
