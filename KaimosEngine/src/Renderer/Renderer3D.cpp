#include "kspch.h"
#include "Renderer3D.h"

#include "Foundations/RenderCommand.h"
#include "Resources/Buffer.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

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
		std::vector<uint> Indices;
		Ref<IndexBuffer> IBuffer			= nullptr;

		NonPBRVertex* NonPBR_VBufferBase	= nullptr;
		NonPBRVertex* NonPBR_VBufferPtr		= nullptr;

		PBRVertex* PBR_VBufferBase			= nullptr;
		PBRVertex* PBR_VBufferPtr			= nullptr;

		Ref<VertexBuffer> VBuffer			= nullptr;
		Ref<VertexArray> VArray				= nullptr;
		Ref<VertexBuffer> PBRVBuffer		= nullptr;
		Ref<VertexArray> PBRVArray			= nullptr;
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

		// -- Vertex, Index Buffers & Array --
		const uint max_vertices = s_3DData->MaxFaces * 4;

		s_3DData->NonPBR_VBufferBase = new NonPBRVertex[max_vertices];
		s_3DData->VArray = VertexArray::Create();
		s_3DData->VBuffer = VertexBuffer::Create(max_vertices * sizeof(NonPBRVertex));

		s_3DData->PBR_VBufferBase = new PBRVertex[max_vertices];
		s_3DData->PBRVArray = VertexArray::Create();
		s_3DData->PBRVBuffer = VertexBuffer::Create(max_vertices * sizeof(PBRVertex));

		s_3DData->IBuffer = IndexBuffer::Create(s_3DData->MaxIndices);

		// -- Vertex Layout & Index Buffer Creation --
		BufferLayout non_pbr_layout = {
			{ SHADER_DATATYPE::FLOAT3,	"a_Position" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Normal" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Tangent" },
			{ SHADER_DATATYPE::FLOAT2,	"a_TexCoord" },
			{ SHADER_DATATYPE::FLOAT4,	"a_Color" },
			{ SHADER_DATATYPE::FLOAT ,	"a_NormalStrength" },
			{ SHADER_DATATYPE::INT,		"a_TexIndex" },
			{ SHADER_DATATYPE::INT,		"a_NormTexIndex" },
			{ SHADER_DATATYPE::INT,		"a_EntityID" },

			{ SHADER_DATATYPE::FLOAT,	"a_Shininess" },
			{ SHADER_DATATYPE::FLOAT,	"a_SpecularStrength" },
			{ SHADER_DATATYPE::INT,		"a_SpecTexIndex" }
		};

		BufferLayout pbr_layout = {
			{ SHADER_DATATYPE::FLOAT3,	"a_Position" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Normal" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Tangent" },
			{ SHADER_DATATYPE::FLOAT2,	"a_TexCoord" },
			{ SHADER_DATATYPE::FLOAT4,	"a_Color" },
			{ SHADER_DATATYPE::FLOAT ,	"a_NormalStrength" },
			{ SHADER_DATATYPE::INT,		"a_TexIndex" },
			{ SHADER_DATATYPE::INT,		"a_NormTexIndex" },
			{ SHADER_DATATYPE::INT,		"a_EntityID" },

			{ SHADER_DATATYPE::FLOAT,	"a_Roughness" },
			{ SHADER_DATATYPE::FLOAT,	"a_Metallic" },
			{ SHADER_DATATYPE::FLOAT,	"a_AmbientOcclusionValue" },
			{ SHADER_DATATYPE::INT,		"a_RoughTexIndex" },
			{ SHADER_DATATYPE::INT,		"a_MetalTexIndex" },
			{ SHADER_DATATYPE::INT,		"a_AOTexIndex" }
		};

		// -- Vertex Array Filling --
		s_3DData->VBuffer->SetLayout(non_pbr_layout);
		s_3DData->VArray->AddVertexBuffer(s_3DData->VBuffer);
		s_3DData->VArray->SetIndexBuffer(s_3DData->IBuffer);

		s_3DData->VArray->Unbind();
		s_3DData->VBuffer->Unbind();

		s_3DData->PBRVBuffer->SetLayout(pbr_layout);
		s_3DData->PBRVArray->AddVertexBuffer(s_3DData->PBRVBuffer);
		s_3DData->PBRVArray->SetIndexBuffer(s_3DData->IBuffer);

		// -- Arrays Unbinding & Indices Deletion --
		s_3DData->PBRVArray->Unbind();
		s_3DData->PBRVBuffer->Unbind();
		s_3DData->IBuffer->Unbind();
	}

	void Renderer3D::Shutdown()
	{
		KS_PROFILE_FUNCTION();

		// This is deleted here (manually), and not treated as smart pointer, waiting for the end of the program lifetime
		// because there is still some code of the graphics (OpenGL) that it has to run to free VRAM (for ex. deleting VArrays, Shaders...)
		delete[] s_3DData->NonPBR_VBufferBase;
		delete[] s_3DData->PBR_VBufferBase;
		delete s_3DData;
	}



	// ----------------------- Public Renderer Methods ----------------------------------------------------
	void Renderer3D::BeginScene()
	{
		KS_PROFILE_FUNCTION();
		Renderer::IsSceneInPBRPipeline() ? s_3DData->PBRVArray->Bind() : s_3DData->VArray->Bind();
		StartBatch();
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

		// -- Set Index Buffer Data --
		s_3DData->IBuffer->SetData(s_3DData->Indices.data(), s_3DData->Indices.size());

		// -- Set Vertex Buffer Data --
		// Data cast: uint8_t = 1 byte large, subtraction give elements in terms of bytes
		Ref<VertexArray> v_array = nullptr;
		if (Renderer::IsSceneInPBRPipeline())
		{
			uint v_data_size = (uint)((uint8_t*)s_3DData->PBR_VBufferPtr - (uint8_t*)s_3DData->PBR_VBufferBase);
			s_3DData->PBRVBuffer->SetData(s_3DData->PBR_VBufferBase, v_data_size);
			v_array = s_3DData->PBRVArray;
		}
		else
		{
			uint v_data_size = (uint)((uint8_t*)s_3DData->NonPBR_VBufferPtr - (uint8_t*)s_3DData->NonPBR_VBufferBase);
			s_3DData->VBuffer->SetData(s_3DData->NonPBR_VBufferBase, v_data_size);
			v_array = s_3DData->VArray;
		}

		// -- Bind Textures & Draw Vertex Array --
		Renderer::BindTextures();
		RenderCommand::DrawIndexed(v_array, s_3DData->IndicesDrawCount);
		++s_3DData->RendererStats.DrawCalls;
	}

	void Renderer3D::StartBatch()
	{
		KS_PROFILE_FUNCTION();
		s_3DData->IndicesDrawCount = 0;
		s_3DData->IndicesCurrentOffset = 0;
		s_3DData->Indices.clear();
		
		if(Renderer::IsSceneInPBRPipeline())
			s_3DData->PBR_VBufferPtr = s_3DData->PBR_VBufferBase;
		else
			s_3DData->NonPBR_VBufferPtr = s_3DData->NonPBR_VBufferBase;
	}

	void Renderer3D::NextBatch()
	{
		KS_PROFILE_FUNCTION();
		Flush();
		StartBatch();
	}

	void Renderer3D::SetBaseVertexData(Vertex* dynamic_vertex, const Vertex& mesh_vertex, const glm::mat4& transform, const Ref<Material>& material, uint albedo_ix, uint norm_ix, uint ent_id)
	{
		dynamic_vertex->Pos = transform * glm::vec4(mesh_vertex.Pos, 1.0f);
		dynamic_vertex->Normal = glm::normalize(glm::vec3(transform * glm::vec4(mesh_vertex.Normal, 0.0f)));
		dynamic_vertex->Tangent = glm::normalize(glm::vec3(transform * glm::vec4(mesh_vertex.Tangent, 0.0f)));
		dynamic_vertex->TexCoord = mesh_vertex.TexCoord;
		
		dynamic_vertex->Color = material->Color;
		dynamic_vertex->Bumpiness = material->Bumpiness;
		
		dynamic_vertex->TexIndex = (int)albedo_ix;
		dynamic_vertex->NormTexIndex = (int)(norm_ix);
		dynamic_vertex->EntityID = (int)ent_id;
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
			// -- Get Material & Check it fits in Batch --
			Ref<Material> material = Renderer::GetMaterial(mesh_component.MaterialID);
			if (!material)
				KS_FATAL_ERROR("Tried to Render a Mesh with a null Material!");

			// -- Update Mesh Timed Vertices --
			static float accumulated_dt = 0.0f;
			accumulated_dt += dt.GetMilliseconds();
			if (accumulated_dt > 200.0f)
			{
				mesh_component.UpdateTimedVertices();
				accumulated_dt = 0.0f;
			}

			// -- Get Texture indexes --
			bool pbr = Renderer::IsSceneInPBRPipeline();
			Renderer::CheckMaterialFitsInBatch(material, &NextBatch);

			uint tex_ix, norm_ix, spec_ix, rough_ix, met_ix, ao_ix;
			tex_ix = Renderer::GetTextureIndex(material->GetTexture(MATERIAL_TEXTURES::ALBEDO), false, &NextBatch);
			norm_ix = Renderer::GetTextureIndex(material->GetTexture(MATERIAL_TEXTURES::NORMAL), true, &NextBatch);

			if (pbr)
			{
				rough_ix = Renderer::GetTextureIndex(material->GetTexture(MATERIAL_TEXTURES::ROUGHNESS), false, &NextBatch);
				met_ix = Renderer::GetTextureIndex(material->GetTexture(MATERIAL_TEXTURES::METALLIC), false, &NextBatch);
				ao_ix = Renderer::GetTextureIndex(material->GetTexture(MATERIAL_TEXTURES::AMBIENT_OC), false, &NextBatch);
			}
			else
				spec_ix = Renderer::GetTextureIndex(material->GetTexture(MATERIAL_TEXTURES::SPECULAR), false, &NextBatch);

			// -- Setup Vertex Array & Vertex Attributes --
			for (uint i = 0; i < mesh->m_Vertices.size(); ++i)
			{
				// Set the vertex data
				Vertex& mesh_vertex = mesh_component.ModifiedVertices[i];
				if (pbr)
				{
					SetBaseVertexData(s_3DData->PBR_VBufferPtr, mesh_vertex, transform, material, tex_ix, norm_ix, entity_id);

					s_3DData->PBR_VBufferPtr->Roughness = material->Roughness;
					s_3DData->PBR_VBufferPtr->Metallic = material->Metallic;
					s_3DData->PBR_VBufferPtr->AmbientOcc = material->AmbientOcclusion;

					s_3DData->PBR_VBufferPtr->RoughTexIndex = (int)rough_ix;
					s_3DData->PBR_VBufferPtr->MetalTexIndex = (int)met_ix;
					s_3DData->PBR_VBufferPtr->AOTexIndex = (int)ao_ix;

					++s_3DData->PBR_VBufferPtr;
				}
				else
				{
					SetBaseVertexData(s_3DData->NonPBR_VBufferPtr, mesh_vertex, transform, material, tex_ix, norm_ix, entity_id);

					s_3DData->NonPBR_VBufferPtr->Shininess = material->Smoothness * 256.0f;
					s_3DData->NonPBR_VBufferPtr->Specularity = material->Specularity;
					s_3DData->NonPBR_VBufferPtr->SpecTexIndex = (int)spec_ix;

					++s_3DData->NonPBR_VBufferPtr;
				}
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
