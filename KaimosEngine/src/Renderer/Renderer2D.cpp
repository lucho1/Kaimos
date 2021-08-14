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

		Ref<VertexArray> QuadVArray					= nullptr;
		Ref<VertexBuffer> QuadVBuffer				= nullptr;
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
			{ SHADER_DATATYPE::FLOAT ,	"a_AmbientOcclussionValue" },
			{ SHADER_DATATYPE::FLOAT ,	"a_MetalTexIndex" },
			{ SHADER_DATATYPE::FLOAT ,	"a_RoughTexIndex" },
			{ SHADER_DATATYPE::FLOAT ,	"a_AOTexIndex" }
		};

		// -- Vertex Array Filling --
		s_Data->QuadVBuffer->SetLayout(non_pbr_layout);
		s_Data->QuadVArray->AddVertexBuffer(s_Data->QuadVBuffer);
		s_Data->QuadVArray->SetIndexBuffer(index_buffer);

		// -- Arrays Unbinding & Indices Deletion --
		s_Data->QuadVArray->Unbind();
		s_Data->QuadVBuffer->Unbind();
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
		delete s_Data;
	}


	
	// ----------------------- Public Renderer Methods ----------------------------------------------------
	void Renderer2D::BeginScene()
	{
		KS_PROFILE_FUNCTION();
		s_Data->QuadVArray->Bind();
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
		uint data_size = (uint)((uint8_t*)s_Data->NonPBR_QuadVBufferPtr - (uint8_t*)s_Data->NonPBR_QuadVBufferBase);
		s_Data->QuadVBuffer->SetData(s_Data->NonPBR_QuadVBufferBase, data_size);

		// -- Bind Textures & Draw Vertex Array --
		Renderer::BindTextures();
		RenderCommand::DrawIndexed(s_Data->QuadVArray, s_Data->QuadIndicesDrawCount);
		++s_Data->RendererStats.DrawCalls;
	}
	
	void Renderer2D::StartBatch()
	{
		KS_PROFILE_FUNCTION();
		s_Data->QuadIndicesDrawCount = 0;
		s_Data->NonPBR_QuadVBufferPtr = s_Data->NonPBR_QuadVBufferBase;
	}
	
	void Renderer2D::NextBatch()
	{
		KS_PROFILE_FUNCTION();
		Flush();
		StartBatch();
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

		Renderer::CheckMaterialFitsInBatch(material, &NextBatch);

		// -- Get Texture indexex --
		uint texture_index = Renderer::GetTextureIndex(material->GetTexture(), false, &NextBatch);
		uint normal_texture_index = Renderer::GetTextureIndex(material->GetNormalTexture(), true, &NextBatch);
		uint specular_texture_index = Renderer::GetTextureIndex(material->GetSpecularTexture(), false, &NextBatch);

		// -- Update Sprite Timed Vertices --
		static float accumulated_dt = 0.0f;
		accumulated_dt += dt.GetMilliseconds();
		if (accumulated_dt > 200.0f)
		{
			sprite_component.UpdateTimedVertices();
			accumulated_dt = 0.0f;
		}

		// -- Setup Vertex Array & Vertex Attributes --
		constexpr size_t quad_vertex_count = 4;
		for (size_t i = 0; i < quad_vertex_count; ++i)
		{
			QuadVertex& quad_vertex = sprite_component.QuadVertices[i];

			// Set the vertex data
			s_Data->NonPBR_QuadVBufferPtr->Pos = transform * glm::vec4(quad_vertex.Pos, 1.0f);
			s_Data->NonPBR_QuadVBufferPtr->Normal = glm::normalize(glm::vec3(transform * glm::vec4(quad_vertex.Normal, 0.0f)));
			s_Data->NonPBR_QuadVBufferPtr->Tangent = glm::normalize(glm::vec3(transform * glm::vec4(quad_vertex.Tangent, 0.0f)));
			s_Data->NonPBR_QuadVBufferPtr->TexCoord = quad_vertex.TexCoord;

			s_Data->NonPBR_QuadVBufferPtr->Color = material->Color;
			s_Data->NonPBR_QuadVBufferPtr->Shininess = material->Smoothness * 256.0f;
			s_Data->NonPBR_QuadVBufferPtr->Bumpiness = material->Bumpiness;
			s_Data->NonPBR_QuadVBufferPtr->Specularity = material->Specularity;

			s_Data->NonPBR_QuadVBufferPtr->TexIndex = texture_index;
			s_Data->NonPBR_QuadVBufferPtr->NormTexIndex = normal_texture_index;
			s_Data->NonPBR_QuadVBufferPtr->SpecTexIndex = specular_texture_index;
			s_Data->NonPBR_QuadVBufferPtr->EntityID = entity_id;

			++s_Data->NonPBR_QuadVBufferPtr;
		}

		// -- Update Stats (Quad = 2 Triangles = 6 Indices) --
		s_Data->QuadIndicesDrawCount += 6;
		++s_Data->RendererStats.QuadCount;
	}
}
