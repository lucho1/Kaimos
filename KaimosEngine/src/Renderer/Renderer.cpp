#include "kspch.h"
#include "Renderer.h"

#include "Renderer/OpenGL/Resources/OGLShader.h"
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

		// -- Vertex Array bound here since RenderCommands should NOT do multiple things, they are just commands (unless specifically suposed-to) --
		vertex_array->Bind();
		RenderCommand::DrawIndexed(vertex_array);
		//vertexArray->Unbind(); //TODO: ?
	}

	Ref<Material> Renderer::CreateMaterial()
	{
		Ref<Material> material = CreateRef<Material>(new Material());
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
			output << YAML::Key << "AttachedGraph";
			mat->m_AttachedGraph->SerializeGraph(output);
			output << YAML::EndMap;
		}

		// -- End Materials Sequence & Renderer Map --
		output << YAML::EndSeq;
		output << YAML::EndMap;

		// -- Save File --
		std::ofstream file("../KaimosEngine/res/settings/KaimosRendererSettings.kaimossave");
		file << output.c_str();
	}

	void Renderer::DeserializeRenderer()
	{
	}


	
	// ----------------------- Event Methods -----------------------------------------------------------------
	void Renderer::OnWindowResize(uint width, uint height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
}
