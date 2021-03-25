#include "kspch.h"
#include "Renderer2D.h"

#include "Buffer.h"
#include "Resources/Shader.h"
#include "RenderCommand.h"


namespace Kaimos {
	
	// ----------------------- Globals --------------------------------------------------------------------
	struct Renderer2DData
	{
		Renderer2D::Statistics RendererStats;

		static const uint MaxQuads = 20000;
		static const uint MaxVertices = MaxQuads * 4;
		static const uint MaxIndices = MaxQuads * 6;
		static const uint MaxTextureSlots = 32; // TODO: RenderCapabilities - Variables based on what the hardware can do

		uint QuadIndicesDrawCount = 0;
		QuadVertex* QuadVBufferBase = nullptr;
		QuadVertex* QuadVBufferPtr	= nullptr;

		Ref<VertexArray> QuadVArray			= nullptr;
		Ref<VertexBuffer> QuadVBuffer		= nullptr;
		Ref<Shader> ColoredTextureShader	= nullptr;
		Ref<Texture2D> WhiteTexture			= nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint TextureSlotIndex = 1;									// Slot 0 is for White Texture 

		glm::vec4 VerticesPositions[4] = {};						// It's a vec4 so it's easier to multiply by a matrix (4th has to be 1)
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
		s_Data = new Renderer2DData();

		// -- Vertices Positions --
		s_Data->VerticesPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->VerticesPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->VerticesPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->VerticesPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

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
		s_Data->QuadVBufferBase = new QuadVertex[s_Data->MaxVertices];
		s_Data->QuadVArray = VertexArray::Create();
		s_Data->QuadVBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(QuadVertex));

		// -- Vertex Layout & Index Buffer Creation --
		index_buffer = IndexBuffer::Create(quad_indices, s_Data->MaxIndices);
		BufferLayout layout = {
			{ SHADER_DATATYPE::FLOAT3,	"a_Position" },
			{ SHADER_DATATYPE::FLOAT2,	"a_TexCoord" },
			{ SHADER_DATATYPE::FLOAT4,	"a_Color" },
			{ SHADER_DATATYPE::FLOAT ,	"a_TexIndex" },
			{ SHADER_DATATYPE::FLOAT ,	"a_TilingFactor" },
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
		
		// -- Shader Creation --
		s_Data->ColoredTextureShader = Shader::Create("assets/shaders/ColoredTextureShader.glsl");
		
		// -- White Texture Creation --
		uint whiteTextData = 0xffffffff; // Full Fs for every channel there (2x4 channels - rgba -)
		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		s_Data->WhiteTexture->SetData(&whiteTextData, sizeof(whiteTextData)); // or sizeof(uint)

		// -- Texture Slots Filling --
		s_Data->TextureSlots[0] = s_Data->WhiteTexture;
		int texture_samplers[s_Data->MaxTextureSlots];

		for (uint i = 0; i < s_Data->MaxTextureSlots; ++i)
			texture_samplers[i] = i;

		// -- Shader Uniform of Texture Slots --
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUIntArray("u_Textures", texture_samplers, s_Data->MaxTextureSlots);
		s_Data->ColoredTextureShader->Unbind();
	}

	void Renderer2D::Shutdown()
	{
		KS_PROFILE_FUNCTION();

		// This is deleted here (manually), and not treated as smart pointer, waiting for the end of the program lifetime
		// because there is still some code of the graphics (OpenGL) that it has to run to free VRAM (for ex. deleting VArrays, Shaders...)
		delete[] s_Data->QuadVBufferBase;
		delete s_Data;
	}


	
	// ----------------------- Public Renderer Methods ----------------------------------------------------
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		KS_PROFILE_FUNCTION();

		s_Data->QuadVArray->Bind();
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		StartBatch();
	}


	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		KS_PROFILE_FUNCTION();

		s_Data->QuadVArray->Bind();
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUMat4("u_ViewProjection", camera.GetViewProjection());
		StartBatch();
	}


	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& camera_transform)
	{
		KS_PROFILE_FUNCTION();
		glm::mat4 view_proj = camera.GetProjection() * glm::inverse(camera_transform);

		s_Data->QuadVArray->Bind();
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUMat4("u_ViewProjection", view_proj);
		StartBatch();
	}


	void Renderer2D::EndScene()
	{
		KS_PROFILE_FUNCTION();
		Flush();
	}


	void Renderer2D::Flush()
	{
		KS_PROFILE_FUNCTION();

		// -- Check if something to draw --
		if (s_Data->QuadIndicesDrawCount == 0)
			return;

		// -- Cast (uint8_t is 1 byte large, the substraction give us elements in terms of bytes) --
		uint data_size = (uint)((uint8_t*)s_Data->QuadVBufferPtr - (uint8_t*)s_Data->QuadVBufferBase);

		// -- Set Vertex Buffer Data --
		s_Data->QuadVBuffer->SetData(s_Data->QuadVBufferBase, data_size);

		// -- Bind all Textures --
		for (uint i = 0; i < s_Data->TextureSlotIndex; ++i)
			s_Data->TextureSlots[i]->Bind(i);

		// -- Draw Vertex Array --
		RenderCommand::DrawIndexed(s_Data->QuadVArray, s_Data->QuadIndicesDrawCount);
		++s_Data->RendererStats.DrawCalls;
	}


	
	// ----------------------- Private Renderer Methods ---------------------------------------------------
	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}


	void Renderer2D::StartBatch()
	{
		s_Data->QuadIndicesDrawCount = 0;
		s_Data->TextureSlotIndex = 1;
		s_Data->QuadVBufferPtr = s_Data->QuadVBufferBase;
	}
		

	void Renderer2D::SetupVertexArray(const glm::mat4& transform, const glm::vec4& color, int entity_id, float texture_index, float texture_tiling, glm::vec2 texture_uvoffset)
	{
		constexpr size_t quad_vertex_count = 4;
		constexpr glm::vec2 tex_coords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		for (size_t i = 0; i < quad_vertex_count; ++i)
		{
			s_Data->QuadVBufferPtr->Pos = transform * s_Data->VerticesPositions[i];
			s_Data->QuadVBufferPtr->TexCoord = tex_coords[i] - texture_uvoffset;
			s_Data->QuadVBufferPtr->Color = color;
			s_Data->QuadVBufferPtr->TexIndex = texture_index;
			s_Data->QuadVBufferPtr->TilingFactor = texture_tiling;
			s_Data->QuadVBufferPtr->EntityID = entity_id;
			++s_Data->QuadVBufferPtr;
		}

		s_Data->QuadIndicesDrawCount += 6;
		++s_Data->RendererStats.QuadCount;
	}


	
	// ----------------------- Public Drawing Methods -----------------------------------------------------
	void Renderer2D::DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& sprite, int entity_id)
	{
		if (sprite.SpriteTexture)
			DrawQuad(transform, sprite.SpriteTexture, entity_id, sprite.Color, sprite.TextureTiling, sprite.TextureUVOffset);
		else
			DrawQuad(transform, sprite.Color, entity_id);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entity_id)
	{
		KS_PROFILE_FUNCTION();

		// -- New Batch if Needed --
		if (s_Data->QuadIndicesDrawCount >= s_Data->MaxIndices)
			NextBatch();

		// -- Set Vertex Array --
		SetupVertexArray(transform, color, entity_id);
	}


	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D> texture, int entity_id, const glm::vec4& tintColor, float tiling, glm::vec2 texture_uvoffset)
	{
		KS_PROFILE_FUNCTION();

		// -- New Batch if Needed --
		if (s_Data->QuadIndicesDrawCount >= s_Data->MaxIndices)
			NextBatch();

		// -- Texture Index Retrieval --
		uint texture_index = 0;
		if (texture)
		{
			for (uint i = 1; i < s_Data->TextureSlotIndex; ++i)
			{
				if (*s_Data->TextureSlots[i] == *texture)
				{
					texture_index = i;
					break;
				}
			}

			if (texture_index == 0)
			{
				// -- New Batch if Needed --
				if (s_Data->TextureSlotIndex >= s_Data->MaxTextureSlots)
					NextBatch();

				// -- Set Texture --
				texture_index = s_Data->TextureSlotIndex;
				s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
				++s_Data->TextureSlotIndex;
			}
		}

		// -- Set Vertex Array --
		SetupVertexArray(transform, tintColor, entity_id, (float)texture_index, tiling, texture_uvoffset);
	}


	
	// ----------------------- Drawing Methods depending on other drawing methods above -------------------
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color)
	{
		DrawQuad(glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f }), color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2 size, const Ref<Texture2D> texture, float tiling, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tiling, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2 size, const Ref<Texture2D> texture, float tiling, const glm::vec4& tintColor)
	{
		DrawQuad(glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f }), texture, -1, tintColor, tiling);
	}

	// Rotated-Quad Methods (the same than previous, but rotated)
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
							* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1))
							* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const Ref<Texture2D> texture, float tiling, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tiling, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const Ref<Texture2D> texture, float tiling, const glm::vec4& tintColor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
							* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1))
							* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, -1, tintColor, tiling);
	}
}
