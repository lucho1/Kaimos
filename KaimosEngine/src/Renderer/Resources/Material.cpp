#include "kspch.h"
#include "Material.h"

#include "Core/Utils/Maths/RandomGenerator.h"
#include "Renderer/Resources/Texture.h"


namespace Kaimos {
	
	// ----------------------- Public Class Methods -------------------------------------------------------
	Material::Material(const std::string& name) : m_Name(name)
	{
		m_ID = (uint)Kaimos::Random::GetRandomInt();
		m_AttachedGraph = CreateScopePtr<MaterialEditor::MaterialGraph>(this);
	}

	Material::~Material()
	{
		RemoveTexture(MATERIAL_TEXTURES::ALBEDO);
		RemoveTexture(MATERIAL_TEXTURES::NORMAL);
		RemoveTexture(MATERIAL_TEXTURES::SPECULAR);
		//RemoveTexture(MATERIAL_TEXTURES::ROUGHNESS);
		//RemoveTexture(MATERIAL_TEXTURES::METALLIC);
		//RemoveTexture(MATERIAL_TEXTURES::AMBIENT_OC);

		if (m_AttachedGraph)
			m_AttachedGraph.reset();
	}


	
	// ----------------------- Private Texture Methods ----------------------------------------------------
	Ref<Texture2D>& Material::GetMaterialTexture(MATERIAL_TEXTURES texture_type)
	{
		switch (texture_type)
		{
			case MATERIAL_TEXTURES::ALBEDO:			return m_Texture;
			case MATERIAL_TEXTURES::NORMAL:			return m_NormalTexture;
			case MATERIAL_TEXTURES::SPECULAR:		return m_SpecularTexture;
			//case MATERIAL_TEXTURES::ROUGHNESS:
			//case MATERIAL_TEXTURES::METALLIC:
			//case MATERIAL_TEXTURES::AMBIENT_OC:
			default:								KS_FATAL_ERROR("[Error] Material: Tried to retrieve a texture from a non-existing texture type!");
		}

		Ref<Texture2D> ret = nullptr;
		return ret;
	}

	std::string& Material::GetMaterialTextureFilepath(MATERIAL_TEXTURES texture_type)
	{
		switch (texture_type)
		{
			case MATERIAL_TEXTURES::ALBEDO:			return m_TextureFilepath;
			case MATERIAL_TEXTURES::NORMAL:			return m_NormalTextureFilepath;
			case MATERIAL_TEXTURES::SPECULAR:		return m_SpecularTexturePath;
			//case MATERIAL_TEXTURES::ROUGHNESS:
			//case MATERIAL_TEXTURES::METALLIC:
			//case MATERIAL_TEXTURES::AMBIENT_OC:
			default:								KS_FATAL_ERROR("[Error] Material: Tried to retrieve a string from a non-existing texture type!");
		}

		std::string ret = "";
		return ret;
	}



	// ----------------------- Public Texture Methods -----------------------------------------------------
	void Material::SetTexture(MATERIAL_TEXTURES texture_type, const std::string& filepath)
	{
		Ref<Texture2D>* texture = &GetMaterialTexture(texture_type);
		std::string* texture_filepath = &GetMaterialTextureFilepath(texture_type);

		Ref<Texture2D> new_texture = Texture2D::Create(filepath);
		if (new_texture)
		{
			RemoveTexture(texture_type);
			*texture = new_texture;
			*texture_filepath = filepath.substr(filepath.find("assets"), filepath.size());
		}
		else
			KS_EDITOR_WARN("Couldn't load Texture from '{0}'", filepath);
	}

	void Material::RemoveTexture(MATERIAL_TEXTURES texture_type)
	{
		Ref<Texture2D>* texture = &GetMaterialTexture(texture_type);
		std::string* texture_filepath = &GetMaterialTextureFilepath(texture_type);

		if (texture && texture->get())
			texture->reset();

		texture_filepath->clear();
	}



	// ----------------------- Texture Getters ------------------------------------------------------------
	uint Material::GetTextureID(MATERIAL_TEXTURES texture_type)
	{
		Ref<Texture2D> texture = GetMaterialTexture(texture_type);
		if (texture)
			return texture->GetTextureID();

		return 0;
	}


	uint Material::GetTextureWidth(MATERIAL_TEXTURES texture_type)
	{
		Ref<Texture2D> texture = GetMaterialTexture(texture_type);
		if (texture)
			return texture->GetWidth();

		return 0;
	}

	uint Material::GetTextureHeight(MATERIAL_TEXTURES texture_type)
	{
		Ref<Texture2D> texture = GetMaterialTexture(texture_type);
		if (texture)
			return texture->GetHeight();

		return 0;
	}

	const Ref<Texture2D>& Material::GetTexture(MATERIAL_TEXTURES texture_type)
	{
		return GetMaterialTexture(texture_type);
	}

	const std::string& Material::GetTextureFilepath(MATERIAL_TEXTURES texture_type)
	{
		return GetMaterialTextureFilepath(texture_type);
	}


	
	// ----------------------- Public Graph Methods -------------------------------------------------------
	bool Material::IsVertexAttributeTimed(MaterialEditor::VertexParameterNodeType vtxpm_node_type) const
	{
		return m_AttachedGraph->IsVertexAttributeTimed(vtxpm_node_type);
	}

	void Material::UpdateVertexParameter(MaterialEditor::VertexParameterNodeType vtxpm_node_type, const glm::vec4& value) const
	{
		m_AttachedGraph->SyncVertexParameterNodes(vtxpm_node_type, value);
	}

	void Material::SyncGraphValuesWithMaterial()
	{
		m_AttachedGraph->SyncMainNodeValuesWithMaterial();
	}

	void Material::RemoveGraph()
	{
		if (m_AttachedGraph)
			m_AttachedGraph.reset();
	}

	// This will set the material graph to the passed one and delete the passed one
	void Material::SetGraphUniqueRef(ScopePtr<MaterialEditor::MaterialGraph>& new_graph)
	{
		if (m_AttachedGraph)
			m_AttachedGraph.reset();

		m_AttachedGraph = std::move(new_graph);
		m_AttachedGraph->SyncMaterialValuesWithGraph();
		new_graph.reset();
	}
}
