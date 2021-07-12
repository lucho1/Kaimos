#include "kspch.h"
#include "Renderer3D.h"

#include "Foundations/RenderCommand.h"
#include "Resources/Buffer.h"
#include "Resources/Shader.h"
#include "Resources/Mesh.h"

#include <glm/gtc/type_ptr.hpp>


namespace Kaimos {

	// ----------------------- Globals --------------------------------------------------------------------
	struct Renderer3DData
	{
		Renderer3D::Statistics RendererStats;

		Ref<Shader> CurrentShader = nullptr;
		Ref<Texture2D> WhiteTexture = nullptr; // TODO: Put this in the Renderer, don't have duplicates in Renderer2D & 3D
	};

	static Renderer3DData* s_3DData = nullptr;	// On shutdown, this is deleted, and ~VertexArray() called, freeing GPU Memory too
												// (the whole renderer has to be shutdown while we still have a context, otherwise, it will crash!)


	// ----------------------- Renderer Statistics Methods ------------------------------------------------
	void Renderer3D::ResetStats()
	{
		memset(&s_3DData->RendererStats, 0, sizeof(Statistics));
	}

	const Renderer3D::Statistics Renderer3D::GetStats()
	{
		return s_3DData->RendererStats;
	}



	// ----------------------- Public Class Methods -------------------------------------------------------
	void Renderer3D::Init()
	{
		KS_PROFILE_FUNCTION();
		s_3DData = new Renderer3DData();

		// -- Shader Creation --
		s_3DData->CurrentShader = Shader::Create("assets/shaders/3DTextureShader.glsl");

		// -- White Texture Creation --
		uint whiteTextData = 0xffffffff; // Full Fs for every channel there (2x4 channels - rgba -)
		s_3DData->WhiteTexture = Texture2D::Create(1, 1);
		s_3DData->WhiteTexture->SetData(&whiteTextData, sizeof(whiteTextData)); // or sizeof(uint)
	}

	void Renderer3D::Shutdown()
	{
		KS_PROFILE_FUNCTION();
		delete s_3DData;
	}



	// ----------------------- Public Renderer Methods ----------------------------------------------------
	void Renderer3D::BeginScene(const CameraComponent& camera_component, const TransformComponent& transform_component)
	{
		KS_PROFILE_FUNCTION();
		glm::mat4 view_proj = camera_component.Camera.GetProjection() * glm::inverse(transform_component.GetTransform());

		s_3DData->CurrentShader->Bind();
		s_3DData->CurrentShader->SetUMat4("u_ViewProjection", view_proj);
	}

	void Renderer3D::BeginScene(const Camera& camera)
	{
		KS_PROFILE_FUNCTION();
		s_3DData->CurrentShader->Bind();
		s_3DData->CurrentShader->SetUMat4("u_ViewProjection", camera.GetViewProjection());
	}

	void Renderer3D::EndScene()
	{
		KS_PROFILE_FUNCTION();
	}



	// ----------------------- Public Drawing Methods -----------------------------------------------------
	void Renderer3D::DrawMesh(const glm::mat4& transform, const MeshRendererComponent& mesh_component, int entity_id)
	{
		// -- Get Mesh --
		Ref<Mesh> mesh = Renderer::GetMesh(mesh_component.MeshID);
		if (mesh)
		{
			// -- Get Material --
			Ref<Material> material = Renderer::GetMaterial(mesh_component.MaterialID);
			if (!material)
				KS_ENGINE_ASSERT(false, "Tried to Render a Mesh with a null Material!");

			//for (uint i = 0; i < mesh->m_Vertices.size(); ++i)
			//{
			//	// Update the Nodes with vertex parameters on each vertex
			//	material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::POSITION, glm::value_ptr(mesh->m_Vertices[i].Pos));
			//	material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::NORMAL, glm::value_ptr(mesh->m_Vertices[i].Normal));
			//	material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::TEX_COORDS, glm::value_ptr(mesh->m_Vertices[i].TexCoord));
			//
			//	// Get the vertex parameters in the main node once updated the nodes
			//	glm::vec3 vpos = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::POSITION);
			//	glm::vec3 vnorm = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::NORMAL);
			//	glm::vec2 tcoords = material->GetVertexAttributeResult<glm::vec2>(MaterialEditor::VertexParameterNodeType::TEX_COORDS);
			//}

			// -- Get Texture & Send Uniforms --
			if (material->GetTexture())
				material->GetTexture()->Bind();
			else
				s_3DData->WhiteTexture->Bind();

			s_3DData->CurrentShader->SetUInt("u_Texture", 0);
			s_3DData->CurrentShader->SetUMat4("u_Model", transform);
			s_3DData->CurrentShader->SetUFloat4("u_Color", material->Color);

			// -- Draw --
			mesh->m_VertexArray->Bind();
			RenderCommand::DrawIndexed(mesh->m_VertexArray);

			// -- Update Statistics --
			++s_3DData->RendererStats.DrawCalls;
			//s_3DData->RendererStats.IndicesCount += mesh indices
			//s_3DData->RendererStats.VerticesCount += mesh vertices

			// -- Left here in case we batch this too --
			//uint texture_index = GetTextureIndex(material->GetTexture());
			//
			//// -- New Batch if Needed --
			//if (s_Data->QuadIndicesDrawCount >= s_Data->MaxIndices)
			//	NextBatch();
			//
			//// -- Setup Vertex Array & Vertex Attributes --
			//// Get Mesh: with mesh.MeshID
			//Ref<Mesh> mesh = Renderer::GetMesh(mesh_component.MeshID);
			//if (mesh)
			//{
			//	SetupVertexBuffer(transform, entity_id, material, texture_index, mesh->GetMeshVertices());
			//	IncrementIndicesDrawn(mesh->GetMeshIndices());
			//}
			//else
			//{
			//	KS_ENGINE_ERROR("Tried to render a non-existing mesh!");
			//}
		}
	}
}
