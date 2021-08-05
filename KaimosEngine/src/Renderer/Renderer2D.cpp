#include "kspch.h"
#include "Renderer2D.h"

#include "Renderer.h"
#include "Foundations/RenderCommand.h"
#include "Resources/Buffer.h"
#include "Resources/Shader.h"
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
		QuadVertex* QuadVBufferBase			= nullptr;
		QuadVertex* QuadVBufferPtr			= nullptr;

		Ref<VertexArray> QuadVArray			= nullptr;
		Ref<VertexBuffer> QuadVBuffer		= nullptr;
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

		s_Data->QuadVBufferBase = new QuadVertex[max_vertices];
		s_Data->QuadVArray = VertexArray::Create();
		s_Data->QuadVBuffer = VertexBuffer::Create(max_vertices * sizeof(QuadVertex));

		// -- Vertex Layout & Index Buffer Creation --
		index_buffer = IndexBuffer::Create(quad_indices, s_Data->MaxIndices);
		BufferLayout layout = {
			{ SHADER_DATATYPE::FLOAT3,	"a_Position" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Normal" },
			{ SHADER_DATATYPE::FLOAT2,	"a_TexCoord" },
			{ SHADER_DATATYPE::FLOAT4,	"a_Color" },
			{ SHADER_DATATYPE::FLOAT ,	"a_TexIndex" },
			{ SHADER_DATATYPE::INT ,	"a_EntityID" }
		};

		// -- Vertex Array Filling --
		s_Data->QuadVBuffer->SetLayout(layout);
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
		delete[] s_Data->QuadVBufferBase;
		delete s_Data;
	}


	
	// ----------------------- Public Renderer Methods ----------------------------------------------------
	void Renderer2D::BeginScene(const CameraComponent& camera_component, const TransformComponent& transform_component)
	{
		KS_PROFILE_FUNCTION();
		glm::mat4 view_proj = camera_component.Camera.GetProjection() * glm::inverse(transform_component.GetTransform());
		SetupRenderingShader(view_proj);
	}
	
	void Renderer2D::BeginScene(const Camera& camera)
	{
		KS_PROFILE_FUNCTION();
		SetupRenderingShader(camera.GetViewProjection());
	}

	void Renderer2D::EndScene()
	{
		KS_PROFILE_FUNCTION();
		Flush();
	}



	// ----------------------- Private Renderer Methods ---------------------------------------------------
	void Renderer2D::SetupRenderingShader(const glm::mat4& vp_matrix)
	{
		Ref<Shader> shader = Renderer::GetShader("BatchedShader");
		if (shader)
		{
			s_Data->QuadVArray->Bind();
			shader->Bind();
			shader->SetUMat4("u_ViewProjection", vp_matrix);
			StartBatch();
		}
		else
			KS_FATAL_ERROR("Tried to Render with a null Shader!");
	}

	void Renderer2D::Flush()
	{
		KS_PROFILE_FUNCTION();

		// -- Check if something to draw --
		if (s_Data->QuadIndicesDrawCount == 0)
			return;

		// -- Set Vertex Buffer Data --
		// Data cast: uint8_t = 1 byte large, subtraction give elements in terms of bytes
		uint data_size = (uint)((uint8_t*)s_Data->QuadVBufferPtr - (uint8_t*)s_Data->QuadVBufferBase);
		s_Data->QuadVBuffer->SetData(s_Data->QuadVBufferBase, data_size);

		// -- Bind Textures & Draw Vertex Array --
		Renderer::BindTextures();
		RenderCommand::DrawIndexed(s_Data->QuadVArray, s_Data->QuadIndicesDrawCount);
		++s_Data->RendererStats.DrawCalls;
	}
	
	void Renderer2D::StartBatch()
	{
		KS_PROFILE_FUNCTION();
		s_Data->QuadIndicesDrawCount = 0;
		s_Data->QuadVBufferPtr = s_Data->QuadVBufferBase;
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

		// -- Get Texture index if Sprite has Texture --
		Ref<Material> material = Renderer::GetMaterial(sprite_component.SpriteMaterialID);
		if (!material)
			KS_FATAL_ERROR("Tried to Render a Sprite with a null Material!");


		uint texture_index = Renderer::GetTextureIndex(material->GetTexture(), &NextBatch);

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
			s_Data->QuadVBufferPtr->Pos = transform * glm::vec4(quad_vertex.Pos, 1.0f);
			s_Data->QuadVBufferPtr->Normal = quad_vertex.Normal;
			s_Data->QuadVBufferPtr->TexCoord = quad_vertex.TexCoord;

			s_Data->QuadVBufferPtr->Color = material->Color;
			s_Data->QuadVBufferPtr->TexIndex = texture_index;
			s_Data->QuadVBufferPtr->EntityID = entity_id;

			++s_Data->QuadVBufferPtr;
		}

		// -- Update Stats (Quad = 2 Triangles = 6 Indices) --
		s_Data->QuadIndicesDrawCount += 6;
		++s_Data->RendererStats.QuadCount;
	}
}
