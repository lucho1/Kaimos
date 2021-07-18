#include "kspch.h"
#include "Renderer.h"

#include "OpenGL/Resources/OGLShader.h"
#include "Resources/Mesh.h"
#include "Renderer2D.h"
#include "Renderer3D.h"

#include <yaml-cpp/yaml.h>


namespace Kaimos {

	ScopePtr<Renderer::RendererData> Renderer::s_RendererData = CreateScopePtr<Renderer::RendererData>();


	// ----------------------- Public Class Methods -------------------------------------------------------
	void Renderer::Init()
	{
		KS_PROFILE_FUNCTION();
		KS_INFO("\n\n--- INITIALIZING KAIMOS RENDERER ---");

		CreateDefaultMaterial(); // Make sure we create it (in case we didn't deserialized)
		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();
	}

	void Renderer::Shutdown()
	{
		KS_INFO("\n\n--- SHUTTING DOWN KAIMOS RENDERER ---");
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();

		for (auto& mat : s_RendererData->Materials)
			mat.second.reset();

		s_RendererData->Materials.clear();
		s_RendererData.reset();
	}



	// ----------------------- Public Renderer Methods -------------------------------------------------------
	// Takes all scene parameters & makes sure shaders we use get the right uniforms
	void Renderer::BeginScene(const Camera& camera)
	{
		s_RendererData->ViewProjectionMatrix = camera.GetViewProjection();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertex_array, const glm::mat4& transformation)
	{
		shader->Bind();
		shader->SetUMat4("u_ViewProjection", s_RendererData->ViewProjectionMatrix);
		shader->SetUMat4("u_Model", transformation);

		// -- Vertex Array bound here since RenderCommands should NOT do multiple things, they are just commands (unless specifically supposed-to) --
		vertex_array->Bind();
		RenderCommand::DrawIndexed(vertex_array);
		//vertexArray->Unbind(); //TODO: ?
	}



	// ----------------------- Public Renderer Serialization Methods -----------------------------------------
	void Renderer::SerializeRenderer()
	{
		KS_PROFILE_FUNCTION();
		KS_TRACE("Serializing Kaimos Renderer");

		// -- Begin Renderer Map --
		YAML::Emitter output;
		output << YAML::BeginMap;
		output << YAML::Key << "KaimosSaveFile" << YAML::Value << "KaimosRenderer";
		
		// -- Serialize Materials (as Sequence) --
		output << YAML::Key << "DefaultMaterialID" << YAML::Value << s_RendererData->DefaultMaterialID;
		output << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;
		for (auto& mat : s_RendererData->Materials)
		{
			output << YAML::BeginMap;
			output << YAML::Key << "Material" << YAML::Value << mat.second->GetID();
			output << YAML::Key << "Name" << YAML::Value << mat.second->GetName();
			output << YAML::Key << "AttachedGraph";
			mat.second->m_AttachedGraph->SerializeGraph(output);
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
		KS_PROFILE_FUNCTION();
		KS_TRACE("Deserializing Kaimos Renderer");

		// -- File Load --
		YAML::Node data;

		std::string filename = INTERNAL_SETTINGS_PATH + std::string("KaimosRendererSettings.kaimossave");
		std::ifstream f(filename.c_str());

		if (!f.good())
		{
			KS_WARN("Couldn't Deserialize Renderer, invalid or non-existent filepath (if it's the first time, the file might not exist yet)");
			return;
		}

		try { data = YAML::LoadFile(filename); }
		catch (const YAML::ParserException& exception)
		{
			KS_ERROR("Error Loading Renderer\nError: {0}", exception.what());
			return;
		}

		if (!data["KaimosSaveFile"])
		{
			KS_ERROR("Error Deserializing Renderer\nError: Wrong File (no 'KaimosSaveFile' node within save file)");
			return;
		}

		// -- Setup --
		if (data["DefaultMaterialID"])
			CreateDefaultMaterial(data["DefaultMaterialID"].as<uint>());
		else
			CreateDefaultMaterial();

		//s_RendererData->DefaultMaterialID = data["DefaultMaterialID"] ? data["DefaultMaterialID"].as<uint>() : Kaimos::Random::GetRandomInt();
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



	// ----------------------- Public Renderer Materials Methods ---------------------------------------------
	Ref<Material> Renderer::CreateMaterial(const std::string& name)
	{
		Ref<Material> material = CreateRef<Material>(name);
		s_RendererData->Materials.insert({ material->GetID(), material });
		return material;
	}

	bool Renderer::IsDefaultMaterial(uint material_id)
	{
		return material_id == s_RendererData->DefaultMaterialID;
	}

	

	// ----------------------- Private Renderer Materials Methods --------------------------------------------
	bool Renderer::MaterialExists(uint material_id)
	{
		return s_RendererData->Materials.find(material_id) != s_RendererData->Materials.end();
	}

	void Renderer::CreateDefaultMaterial(uint default_mat_id)
	{
		if (!MaterialExists(s_RendererData->DefaultMaterialID))
		{
			if (default_mat_id == 0)
			{
				Ref<Material> material = CreateRef<Material>("DefaultMaterial");
				s_RendererData->Materials.insert({ material->GetID(), material });
				s_RendererData->DefaultMaterialID = material->GetID();
			}
			else
			{
				s_RendererData->Materials.insert({ default_mat_id, CreateRef<Material>(new Material(default_mat_id, "DefaultMaterial")) });
				s_RendererData->DefaultMaterialID = default_mat_id;
			}
		}
	}

	Ref<Material> Renderer::CreateMaterialWithID(uint material_id, const std::string& name)
	{
		if (material_id == s_RendererData->DefaultMaterialID)
		{
			if (!MaterialExists(s_RendererData->DefaultMaterialID))
				KS_FATAL_ERROR("Tried to get default material but does not exists");

			return s_RendererData->Materials[s_RendererData->DefaultMaterialID];
		}

		Ref<Material> mat = CreateRef<Material>(new Material(material_id, name));
		s_RendererData->Materials.insert({ material_id, mat });
		return mat;
	}



	// ----------------------- Public Renderer Materials Getters ---------------------------------------------
	Ref<Material> Renderer::GetMaterial(uint material_id)
	{
		if (MaterialExists(material_id))
			return s_RendererData->Materials[material_id];

		return nullptr;
	}

	Ref<Material> Renderer::GetMaterialFromIndex(uint index)
	{
		if (index < s_RendererData->Materials.size())
		{
			uint i = 0;
			for (auto& mat : s_RendererData->Materials)
			{
				if (i == index)
					return mat.second;

				++i;
			}
		}

		return nullptr;
	}

	uint Renderer::GetMaterialIfExists(uint material_id)
	{
		if (MaterialExists(material_id))
			return material_id;

		return 0;
	}

	uint Renderer::GetDefaultMaterialID()
	{
		return s_RendererData->DefaultMaterialID;
	}

	uint Renderer::GetMaterialsQuantity()
	{
		if (s_RendererData)
			return s_RendererData->Materials.size();

		return 0;
	}
}
