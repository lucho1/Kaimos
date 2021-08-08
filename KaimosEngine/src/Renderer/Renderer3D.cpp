#include "kspch.h"
#include "Renderer3D.h"

#include "Foundations/RenderCommand.h"
#include "Resources/Buffer.h"
#include "Resources/Shader.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/Light.h"

#include "Core/Resources/ResourceManager.h"
#include "Scene/ECS/Components.h"

#include <glm/gtc/type_ptr.hpp>


namespace Kaimos {

	// ----------------------- Globals --------------------------------------------------------------------
	struct Renderer3DData
	{
		Renderer3D::Statistics RendererStats;

		static const uint MaxFaces = 20000;
		static const uint MaxIndices = MaxFaces * 6;

		uint IndicesDrawCount = 0;
		uint IndicesCurrentOffset = 0;
		Vertex* VBufferBase			= nullptr;
		Vertex* VBufferPtr			= nullptr;
		std::vector<uint> Indices;

		Ref<VertexArray> VArray		= nullptr;
		Ref<IndexBuffer> IBuffer	= nullptr;
		Ref<VertexBuffer> VBuffer	= nullptr;
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

	const uint Renderer3D::GetMaxFaces()
	{
		return s_3DData->MaxFaces;
	}



	// ----------------------- Public Class Methods -------------------------------------------------------
	void Renderer3D::Init()
	{
		KS_PROFILE_FUNCTION();
		KS_TRACE("Initializing 3D Renderer");
		s_3DData = new Renderer3DData();

		// -- Vertex Buffer & Array --
		const uint max_vertices = s_3DData->MaxFaces * 4;

		s_3DData->VBufferBase = new Vertex[max_vertices];
		s_3DData->VArray = VertexArray::Create();
		s_3DData->VBuffer = VertexBuffer::Create(max_vertices * sizeof(Vertex));

		// -- Vertex Layout & Index Buffer Creation --
		s_3DData->IBuffer = IndexBuffer::Create(s_3DData->MaxIndices);
		BufferLayout layout = {
			{ SHADER_DATATYPE::FLOAT3,	"a_Position" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Normal" },
			{ SHADER_DATATYPE::FLOAT3,	"a_NormalTransformed" },
			{ SHADER_DATATYPE::FLOAT2,	"a_TexCoord" },
			{ SHADER_DATATYPE::FLOAT4,	"a_Color" },
			{ SHADER_DATATYPE::FLOAT ,	"a_TexIndex" },
			{ SHADER_DATATYPE::INT,		"a_EntityID" }
		};

		// -- Vertex Array Filling --
		s_3DData->VBuffer->SetLayout(layout);
		s_3DData->VArray->AddVertexBuffer(s_3DData->VBuffer);
		s_3DData->VArray->SetIndexBuffer(s_3DData->IBuffer);

		// -- Arrays Unbinding & Indices Deletion --
		s_3DData->VArray->Unbind();
		s_3DData->VBuffer->Unbind();
		s_3DData->IBuffer->Unbind();
	}

	void Renderer3D::Shutdown()
	{
		KS_PROFILE_FUNCTION();

		// This is deleted here (manually), and not treated as smart pointer, waiting for the end of the program lifetime
		// because there is still some code of the graphics (OpenGL) that it has to run to free VRAM (for ex. deleting VArrays, Shaders...)
		delete[] s_3DData->VBufferBase;
		delete s_3DData;
	}



	// ----------------------- Public Renderer Methods ----------------------------------------------------
	void Renderer3D::BeginScene(const glm::mat4& view_projection_matrix, const glm::vec3& camera_pos, const std::vector<std::pair<Ref<Light>, glm::vec3>>& dir_lights, const std::vector<std::pair<Ref<PointLight>, glm::vec3>>& point_lights)
	{
		KS_PROFILE_FUNCTION();

		Ref<Shader> shader = Renderer::GetShader("BatchedShader");
		if (shader)
		{
			s_3DData->VArray->Bind();
			shader->Bind();
			shader->SetUMat4("u_ViewProjection", view_projection_matrix);
			shader->SetUFloat3("u_ViewPos", camera_pos);
			shader->SetUFloat3("u_SceneColor", Renderer::GetSceneColor());

			uint max_dir_lights = Renderer::GetMaxDirLights();
			uint dir_lights_size = dir_lights.size() >= max_dir_lights ? max_dir_lights : dir_lights.size();
			shader->SetUInt("u_DirectionalLightsNum", dir_lights_size);

			for (uint i = 0; i < dir_lights_size; ++i)
			{
				std::string light_array_uniform = "u_DirectionalLights[" + std::to_string(i) + "].";
				shader->SetUFloat4(light_array_uniform + "Radiance", dir_lights[i].first->Radiance);
				shader->SetUFloat3(light_array_uniform + "Direction", dir_lights[i].second);
				shader->SetUFloat(light_array_uniform + "Intensity", dir_lights[i].first->Intensity);
			}

			uint max_point_lights = Renderer::GetMaxPointLights();
			uint point_lights_size = point_lights.size() >= max_point_lights ? max_point_lights : point_lights.size();
			shader->SetUInt("u_PointLightsNum", point_lights_size);

			for (uint i = 0; i < point_lights_size; ++i)
			{
				std::string light_array_uniform = "u_PointLights[" + std::to_string(i) + "].";
				shader->SetUFloat4(light_array_uniform + "Radiance", point_lights[i].first->Radiance);
				shader->SetUFloat3(light_array_uniform + "Position", point_lights[i].second);
				shader->SetUFloat(light_array_uniform + "Intensity", point_lights[i].first->Intensity);

				shader->SetUFloat(light_array_uniform + "MinRadius", point_lights[i].first->GetMinRadius());
				shader->SetUFloat(light_array_uniform + "MaxRadius", point_lights[i].first->GetMaxRadius());
				shader->SetUFloat(light_array_uniform + "FalloffFactor", point_lights[i].first->FalloffMultiplier);
				shader->SetUFloat(light_array_uniform + "AttL", point_lights[i].first->GetLinearAttenuationFactor());
				shader->SetUFloat(light_array_uniform + "AttQ", point_lights[i].first->GetQuadraticAttenuationFactor());
			}

			StartBatch();
		}
		else
			KS_FATAL_ERROR("Renderer3D: Tried to Render with a null Shader!");
	}

	void Renderer3D::EndScene()
	{
		KS_PROFILE_FUNCTION();
		Flush();
	}



	// ----------------------- Private Renderer Methods ---------------------------------------------------
	void Renderer3D::Flush()
	{
		KS_PROFILE_FUNCTION();

		// -- Check if something to draw --
		if (s_3DData->IndicesDrawCount == 0)
			return;

		// -- Set Vertex Buffer Data --
		// Data cast: uint8_t = 1 byte large, subtraction give elements in terms of bytes
		uint v_data_size = (uint)((uint8_t*)s_3DData->VBufferPtr - (uint8_t*)s_3DData->VBufferBase);
		s_3DData->VBuffer->SetData(s_3DData->VBufferBase, v_data_size);
		s_3DData->IBuffer->SetData(s_3DData->Indices.data(), s_3DData->Indices.size());

		// -- Bind Textures & Draw Vertex Array --
		Renderer::BindTextures();
		RenderCommand::DrawIndexed(s_3DData->VArray, s_3DData->IndicesDrawCount);
		++s_3DData->RendererStats.DrawCalls;
	}

	void Renderer3D::StartBatch()
	{
		KS_PROFILE_FUNCTION();
		s_3DData->IndicesDrawCount = 0;
		s_3DData->IndicesCurrentOffset = 0;
		s_3DData->VBufferPtr = s_3DData->VBufferBase;
		s_3DData->Indices.clear();
	}

	void Renderer3D::NextBatch()
	{
		KS_PROFILE_FUNCTION();
		Flush();
		StartBatch();
	}



	// ----------------------- Public Drawing Methods -----------------------------------------------------
	void Renderer3D::DrawMesh(Timestep dt, const glm::mat4& transform, MeshRendererComponent& mesh_component, int entity_id)
	{
		// -- New Batch if Needed --
		if (s_3DData->IndicesDrawCount >= s_3DData->MaxIndices)
			NextBatch();

		// -- Get Mesh --
		Ref<Mesh> mesh = Resources::ResourceManager::GetMesh(mesh_component.MeshID);
		if (mesh && mesh->GetVertices().size() == mesh_component.ModifiedVertices.size())
		{
			// -- Get Material & Texture Index --
			Ref<Material> material = Renderer::GetMaterial(mesh_component.MaterialID);
			if (!material)
				KS_FATAL_ERROR("Tried to Render a Mesh with a null Material!");

			uint texture_index = Renderer::GetTextureIndex(material->GetTexture(), &NextBatch);

			// -- Update Mesh Timed Vertices --
			static float accumulated_dt = 0.0f;
			accumulated_dt += dt.GetMilliseconds();
			if (accumulated_dt > 200.0f)
			{
				mesh_component.UpdateTimedVertices();
				accumulated_dt = 0.0f;
			}

			// -- Setup Vertex Array & Vertex Attributes --
			for (uint i = 0; i < mesh->m_Vertices.size(); ++i)
			{
				// Set the vertex data
				Vertex& mesh_vertex = mesh_component.ModifiedVertices[i];

				s_3DData->VBufferPtr->Pos = transform * glm::vec4(mesh_vertex.Pos, 1.0f);
				s_3DData->VBufferPtr->Normal = mesh_vertex.Normal;
				s_3DData->VBufferPtr->NormalTrs = glm::mat3(glm::transpose(glm::inverse(transform))) * mesh_vertex.Normal;
				s_3DData->VBufferPtr->TexCoord = mesh_vertex.TexCoord;

				s_3DData->VBufferPtr->Color = material->Color;
				s_3DData->VBufferPtr->TexIndex = texture_index;
				s_3DData->VBufferPtr->EntityID = entity_id;

				++s_3DData->VBufferPtr;
			}

			// -- Setup Index Buffer --
			std::vector<uint> indices = mesh->m_Indices;
			for (uint i = 0; i < indices.size(); ++i)
				s_3DData->Indices.push_back(s_3DData->IndicesCurrentOffset + indices[i]);

			// -- Update Stats --
			s_3DData->RendererStats.IndicesCount = s_3DData->IndicesDrawCount += indices.size();
			s_3DData->RendererStats.VerticesCount += mesh->m_Vertices.size();
			s_3DData->IndicesCurrentOffset += mesh->m_MaxIndex;
		}
	}
}
