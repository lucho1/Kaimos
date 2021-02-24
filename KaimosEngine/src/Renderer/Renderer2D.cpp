#include "kspch.h"
#include "Renderer2D.h"

#include "Buffer.h"
#include "Resources/Shader.h"
#include "RenderCommand.h"


namespace Kaimos {

	struct LineVertex
	{
		glm::vec4 ClipCoord;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float Width, Length;
	};

	struct QuadVertex
	{
		glm::vec3 Pos;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		float TexIndex;
		float TilingFactor;
		glm::vec2 UVOffsetFactor;

		// -- Editor Variables --
		int EntityID;
	};

	struct Renderer2DData
	{
		Renderer2D::Statistics RendererStats;

		// --- Quads ---
		static const uint MaxQuads = 20000;
		static const uint MaxQuadVertices = MaxQuads * 4;
		static const uint MaxQuadIndices = MaxQuads * 6;

		uint QuadIndicesDrawCount = 0;
		QuadVertex* QuadVBufferBase = nullptr;
		QuadVertex* QuadVBufferPtr = nullptr;

		Ref<VertexArray> QuadVArray;
		Ref<VertexBuffer> QuadVBuffer;
		Ref<Shader> ColoredTextureShader;
		Ref<Texture2D> WhiteTexture;

		// --- Lines ---
		static const uint MaxLines = 20000;
		static const uint MaxLineVertices = MaxLines * 4;
		static const uint MaxLineIndices = MaxLines * 6;

		uint LineIndicesDrawCount = 0;
		LineVertex* LineVBufferBase = nullptr;
		LineVertex* LineVBufferPtr = nullptr;

		Ref<VertexArray> LineVArray;
		Ref<VertexBuffer> LineVBuffer;
		Ref<Shader> LineShader;

		// --- Textures ---
		static const uint MaxTextureSlots = 32; // TODO: RenderCapabilities
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint TextureSlotIndex = 1; // Slot 0 is for White Texture 

		glm::vec4 VerticesPositions[4]; // It's a vec4 so it's easier to multiply by a matrix (4th has to be 1)
	};
	
	static Renderer2DData* s_Data;	// On shutdown, this is deleted, and ~VertexArray() called, freeing GPU Memory too
										// (the whole renderer has to be shutdown while we still have a context, otherwise, it will crash!)

	
	// --- Statistics Methods ---
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

	const uint Renderer2D::GetMaxLines()
	{
		return s_Data->MaxLines;
	}



	// --- Class Methods ---
	void Renderer2D::Init()
	{
		KS_PROFILE_FUNCTION();
		s_Data = new Renderer2DData();

		// --- Vertices Positioning ---
		s_Data->VerticesPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->VerticesPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->VerticesPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->VerticesPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };


		// --- Vertex Array & Buffers ---
		//Ref<VertexBuffer> m_VBuffer;
		//uint indices[6] = { 0, 1, 2, 2, 3, 0 };

		//float vertices[5 * 4] = {
		//		-0.5f,	-0.5f,	0.0f, 0.0f, 0.0f,		// For negative X positions, UV should be 0, for positive, 1
		//		 0.5f,	-0.5f,	0.0f, 1.0f, 0.0f,		// If you render, on a square, the texCoords (as color = vec4(tC, 0, 1)), the colors of the square in its corners are
		//		 0.5f,	 0.5f,	0.0f, 1.0f, 1.0f,		// (0,0,0,1) - Black, (1,0,0,1) - Red, (1,1,0,0) - Yellow, (0,1,0,1) - Green
		//		-0.5f,	 0.5f,	0.0f, 0.0f, 1.0f
		//};

		// --- Quads ---
		Ref<IndexBuffer> m_QuadIBuffer;
		uint* quadIndices = new uint[s_Data->MaxQuadIndices];

		uint q_offset = 0;
		for (uint i = 0; i < s_Data->MaxQuadIndices; i += 6)
		{
			quadIndices[i + 0] = q_offset + 0;
			quadIndices[i + 1] = q_offset + 1;
			quadIndices[i + 2] = q_offset + 2;

			quadIndices[i + 3] = q_offset + 2;
			quadIndices[i + 4] = q_offset + 3;
			quadIndices[i + 5] = q_offset + 0;

			q_offset += 4;
		}

		//m_VBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		//m_IBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint));
		
		s_Data->QuadVBufferBase = new QuadVertex[s_Data->MaxQuadVertices];
		s_Data->QuadVArray = VertexArray::Create();
		s_Data->QuadVBuffer = VertexBuffer::Create(s_Data->MaxQuadVertices * sizeof(QuadVertex));
		m_QuadIBuffer = IndexBuffer::Create(quadIndices, s_Data->MaxQuadIndices);

		BufferLayout q_layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float , "a_TexIndex" },
			{ ShaderDataType::Float , "a_TilingFactor" },
			{ ShaderDataType::Float2, "a_UVOffsetFactor" },
			{ ShaderDataType::Int ,	  "a_EntityID" }
		};

		//m_VBuffer->SetLayout(layout);
		//s_Data->QuadVArray->AddVertexBuffer(m_VBuffer);
		s_Data->QuadVBuffer->SetLayout(q_layout);
		s_Data->QuadVArray->AddVertexBuffer(s_Data->QuadVBuffer);
		s_Data->QuadVArray->SetIndexBuffer(m_QuadIBuffer);

		s_Data->QuadVArray->Unbind();
		s_Data->QuadVBuffer->Unbind();
		m_QuadIBuffer->Unbind();
		delete[] quadIndices;

		// --- Lines ---
		s_Data->LineVArray = VertexArray::Create();
		s_Data->LineVBuffer = VertexBuffer::Create(s_Data->MaxLineVertices * sizeof(LineVertex));

		BufferLayout l_layout = {
			{ ShaderDataType::Float4, "a_ClipCoord" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float , "a_Width" },
			{ ShaderDataType::Float , "a_Length" },
		};

		s_Data->LineVBuffer->SetLayout(l_layout);
		s_Data->LineVArray->AddVertexBuffer(s_Data->LineVBuffer);
		s_Data->LineVBufferBase = new LineVertex[s_Data->MaxLineVertices];

		Ref<IndexBuffer> m_LineIBuffer;
		uint* lineIndices = new uint[s_Data->MaxLineIndices];

		uint l_offset = 0;
		for (uint i = 0; i < s_Data->MaxLineIndices; i += 6)
		{
			lineIndices[i + 0] = l_offset + 0;
			lineIndices[i + 1] = l_offset + 1;
			lineIndices[i + 2] = l_offset + 2;

			lineIndices[i + 3] = l_offset + 2;
			lineIndices[i + 4] = l_offset + 3;
			lineIndices[i + 5] = l_offset + 0;

			l_offset += 4;
		}

		m_LineIBuffer = IndexBuffer::Create(lineIndices, s_Data->MaxLineIndices);
		s_Data->LineVArray->SetIndexBuffer(m_LineIBuffer);

		s_Data->LineVArray->Unbind();
		s_Data->LineVBuffer->Unbind();
		m_LineIBuffer->Unbind();
		delete[] lineIndices;
		
		// --- Shaders ---
		s_Data->ColoredTextureShader = Shader::Create("assets/shaders/ColoredTextureShader.glsl");
		s_Data->LineShader = Shader::Create("assets/shaders/LineShader.glsl");
		
		// --- Textures ---
		uint whiteTextData = 0xffffffff; // Full Fs for every channel there (2x4 channels - rgba -)
		s_Data->WhiteTexture = Texture2D::Create(1, 1);
		s_Data->WhiteTexture->SetData(&whiteTextData, sizeof(whiteTextData)); // or sizeof(uint)

		// Set 1st TextureSlot to 0 --> Could be also done with a memset or with s_Data->TextureSlots.fill(0);
		//for (uint i = 0; i < s_Data->TextureSlots.size(); ++i)
		//	s_Data->TextureSlots[i] = 0;
		s_Data->TextureSlots[0] = s_Data->WhiteTexture;
		int texture_samplers[s_Data->MaxTextureSlots];

		for (uint i = 0; i < s_Data->MaxTextureSlots; ++i)
			texture_samplers[i] = i;

		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUIntArray("u_Textures", texture_samplers, s_Data->MaxTextureSlots);
		s_Data->ColoredTextureShader->Unbind();

		s_Data->LineShader->Bind();
		s_Data->LineShader->SetUInt("u_Caps", 1); // 0 = NONE, 1 = SQUARE, 2 = ROUND, 3 = TRIANGLE
		s_Data->LineShader->Unbind();
	}

	void Renderer2D::Shutdown()
	{
		KS_PROFILE_FUNCTION();

		// This is deleted here (manually), and not treated as smart pointer, waiting for the end of the program lifetime
		// because there is still some code of the graphics (OpenGL) that it has to run to free VRAM (for ex. deleting VArrays, Shaders...)
		delete[] s_Data->QuadVBufferBase;
		delete[] s_Data->LineVBufferBase;
		delete s_Data;
	}



	// --- Rendering Methods ---
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		KS_PROFILE_FUNCTION();

		// TODO: This should not be made here, but when VArrays has something to draw!!
		s_Data->QuadVArray->Bind();
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		KS_PROFILE_FUNCTION();

		// TODO: This should not be made here, but when VArrays has something to draw!!
		s_Data->QuadVArray->Bind();
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUMat4("u_ViewProjection", camera.GetViewProj());
		StartBatch();
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& camera_transform)
	{
		KS_PROFILE_FUNCTION();
		glm::mat4 ViewProj = camera.GetProjection() * glm::inverse(camera_transform);

		// TODO: This should not be made here, but when VArrays has something to draw!!
		s_Data->QuadVArray->Bind();
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUMat4("u_ViewProjection", ViewProj);
		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		KS_PROFILE_FUNCTION();
		QuadsFlush();
		LinesFlush();
	}

	void Renderer2D::NextBatch()
	{
		QuadsFlush();
		LinesFlush();
		StartBatch();
	}

	void Renderer2D::StartBatch()
	{
		s_Data->TextureSlotIndex = 1;
		s_Data->QuadIndicesDrawCount = 0;
		s_Data->QuadVBufferPtr = s_Data->QuadVBufferBase;

		s_Data->LineIndicesDrawCount = 0;
		s_Data->LineVBufferPtr = s_Data->LineVBufferBase;
	}

	void Renderer2D::LinesFlush()
	{
		KS_PROFILE_FUNCTION();
		if (s_Data->LineIndicesDrawCount == 0)
			return;

		uint linesDataSize = (uint)((uint8_t*)s_Data->LineVBufferPtr - (uint8_t*)s_Data->LineVBufferBase);
		s_Data->LineVBuffer->SetData(s_Data->LineVBufferBase, linesDataSize);

		s_Data->LineVArray->Bind();
		s_Data->LineShader->Bind();
		//s_Data->LineShader->SetUMat4("u_ViewProjection", glm::mat4(1.0f));

		RenderCommand::DrawIndexed(s_Data->LineVArray, s_Data->LineIndicesDrawCount);
		++s_Data->RendererStats.LineDrawCalls;
	}

	void Renderer2D::QuadsFlush()
	{
		KS_PROFILE_FUNCTION();

		// Check if something to draw
		if (s_Data->QuadIndicesDrawCount == 0)
			return;

		// This cast is because uint8_t is 1 byte large, so the substraction give us the elements in terms of bytes
		uint dataSize = (uint)((uint8_t*)s_Data->QuadVBufferPtr - (uint8_t*)s_Data->QuadVBufferBase);
		s_Data->QuadVBuffer->SetData(s_Data->QuadVBufferBase, dataSize);

		// Bind all Textures to bind
		for (uint i = 0; i < s_Data->TextureSlotIndex; ++i)
			s_Data->TextureSlots[i]->Bind(i);

		// Draw VArray
		RenderCommand::DrawIndexed(s_Data->QuadVArray, s_Data->QuadIndicesDrawCount);
		++s_Data->RendererStats.QuadDrawCalls;
	}

	void Renderer2D::SetupVertexArray(const glm::mat4& transform, const glm::vec4& color, int entity_id, float texture_index, float texture_tiling, glm::vec2 texture_uvoffset)
	{
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 texCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

		for (size_t i = 0; i < quadVertexCount; ++i)
		{
			s_Data->QuadVBufferPtr->Pos = transform * s_Data->VerticesPositions[i];
			s_Data->QuadVBufferPtr->TexCoord = texCoords[i];
			s_Data->QuadVBufferPtr->Color = color;
			s_Data->QuadVBufferPtr->TexIndex = texture_index;
			s_Data->QuadVBufferPtr->TilingFactor = texture_tiling;
			s_Data->QuadVBufferPtr->UVOffsetFactor = texture_uvoffset;
			s_Data->QuadVBufferPtr->EntityID = entity_id;
			++s_Data->QuadVBufferPtr;
		}

		s_Data->QuadIndicesDrawCount += 6;
		++s_Data->RendererStats.QuadCount;
	}



	// --- Drawing Methods ---
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

		if (s_Data->QuadIndicesDrawCount >= s_Data->MaxQuadIndices)
			NextBatch();

		// Vertex Buffer setup
		SetupVertexArray(transform, color, entity_id);
	}


	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D> texture, int entity_id, const glm::vec4& tintColor, float tiling, glm::vec2 texture_uvoffset)
	{
		KS_PROFILE_FUNCTION();

		if (s_Data->QuadIndicesDrawCount >= s_Data->MaxQuadIndices)
			NextBatch();

		// Texture Index retrieval
		uint textureIndex = 0;
		if (texture)
		{
			for (uint i = 1; i < s_Data->TextureSlotIndex; ++i)
			{
				if (*s_Data->TextureSlots[i] == *texture)
				{
					textureIndex = i;
					break;
				}
			}

			if (textureIndex == 0)
			{
				if (s_Data->TextureSlotIndex >= s_Data->MaxTextureSlots)
					NextBatch();

				textureIndex = s_Data->TextureSlotIndex;
				s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
				++s_Data->TextureSlotIndex;
			}
		}

		// Vertex Buffer setup
		SetupVertexArray(transform, tintColor, entity_id, (float)textureIndex, tiling, texture_uvoffset);
	}


	void Renderer2D::DrawLine(const glm::mat4& viewproj_mat, const glm::vec3& position, const glm::vec3& destination, float width, const glm::vec4& color)
	{
		if (s_Data->LineIndicesDrawCount >= s_Data->MaxLineIndices)
			NextBatch();

		uint screen_width = 1280, screen_height = 720;
		glm::mat4 projMatrix = viewproj_mat;

		// World Coordinates to Clip-Space Coordinates
		glm::vec4 clip_pos = projMatrix * glm::vec4(position, 1.0f);
		glm::vec4 clip_des = projMatrix * glm::vec4(destination * 2.0f, 1.0f);

		// Clip-Space Coordinates to Pixel/Screen Coordinates
		glm::vec2 pixel_pos;

		float clip_posX = clip_pos.x / clip_pos.w;
		float clip_posY = clip_pos.y / clip_pos.w;

		if (glm::isinf(clip_posX) || glm::isnan(clip_posX))
			clip_posX = 0.0f;

		if (glm::isinf(clip_posY) || glm::isnan(clip_posY))
			clip_posY = 0.0f;

		pixel_pos.x = 0.5f * (float)screen_width * (clip_posX + 1.0f);
		pixel_pos.y = 0.5f * (float)screen_height * (1.0f - clip_posY);

		glm::vec2 pixel_des;

		float clip_desX = clip_des.x / clip_des.w;
		float clip_desY = clip_des.y / clip_des.w;

		if (glm::isinf(clip_desX) || glm::isnan(clip_desX))
			clip_desX = 0.0f;

		if (glm::isinf(clip_desY) || glm::isnan(clip_desY))
			clip_desY = 0.0f;

		pixel_des.x = 0.5f * (float)screen_width * (clip_desX + 1.0f);
		pixel_des.y = 0.5f * (float)screen_height * (1.0f - clip_desY);

		// Line Direction
		glm::vec2 dir = pixel_des - pixel_pos;
		float line_length = glm::length(dir);
		if (line_length < 0.00001f)
			return;

		dir /= line_length;
		glm::vec2 normal = { -dir.y, +dir.x };
		float d = 0.5f * width;

		float dOverWidth = d / screen_width;
		float dOverHeight = d / screen_height;

		glm::vec4 offset(0.0f);
		offset.x = (-dir.x + normal.x) * dOverWidth;
		offset.y = (+dir.y - normal.y) * dOverHeight;
		s_Data->LineVBufferPtr->ClipCoord = clip_pos + offset;
		s_Data->LineVBufferPtr->Color = color;
		s_Data->LineVBufferPtr->TexCoord = { -d, +d };
		s_Data->LineVBufferPtr->Width = 2 * d;
		s_Data->LineVBufferPtr->Length = line_length;
		s_Data->LineVBufferPtr++;

		offset.x = (+dir.x + normal.x) * dOverWidth;
		offset.y = (-dir.y - normal.y) * dOverHeight;
		s_Data->LineVBufferPtr->ClipCoord = clip_des + offset;
		s_Data->LineVBufferPtr->Color = color;
		s_Data->LineVBufferPtr->TexCoord = { line_length + d, +d };
		s_Data->LineVBufferPtr->Width = 2 * d;
		s_Data->LineVBufferPtr->Length = line_length;
		s_Data->LineVBufferPtr++;

		offset.x = (+dir.x - normal.x) * dOverWidth;
		offset.y = (-dir.y + normal.y) * dOverHeight;
		s_Data->LineVBufferPtr->ClipCoord = clip_des + offset;
		s_Data->LineVBufferPtr->Color = color;
		s_Data->LineVBufferPtr->TexCoord = { line_length + d, -d };
		s_Data->LineVBufferPtr->Width = 2 * d;
		s_Data->LineVBufferPtr->Length = line_length;
		s_Data->LineVBufferPtr++;

		offset.x = (-dir.x - normal.x) * dOverWidth;
		offset.y = (+dir.y + normal.y) * dOverHeight;
		s_Data->LineVBufferPtr->ClipCoord = clip_pos + offset;
		s_Data->LineVBufferPtr->Color = color;
		s_Data->LineVBufferPtr->TexCoord = { -d, -d };
		s_Data->LineVBufferPtr->Width = 2 * d;
		s_Data->LineVBufferPtr->Length = line_length;
		s_Data->LineVBufferPtr++;

		s_Data->LineIndicesDrawCount += 6;
		s_Data->RendererStats.LineCount++;
	}



	// --- Drawing Methods depending on other drawing methods above ---
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
