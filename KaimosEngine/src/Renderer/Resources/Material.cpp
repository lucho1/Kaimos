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
		RemoveTexture();
		if (m_AttachedGraph)
			m_AttachedGraph.reset();
	}


	
	// ----------------------- Public Texture Methods -----------------------------------------------------
	void Material::SetTexture(const std::string& filepath)
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

	void Material::RemoveTexture()
	{
		if (m_Texture)
			m_Texture.reset();

		m_TextureFilepath.clear();
	}


	
	// ----------------------- Public Graph Methods -------------------------------------------------------
	void Material::UpdateVertexParameter(MaterialEditor::VertexParameterNodeType vtxpm_node_type, float* value) const
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
