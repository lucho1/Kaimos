#include "kspch.h"
#include "Renderer2D.h"

#include "Foundations/RenderCommand.h"
#include "Resources/Buffer.h"
#include "Resources/Material.h"

#include "Scene/ECS/Components.h"

#include <glm/gtc/type_ptr.hpp>


namespace Kaimos {
	
	// ----------------------- Globals --------------------------------------------------------------------
	struct Renderer2DData
	{
		Renderer2D::Statistics RendererStats;

		static const uint MaxQuads = 20000;
		static const uint MaxIndices = MaxQuads * 6;

		uint QuadIndicesDrawCount = 0;
		NonPBRQuadVertex* NonPBR_QuadVBufferBase	= nullptr;
		NonPBRQuadVertex* NonPBR_QuadVBufferPtr		= nullptr;

		PBRQuadVertex* PBR_QuadVBufferBase			= nullptr;
		PBRQuadVertex* PBR_QuadVBufferPtr			= nullptr;

		Ref<VertexArray> QuadVArray					= nullptr;
		Ref<VertexBuffer> QuadVBuffer				= nullptr;
		Ref<VertexArray> PBRQuadVArray				= nullptr;
		Ref<VertexBuffer> PBRQuadVBuffer			= nullptr;
	};
	
	static Renderer2DData* s_Data = nullptr;	// On shutdown, this is deleted, and ~VertexArray() called, freeing GPU Memory too
												// (the whole renderer has to be shutdown while we still have a context, otherwise, it will crash!)

	
	// ----------------------- Renderer Statistics Methods ------------------------------------------------
	void Renderer2D::ResetStats()
	{
		memset(&s_Data->RendererStats, 0, sizeof(Statistics));
	}
	
	const Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data->RendererStats;
	}

	const uint Renderer2D::GetMaxQuads()
	{
		return s_Data->MaxQuads;
	}


	
	// ----------------------- Public Class Methods -------------------------------------------------------
	void Renderer2D::Init()
	{
		KS_PROFILE_FUNCTION();
		KS_TRACE("Initializing 2D Renderer");
		s_Data = new Renderer2DData();

		// -- Index Buffer --
		Ref<IndexBuffer> index_buffer;
		uint* quad_indices = new uint[s_Data->MaxIndices];

		uint offset = 0;
		for (uint i = 0; i < s_Data->MaxIndices; i += 6)
		{
			quad_indices[i + 0] = offset + 0;
			quad_indices[i + 1] = offset + 1;
			quad_indices[i + 2] = offset + 2;

			quad_indices[i + 3] = offset + 2;
			quad_indices[i + 4] = offset + 3;
			quad_indices[i + 5] = offset + 0;

			offset += 4;
		}
		
		// -- Vertex Buffer & Array --
		const uint max_vertices = s_Data->MaxQuads * 4;

		s_Data->NonPBR_QuadVBufferBase = new NonPBRQuadVertex[max_vertices];
		s_Data->QuadVArray = VertexArray::Create();
		s_Data->QuadVBuffer = VertexBuffer::Create(max_vertices * sizeof(NonPBRQuadVertex));

		s_Data->PBR_QuadVBufferBase = new PBRQuadVertex[max_vertices];
		s_Data->PBRQuadVArray = VertexArray::Create();
		s_Data->PBRQuadVBuffer = VertexBuffer::Create(max_vertices * sizeof(PBRQuadVertex));

		// -- Vertex Layout & Index Buffer Creation --
		index_buffer = IndexBuffer::Create(quad_indices, s_Data->MaxIndices);
		BufferLayout non_pbr_layout = {
			{ SHADER_DATATYPE::FLOAT3,	"a_Position" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Normal" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Tangent" },
			{ SHADER_DATATYPE::FLOAT2,	"a_TexCoord" },
			{ SHADER_DATATYPE::FLOAT4,	"a_Color" },
			{ SHADER_DATATYPE::FLOAT ,	"a_NormalStrength" },
			{ SHADER_DATATYPE::FLOAT ,	"a_TexIndex" },
			{ SHADER_DATATYPE::FLOAT ,	"a_NormTexIndex" },
			{ SHADER_DATATYPE::INT ,	"a_EntityID" },

			{ SHADER_DATATYPE::FLOAT ,	"a_Shininess" },
			{ SHADER_DATATYPE::FLOAT ,	"a_SpecularStrength" },
			{ SHADER_DATATYPE::FLOAT ,	"a_SpecTexIndex" }
		};

		BufferLayout pbr_layout = {
			{ SHADER_DATATYPE::FLOAT3,	"a_Position" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Normal" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Tangent" },
			{ SHADER_DATATYPE::FLOAT2,	"a_TexCoord" },
			{ SHADER_DATATYPE::FLOAT4,	"a_Color" },
			{ SHADER_DATATYPE::FLOAT ,	"a_NormalStrength" },
			{ SHADER_DATATYPE::FLOAT ,	"a_TexIndex" },
			{ SHADER_DATATYPE::FLOAT ,	"a_NormTexIndex" },
			{ SHADER_DATATYPE::INT ,	"a_EntityID" },

			{ SHADER_DATATYPE::FLOAT ,	"a_Roughness" },
			{ SHADER_DATATYPE::FLOAT ,	"a_Metallic" },
			{ SHADER_DATATYPE::FLOAT ,	"a_AmbientOcclusionValue" },
			{ SHADER_DATATYPE::FLOAT ,	"a_MetalTexIndex" },
			{ SHADER_DATATYPE::FLOAT ,	"a_RoughTexIndex" },
			{ SHADER_DATATYPE::FLOAT ,	"a_AOTexIndex" }
		};

		// -- Vertex Array Filling --
		s_Data->QuadVBuffer->SetLayout(non_pbr_layout);
		s_Data->QuadVArray->AddVertexBuffer(s_Data->QuadVBuffer);
		s_Data->QuadVArray->SetIndexBuffer(index_buffer);

		s_Data->QuadVArray->Unbind();
		s_Data->QuadVBuffer->Unbind();

		s_Data->PBRQuadVBuffer->SetLayout(pbr_layout);
		s_Data->PBRQuadVArray->AddVertexBuffer(s_Data->PBRQuadVBuffer);
		s_Data->PBRQuadVArray->SetIndexBuffer(index_buffer);

		// -- Arrays Unbinding & Indices Deletion --
		s_Data->PBRQuadVArray->Unbind();
		s_Data->PBRQuadVBuffer->Unbind();
		index_buffer->Unbind();
		delete[] quad_indices;
	}


	void Renderer2D::Shutdown()
	{
		KS_PROFILE_FUNCTION();
		KS_TRACE("Shutting Down 2D Renderer");

		// This is deleted here (manually), and not treated as smart pointer, waiting for the end of the program lifetime
		// because there is still some code of the graphics (OpenGL) that it has to run to free VRAM (for ex. deleting VArrays, Shaders...)
		delete[] s_Data->NonPBR_QuadVBufferBase;
		delete[] s_Data->PBR_QuadVBufferBase;
		delete s_Data;
	}


	
	// ----------------------- Public Renderer Methods ----------------------------------------------------
	void Renderer2D::BeginScene()
	{
		KS_PROFILE_FUNCTION();
		Renderer::IsSceneInPBRPipeline() ? s_Data->PBRQuadVArray->Bind() : s_Data->QuadVArray->Bind();
		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		KS_PROFILE_FUNCTION();
		Flush();
	}



	// ----------------------- Private Renderer Methods ---------------------------------------------------
	void Renderer2D::Flush()
	{
		KS_PROFILE_FUNCTION();

		// -- Check if something to draw --
		if (s_Data->QuadIndicesDrawCount == 0)
			return;

		// -- Set Vertex Buffer Data --
		// Data cast: uint8_t = 1 byte large, subtraction give elements in terms of bytes
		Ref<VertexArray> v_array = nullptr;
		if (Renderer::IsSceneInPBRPipeline())
		{
			uint data_size = (uint)((uint8_t*)s_Data->PBR_QuadVBufferPtr - (uint8_t*)s_Data->PBR_QuadVBufferBase);
			s_Data->PBRQuadVBuffer->SetData(s_Data->PBR_QuadVBufferBase, data_size);
			v_array = s_Data->PBRQuadVArray;
		}
		else
		{
			uint data_size = (uint)((uint8_t*)s_Data->NonPBR_QuadVBufferPtr - (uint8_t*)s_Data->NonPBR_QuadVBufferBase);
			s_Data->QuadVBuffer->SetData(s_Data->NonPBR_QuadVBufferBase, data_size);
			v_array = s_Data->QuadVArray;
		}

		// -- Bind Textures & Draw Vertex Array --
		Renderer::BindTextures();
		RenderCommand::DrawIndexed(v_array, s_Data->QuadIndicesDrawCount);
		++s_Data->RendererStats.DrawCalls;
	}
	
	void Renderer2D::StartBatch()
	{
		KS_PROFILE_FUNCTION();
		s_Data->QuadIndicesDrawCount = 0;

		if(Renderer::IsSceneInPBRPipeline())
			s_Data->PBR_QuadVBufferPtr = s_Data->PBR_QuadVBufferBase;
		else
			s_Data->NonPBR_QuadVBufferPtr = s_Data->NonPBR_QuadVBufferBase;
	}
	
	void Renderer2D::NextBatch()
	{
		KS_PROFILE_FUNCTION();
		Flush();
		StartBatch();
	}

	void Renderer2D::SetBaseVertexData(QuadVertex* dynamic_vertex, const QuadVertex& quad_vertex, const glm::mat4& transform, const Ref<Material>& material, uint albedo_ix, uint norm_ix, uint ent_id)
	{
		dynamic_vertex->Pos = transform * glm::vec4(quad_vertex.Pos, 1.0f);
		dynamic_vertex->Normal = glm::normalize(glm::vec3(transform * glm::vec4(quad_vertex.Normal, 0.0f)));
		dynamic_vertex->Tangent = glm::normalize(glm::vec3(transform * glm::vec4(quad_vertex.Tangent, 0.0f)));
		dynamic_vertex->TexCoord = quad_vertex.TexCoord;

		dynamic_vertex->Color = material->Color;
		dynamic_vertex->Bumpiness = material->Bumpiness;

		dynamic_vertex->TexIndex = albedo_ix;
		dynamic_vertex->NormTexIndex = norm_ix;
		dynamic_vertex->EntityID = ent_id;
	}


	
	// ----------------------- Drawing Methods ------------------------------------------------------------
	void Renderer2D::DrawSprite(Timestep dt, const glm::mat4& transform, SpriteRendererComponent& sprite_component, int entity_id)
	{
		// -- New Batch if Needed --
		if (s_Data->QuadIndicesDrawCount >= s_Data->MaxIndices)
			NextBatch();

		// -- Get Material & Check it fits in Batch --
		Ref<Material> material = Renderer::GetMaterial(sprite_component.SpriteMaterialID);
		if (!material)
			KS_FATAL_ERROR("Tried to Render a Sprite with a null Material!");

		// -- Update Sprite Timed Vertices --
		static float accumulated_dt = 0.0f;
		accumulated_dt += dt.GetMilliseconds();
		if (accumulated_dt > 200.0f)
		{
			sprite_component.UpdateTimedVertices();
			accumulated_dt = 0.0f;
		}

		// -- Get Texture indexes --
		bool pbr = Renderer::IsSceneInPBRPipeline();
		Renderer::CheckMaterialFitsInBatch(material, &NextBatch);

		uint tex_ix, norm_ix, spec_ix, met_ix, rough_ix, ao_ix;
		tex_ix = Renderer::GetTextureIndex(material->GetTexture(), false, &NextBatch);
		norm_ix = Renderer::GetTextureIndex(material->GetNormalTexture(), true, &NextBatch);

		if (pbr)
		{
			met_ix = 0;
			rough_ix = 0;
			ao_ix = 0;
		}
		else
			spec_ix = Renderer::GetTextureIndex(material->GetSpecularTexture(), false, &NextBatch);

		// -- Setup Vertex Array & Vertex Attributes --
		constexpr size_t quad_vertex_count = 4;
		for (size_t i = 0; i < quad_vertex_count; ++i)
		{
			QuadVertex& quad_vertex = sprite_component.QuadVertices[i];

			if (pbr)
			{
				SetBaseVertexData(s_Data->PBR_QuadVBufferPtr, quad_vertex, transform, material, tex_ix, norm_ix, entity_id);

				s_Data->PBR_QuadVBufferPtr->Roughness = material->Roughness;
				s_Data->PBR_QuadVBufferPtr->Metallic = material->Metallic;
				s_Data->PBR_QuadVBufferPtr->AmbientOcc = material->AmbientOcclusion;

				s_Data->PBR_QuadVBufferPtr->MetalTexIndex = met_ix;
				s_Data->PBR_QuadVBufferPtr->RoughTexIndex = rough_ix;
				s_Data->PBR_QuadVBufferPtr->AOTexIndex = ao_ix;

				++s_Data->PBR_QuadVBufferPtr;
			}
			else
			{
				SetBaseVertexData(s_Data->NonPBR_QuadVBufferPtr, quad_vertex, transform, material, tex_ix, norm_ix, entity_id);

				s_Data->NonPBR_QuadVBufferPtr->Shininess = material->Smoothness * 256.0f;
				s_Data->NonPBR_QuadVBufferPtr->Specularity = material->Specularity;
				s_Data->NonPBR_QuadVBufferPtr->SpecTexIndex = spec_ix;

				++s_Data->NonPBR_QuadVBufferPtr;
			}
		}

		// -- Update Stats (Quad = 2 Triangles = 6 Indices) --
		s_Data->QuadIndicesDrawCount += 6;
		++s_Data->RendererStats.QuadCount;
	}
}
