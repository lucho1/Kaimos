#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Core/Core.h"
#include "Renderer/MaterialEditor/MaterialGraph.h"

namespace Kaimos {

	class Texture2D;
	enum class MATERIAL_TEXTURES { ALBEDO = 0, NORMAL, SPECULAR/*, ROUGHNESS, METALLIC, AMBIENT_OC*/ };

	class Material
	{
		friend class MaterialEditorPanel;
		friend class Renderer;
	public:

		// --- Public Class Methods ---
		Material(const std::string& name);
		~Material();

	protected:

		// This constructor requires its attached graph to be created and assigned just after this material
		// As is a bit "insecure" is protected so only renderer (and other friends tho) can access.
		// It is thought to deserialize a material
		Material(uint id, const std::string& name) : m_Name(name) { m_ID = id; }

	private:

		// --- Private Texture Methods ---
		Ref<Texture2D>& GetMaterialTexture(MATERIAL_TEXTURES texture_type);
		std::string& GetMaterialTextureFilepath(MATERIAL_TEXTURES texture_type);

	public:

		// --- Public Texture Methods ---
		void SetTexture(MATERIAL_TEXTURES texture_type, const std::string& filepath);
		void RemoveTexture(MATERIAL_TEXTURES texture_type);

		// --- Texture Getters ---
		uint GetTextureID(MATERIAL_TEXTURES texture_type);
		uint GetTextureWidth(MATERIAL_TEXTURES texture_type);
		uint GetTextureHeight(MATERIAL_TEXTURES texture_type);

		const Ref<Texture2D>& GetTexture(MATERIAL_TEXTURES texture_type);
		const std::string& GetTextureFilepath(MATERIAL_TEXTURES texture_type);

		bool HasAlbedo()	const { return m_Texture != nullptr; }
		bool HasNormal()	const { return m_NormalTexture != nullptr; }
		bool HasSpecular()	const { return m_SpecularTexture != nullptr; }
		

	public:

		// --- Public Graph Methods ---
		bool IsVertexAttributeTimed(MaterialEditor::VertexParameterNodeType vtxpm_node_type) const;
		void UpdateVertexParameter(MaterialEditor::VertexParameterNodeType vtxpm_node_type, const glm::vec4& value) const;
		void SyncGraphValuesWithMaterial();
		void RemoveGraph();

		// This will set the material graph to the passed one and delete the passed one
		void SetGraphUniqueRef(ScopePtr<MaterialEditor::MaterialGraph>& new_graph);

		template<typename T>
		inline T& GetVertexAttributeResult(MaterialEditor::VertexParameterNodeType vtxpm_node_type)
		{
			return m_AttachedGraph->GetVertexParameterResult<T>(vtxpm_node_type);
		}


	public:

		// --- Getters ---
		uint GetID()								const { return m_ID; }
		uint GetAttachedGraphID()					const { return m_AttachedGraph->GetID(); }
		const std::string& GetName()				const { return m_Name; }
		
	public:

		// --- Public Variables ---
		glm::vec4 Color = glm::vec4(1.0f);
		float Smoothness = 0.5f, Bumpiness = 1.0f, Specularity = 1.0f; // Non-PBR Values
		float Roughness = 0.5f, Metallic = 0.5f, AmbientOcclusion = 0.5f; // PBR Values
		bool InPBRPipeline = false;

	private:

		// --- Private Variables ---
		uint m_ID = 0;
		ScopePtr<MaterialEditor::MaterialGraph> m_AttachedGraph = nullptr;

		Ref<Texture2D> m_Texture = nullptr, m_NormalTexture = nullptr, m_SpecularTexture = nullptr;
		std::string m_TextureFilepath = "", m_NormalTextureFilepath = "", m_SpecularTexturePath = "";
		std::string m_Name = "Unnamed";
	};
}

#endif //_MATERIAL_H_
