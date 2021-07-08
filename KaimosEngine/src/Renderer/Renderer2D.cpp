#include "kspch.h"
#include "Renderer2D.h"

#include "Foundations/RenderCommand.h"
#include "Resources/Buffer.h"
#include "Resources/Shader.h"

#include <glm/gtc/type_ptr.hpp>


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
		QuadVertex* QuadVBufferBase			= nullptr;
		QuadVertex* QuadVBufferPtr			= nullptr;

		Ref<VertexArray> QuadVArray			= nullptr;
		Ref<VertexBuffer> QuadVBuffer		= nullptr;
		Ref<Shader> ColoredTextureShader	= nullptr;
		Ref<Texture2D> WhiteTexture			= nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint TextureSlotIndex = 1;									// Slot 0 is for White Texture 

		std::vector<QuadVertex> VerticesVector = std::vector<QuadVertex>(4);
	};
	
	static Renderer2DData* s_Data = nullptr;	// On shutdown, this is deleted, and ~VertexArray() called, freeing GPU Memory too
												// (the whole renderer has to be shutdown while we still have a context, otherwise, it will crash!)

	
	// ----------------------- Renderer Statistics Methods ------------------------------------------------
	void Renderer2D::ResetStats()
	{
		memset(&s_Data->RendererStats, 0, sizeof(Statistics));
	}

	void Renderer2D::IncrementIndicesDrawn(uint indices_increment)
	{
		s_Data->QuadIndicesDrawCount += indices_increment;
		s_Data->RendererStats.IndicesCount += indices_increment;
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

		// -- Vertices Positions & TCoords Definition --
		s_Data->VerticesVector[0].Pos = { -0.5f, -0.5f, 0.0f };
		s_Data->VerticesVector[1].Pos = {  0.5f, -0.5f, 0.0f };
		s_Data->VerticesVector[2].Pos = {  0.5f,  0.5f, 0.0f };
		s_Data->VerticesVector[3].Pos = { -0.5f,  0.5f, 0.0f };

		s_Data->VerticesVector[0].Normal = { 0.0f,  0.0f, -1.0f };
		s_Data->VerticesVector[1].Normal = { 0.0f,  0.0f, -1.0f };
		s_Data->VerticesVector[2].Normal = { 0.0f,  0.0f, -1.0f };
		s_Data->VerticesVector[3].Normal = { 0.0f,  0.0f, -1.0f };

		s_Data->VerticesVector[0].TexCoord = { 0.0f, 0.0f };
		s_Data->VerticesVector[1].TexCoord = { 1.0f, 0.0f};
		s_Data->VerticesVector[2].TexCoord = { 1.0f, 1.0f};
		s_Data->VerticesVector[3].TexCoord = { 0.0f, 1.0f};

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
			{ SHADER_DATATYPE::FLOAT3,	"a_Normal" },
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
	void Renderer2D::BeginScene(const CameraComponent& camera_component, const TransformComponent& transform_component)
	{
		KS_PROFILE_FUNCTION();
		glm::mat4 view_proj = camera_component.Camera.GetProjection() * glm::inverse(transform_component.GetTransform());

		s_Data->QuadVArray->Bind();
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUMat4("u_ViewProjection", view_proj);
		StartBatch();
	}
	
	void Renderer2D::BeginScene(const Camera& camera)
	{
		KS_PROFILE_FUNCTION();
		s_Data->QuadVArray->Bind();
		s_Data->ColoredTextureShader->Bind();
		s_Data->ColoredTextureShader->SetUMat4("u_ViewProjection", camera.GetViewProjection());
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

		// -- Cast (uint8_t is 1 byte large, the subtraction give us elements in terms of bytes) --
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
	void Renderer2D::StartBatch()
	{
		KS_PROFILE_FUNCTION();
		s_Data->QuadIndicesDrawCount = 0;
		s_Data->TextureSlotIndex = 1; // 0 is white texture
		s_Data->QuadVBufferPtr = s_Data->QuadVBufferBase;
	}		
	
	void Renderer2D::NextBatch()
	{
		KS_PROFILE_FUNCTION();
		Flush();
		StartBatch();
	}


	
	// ----------------------- Drawing Methods ------------------------------------------------------------
	void Renderer2D::DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& sprite, int entity_id)
	{
		// -- Get Texture index if Sprite has Texture --
		Ref<Material> material = Renderer::GetMaterial(sprite.SpriteMaterialID);
		if (!material)
			KS_ENGINE_ASSERT(false, "Tried to Render with a null Material!")

		uint texture_index = GetTextureIndex(material->GetTexture());

		// -- New Batch if Needed --
		if (s_Data->QuadIndicesDrawCount >= s_Data->MaxIndices)
			NextBatch();

		// -- Setup Vertex Array & Vertex Attributes --
		SetupVertexBuffer(transform, entity_id, material, texture_index, s_Data->VerticesVector);
		IncrementIndicesDrawn(6); // A quad has 6 indices (2 triangles)
		s_Data->QuadIndicesDrawCount += 6;
		++s_Data->RendererStats.QuadCount;
	}

	uint Renderer2D::GetTextureIndex(const Ref<Texture2D>& texture)
	{
		uint ret = 0;
		if (texture)
		{
			// -- Find Texture if Exists --
			for (uint i = 1; i < s_Data->TextureSlotIndex; ++i)
			{
				if (*s_Data->TextureSlots[i] == *texture)
				{
					ret = i;
					break;
				}
			}

			// -- If it doesn't exists, add it to batch data --
			if (ret == 0)
			{
				// - New Batch if Needed -
				if (s_Data->TextureSlotIndex >= s_Data->MaxTextureSlots)
					NextBatch();

				// - Set Texture -
				ret = s_Data->TextureSlotIndex;
				s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
				++s_Data->TextureSlotIndex;
			}
		}

		return ret;
	}

	void Renderer2D::SetupVertexBuffer(const glm::mat4& transform, int entity_id, const Ref<Material>& material, uint texture_index, std::vector<QuadVertex> vertices_vector)
	{
		for (size_t i = 0; i < vertices_vector.size(); ++i)
		{
			// Update the Nodes with vertex parameters on each vertex
			material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::POSITION, glm::value_ptr(vertices_vector[i].Pos));
			material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::NORMAL, glm::value_ptr(vertices_vector[i].Normal));
			material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::TEX_COORDS, glm::value_ptr(vertices_vector[i].TexCoord));

			// Get the vertex parameters in the main node once updated the nodes
			glm::vec3 vpos = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::POSITION);
			glm::vec3 vnorm = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::NORMAL);
			glm::vec2 tcoords = material->GetVertexAttributeResult<glm::vec2>(MaterialEditor::VertexParameterNodeType::TEX_COORDS);

			// Set the vertex data
			s_Data->QuadVBufferPtr->Pos = transform * glm::vec4(vpos, 1.0f);
			s_Data->QuadVBufferPtr->Normal = vnorm;
			s_Data->QuadVBufferPtr->TexCoord = tcoords - material->TextureUVOffset;

			s_Data->QuadVBufferPtr->Color = material->Color;
			s_Data->QuadVBufferPtr->TexIndex = texture_index;
			s_Data->QuadVBufferPtr->TilingFactor = material->TextureTiling;
			s_Data->QuadVBufferPtr->EntityID = entity_id;

			++s_Data->QuadVBufferPtr;
			++s_Data->RendererStats.VerticesCount;
		}
	}
}
