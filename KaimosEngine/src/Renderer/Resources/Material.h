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
	public:

		// --- Public Class Methods ---
		Material()	{ m_ID = (uint)Kaimos::Random::GetRandomInt(); m_AttachedGraph = CreateScopePtr<MaterialEditor::MaterialGraph>(this); }
		~Material()	{ RemoveTexture(); if (m_AttachedGraph) m_AttachedGraph.reset(); }

	public:

		// --- Public Material Methods ---
		void SetTexture(const std::string& filepath)
		{
			Ref<Texture2D> texture = Texture2D::Create(filepath);
			if (texture)
			{
				RemoveTexture();
				m_Texture = texture;
				m_TextureFilepath = filepath;
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

		inline void SyncGraphValuesWithMaterial()
		{
			m_AttachedGraph->SyncMainNodeValuesWithMaterial();
		}


	public:

		// --- Getters ---
		const Ref<Texture2D>& GetTexture()	const { return m_Texture; }
		const std::string& GetTexturePath()	const { return m_TextureFilepath; }

		uint GetID()						const { return m_ID; }
		uint GetAttachedGraphID()			const { return m_AttachedGraph->GetID(); }
		
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
	};
}

#endif //_MATERIAL_H_
