#include "kspch.h"
#include "Renderer.h"

#include "OpenGL/Resources/OGLShader.h"
#include "Resources/Mesh.h"
#include "Renderer2D.h"

#include <yaml-cpp/yaml.h>


namespace Kaimos {

	ScopePtr<Renderer::SceneData> Renderer::s_SceneData = CreateScopePtr<Renderer::SceneData>();


	// ----------------------- Public Class Methods -------------------------------------------------------
	void Renderer::Init()
	{
		KS_PROFILE_FUNCTION();
		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();

		for (Ref<Material>& mat : s_SceneData->Materials)
			mat.reset();

		for (auto& mesh : s_SceneData->Meshes)
			mesh.second.reset();

		s_SceneData->Meshes.clear();
		s_SceneData->Materials.clear();
		s_SceneData.reset();
	}



	// ----------------------- Public Renderer Methods -------------------------------------------------------
	// Takes all scene parameters & makes sure shaders we use get the right uniforms
	void Renderer::BeginScene(const Camera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjection();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertex_array, const glm::mat4& transformation)
	{
		shader->Bind();
		shader->SetUMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetUMat4("u_Model", transformation);

		// -- Vertex Array bound here since RenderCommands should NOT do multiple things, they are just commands (unless specifically supposed-to) --
		vertex_array->Bind();
		RenderCommand::DrawIndexed(vertex_array);
		//vertexArray->Unbind(); //TODO: ?
	}



	// ----------------------- Public Renderer Materials Methods ---------------------------------------------
	void Renderer::CreateMesh(const Ref<Mesh>& mesh)
	{
		uint mesh_id = mesh->GetID();
		if (s_SceneData->Meshes.find(mesh_id) == s_SceneData->Meshes.end())
			s_SceneData->Meshes.insert({ mesh_id, mesh });
	}

	void Renderer::CreateDefaultMaterial()
	{
		if (s_SceneData->Materials.size() == 0)
			s_SceneData->Materials.push_back(CreateRef<Material>(new Material("DefaultMaterial")));
		else if (s_SceneData->Materials[0]->GetName() != "DefaultMaterial")
			s_SceneData->Materials.emplace(s_SceneData->Materials.begin(), CreateRef<Material>(new Material("DefaultMaterial")));
	}

	Ref<Material> Renderer::CreateMaterial(const std::string& name)
	{
		Ref<Material> material = CreateRef<Material>(new Material(name));
		s_SceneData->Materials.push_back(material);
		return material;
	}

	Ref<Material> Renderer::CreateMaterialWithID(uint material_id, const std::string& name)
	{
		if (name == "DefaultMaterial" && s_SceneData->Materials.size() > 0)
			return s_SceneData->Materials[0];

		Ref<Material> material = CreateRef<Material>(new Material(material_id, name));
		s_SceneData->Materials.push_back(material);
		return material;
	}

	Ref<Material> Renderer::GetMaterial(uint material_id)
	{
		for (Ref<Material>& mat : s_SceneData->Materials)
		{
			if (mat->GetID() == material_id)
				return mat;
		}

		return nullptr;
	}

	uint Renderer::GetMaterialIfExists(uint material_id)
	{
		Ref<Material> mat = GetMaterial(material_id);
		if (mat && mat.get())
			return mat->GetID();
		
		return 0;
	}

	Kaimos::Ref<Kaimos::Material> Renderer::GetMaterialFromIndex(uint index)
	{
		if (s_SceneData && index < s_SceneData->Materials.size())
			return s_SceneData->Materials[index];

		return nullptr;
	}

	uint Renderer::GetMaterialsQuantity()
	{
		if (s_SceneData)
			return s_SceneData->Materials.size();

		return 0;
	}



	// ----------------------- Public Renderer Serialization Methods -----------------------------------------
	void Renderer::SerializeRenderer()
	{
		KS_PROFILE_FUNCTION();

		// -- Begin Renderer Map --
		YAML::Emitter output;
		output << YAML::BeginMap;
		output << YAML::Key << "KaimosSaveFile" << YAML::Value << "KaimosRenderer";
		
		// -- Serialize Materials (as Sequence) --
		output << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;
		for (Ref<Material>& mat : s_SceneData->Materials)
		{
			output << YAML::BeginMap;
			output << YAML::Key << "Material" << YAML::Value << mat->GetID();
			output << YAML::Key << "Name" << YAML::Value << mat->GetName();
			output << YAML::Key << "AttachedGraph";
			mat->m_AttachedGraph->SerializeGraph(output);
			output << YAML::EndMap;
		}

		// -- End Materials Sequence & Renderer Map --
		output << YAML::EndSeq;
		output << YAML::EndMap;

		// -- Save File --
		std::string filepath = INTERNAL_SETTINGS_PATH + std::string("KaimosRendererSettings.kaimossave");
		std::ofstream file(filepath.c_str());
		file << output.c_str();
	}

	void Renderer::DeserializeRenderer()
	{
		// -- File Load --
		YAML::Node data;

		std::string filename = INTERNAL_SETTINGS_PATH + std::string("KaimosRendererSettings.kaimossave");
		std::ifstream f(filename.c_str());

		if (!f.good())
		{
			KS_ENGINE_WARN("Error Loading Renderer, filepath invalid");
			return;
		}

		try { data = YAML::LoadFile(filename); }
		catch (const YAML::ParserException& exception)
		{
			KS_ENGINE_ERROR("Error Loading Renderer\nError: {0}", exception.what());
			return;
		}

		if (!data["KaimosSaveFile"])
		{
			KS_ENGINE_ERROR("Error Loading Renderer\nError: Wrong File, it has no 'KaimosSaveFile' node");
			return;
		}

		// -- Scene Setup --
		KS_ENGINE_TRACE("Deserializing KaimosRenderer");
		YAML::Node materials_node = data["Materials"];
		if (materials_node)
		{
			for (auto material_subnode : materials_node)
			{
				auto graph_subnode = material_subnode["AttachedGraph"];
				if (material_subnode["Material"] && graph_subnode)
				{
					// Get or Create Material with ID
					uint mat_id = material_subnode["Material"].as<uint>();
					Ref<Material> material = GetMaterial(mat_id);
					if (!material)
					{
						const std::string mat_name = material_subnode["Name"] ? material_subnode["Name"].as<std::string>() : "MAT_NONAME_ONLOAD";
						material = CreateMaterialWithID(mat_id, mat_name);
					}

					// Remove the Graph (if exists)
					material->RemoveGraph();

					// Create a graph with ID and call him to deseralize passing file
					uint graph_id = graph_subnode["MaterialGraph"].as<uint>();
					ScopePtr<MaterialEditor::MaterialGraph> mat_graph = CreateScopePtr<MaterialEditor::MaterialGraph>(new MaterialEditor::MaterialGraph(graph_id));

					std::string mat_texture_file;
					mat_graph->DeserializeGraph(graph_subnode, material, mat_texture_file);

					// Finally, assign graph & texture to material
					material->SetGraphUniqueRef(mat_graph);
					if(!mat_texture_file.empty())
						material->SetTexture(mat_texture_file);
				}
			}
		}
	}


	
	// ----------------------- Event Methods -----------------------------------------------------------------
	void Renderer::OnWindowResize(uint width, uint height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}



	// ----------------------- Getters -----------------------------------------------------------------------
	bool Renderer::IsDefaultMaterial(uint material_id)
	{
		if (s_SceneData->Materials.size() > 1)
			return material_id == s_SceneData->Materials[0]->GetID();

		return false;
	}

	uint Renderer::GetDefaultMaterialID()
	{
		if (s_SceneData->Materials.size() > 1)
			return s_SceneData->Materials[0]->GetID();

		return 0;
	}
}
