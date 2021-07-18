#include "kspch.h"
#include "Renderer3D.h"

#include "Core/Resources/ResourceManager.h"
#include "Foundations/RenderCommand.h"
#include "Resources/Buffer.h"
#include "Resources/Shader.h"
#include "Resources/Mesh.h"
#include "Scene/ECS/Components.h"

#include <glm/gtc/type_ptr.hpp>


namespace Kaimos {

	// ----------------------- Globals --------------------------------------------------------------------
	struct Renderer3DData
	{
		Renderer3D::Statistics RendererStats;

		static const uint MaxFaces = 20000;
		static const uint MaxVertices = MaxFaces * 4;
		static const uint MaxIndices = MaxFaces * 6;
		static const uint MaxTextureSlots = 32;						// TODO: RenderCapabilities - Variables based on what the hardware can do

		uint TextureSlotIndex = 1;									// Slot 0 is for White Texture 
		Ref<Texture2D> WhiteTexture = nullptr;						// TODO: Put this in the Renderer, don't have duplicates in Renderer2D & 3D
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;

		uint IndicesDrawCount = 0;
		uint IndicesCurrentOffset = 0;
		Vertex* VBufferBase			= nullptr;
		Vertex* VBufferPtr			= nullptr;
		std::vector<uint> Indices;

		Ref<VertexArray> VArray		= nullptr;
		Ref<IndexBuffer> IBuffer	= nullptr;
		Ref<VertexBuffer> VBuffer	= nullptr;

		Ref<Shader> CurrentShader	= nullptr;
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
		s_3DData = new Renderer3DData();

		// -- Vertex Buffer & Array --
		s_3DData->VBufferBase = new Vertex[s_3DData->MaxVertices];
		s_3DData->VArray = VertexArray::Create();
		s_3DData->VBuffer = VertexBuffer::Create(s_3DData->MaxVertices * sizeof(Vertex));

		// -- Vertex Layout & Index Buffer Creation --
		s_3DData->IBuffer = IndexBuffer::Create(s_3DData->MaxIndices);
		BufferLayout layout = {
			{ SHADER_DATATYPE::FLOAT3,	"a_Position" },
			{ SHADER_DATATYPE::FLOAT3,	"a_Normal" },
			{ SHADER_DATATYPE::FLOAT2,	"a_TexCoord" },
			{ SHADER_DATATYPE::FLOAT4,	"a_Color" },
			{ SHADER_DATATYPE::FLOAT ,	"a_TexIndex" },
			{ SHADER_DATATYPE::INT ,	"a_EntityID" }
		};

		// -- Vertex Array Filling --
		s_3DData->VBuffer->SetLayout(layout);
		s_3DData->VArray->AddVertexBuffer(s_3DData->VBuffer);
		s_3DData->VArray->SetIndexBuffer(s_3DData->IBuffer);

		// -- Arrays Unbinding & Indices Deletion --
		s_3DData->VArray->Unbind();
		s_3DData->VBuffer->Unbind();
		s_3DData->IBuffer->Unbind();

		// -- Shader Creation --
		s_3DData->CurrentShader = Shader::Create("assets/shaders/3DTextureShader.glsl");

		// -- White Texture Creation --
		uint whiteTextData = 0xffffffff; // Full Fs for every channel there (2x4 channels - rgba -)
		s_3DData->WhiteTexture = Texture2D::Create(1, 1);
		s_3DData->WhiteTexture->SetData(&whiteTextData, sizeof(whiteTextData)); // or sizeof(uint)

		// -- Texture Slots Filling --
		s_3DData->TextureSlots[0] = s_3DData->WhiteTexture;
		int texture_samplers[s_3DData->MaxTextureSlots];

		for (uint i = 0; i < s_3DData->MaxTextureSlots; ++i)
			texture_samplers[i] = i;

		// -- Shader Uniform of Texture Slots --
		s_3DData->CurrentShader->Bind();
		s_3DData->CurrentShader->SetUIntArray("u_Textures", texture_samplers, s_3DData->MaxTextureSlots);
		s_3DData->CurrentShader->Unbind();
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
	void Renderer3D::BeginScene(const CameraComponent& camera_component, const TransformComponent& transform_component)
	{
		KS_PROFILE_FUNCTION();
		glm::mat4 view_proj = camera_component.Camera.GetProjection() * glm::inverse(transform_component.GetTransform());

		s_3DData->VArray->Bind();
		s_3DData->CurrentShader->Bind();
		s_3DData->CurrentShader->SetUMat4("u_ViewProjection", view_proj);
		StartBatch();
	}

	void Renderer3D::BeginScene(const Camera& camera)
	{
		KS_PROFILE_FUNCTION();
		s_3DData->VArray->Bind();
		s_3DData->CurrentShader->Bind();
		s_3DData->CurrentShader->SetUMat4("u_ViewProjection", camera.GetViewProjection());
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

		// -- Cast (uint8_t is 1 byte large, the subtraction give us elements in terms of bytes) --
		uint v_data_size = (uint)((uint8_t*)s_3DData->VBufferPtr - (uint8_t*)s_3DData->VBufferBase);

		// -- Set Vertex Buffer Data --
		s_3DData->VBuffer->SetData(s_3DData->VBufferBase, v_data_size);
		s_3DData->IBuffer->SetData(s_3DData->Indices.data(), s_3DData->Indices.size());

		// -- Bind all Textures --
		for (uint i = 0; i < s_3DData->TextureSlotIndex; ++i)
			s_3DData->TextureSlots[i]->Bind(i);

		// -- Draw Vertex Array --
		RenderCommand::DrawIndexed(s_3DData->VArray, s_3DData->IndicesDrawCount);
		++s_3DData->RendererStats.DrawCalls;
	}

	void Renderer3D::StartBatch()
	{
		KS_PROFILE_FUNCTION();
		s_3DData->IndicesDrawCount = 0;
		s_3DData->IndicesCurrentOffset = 0;
		s_3DData->TextureSlotIndex = 1; // 0 is white texture
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
	void Renderer3D::DrawMesh(const glm::mat4& transform, const MeshRendererComponent& mesh_component, int entity_id)
	{
		// -- New Batch if Needed --
		if (s_3DData->IndicesDrawCount >= s_3DData->MaxIndices)
			NextBatch();

		// -- Get Mesh --
		// if mesh_comp.modverts.size != mesh ones
		Ref<Mesh> mesh = Resources::ResourceManager::GetMesh(mesh_component.MeshID);
		if (mesh && mesh->GetVertices().size() == mesh_component.ModifiedVertices.size())
		{
			// -- Get Material --
			Ref<Material> material = Renderer::GetMaterial(mesh_component.MaterialID);
			if (!material)
				KS_FATAL_ERROR("Tried to Render a Mesh with a null Material!");

			// -- Setup Vertex Array & Vertex Attributes --
			uint texture_index = GetTextureIndex(material->GetTexture());
			for (uint i = 0; i < mesh->m_Vertices.size(); ++i)
			{
				glm::vec3 vpos = mesh_component.ModifiedVertices[i].Pos;
				glm::vec3 vnorm = mesh_component.ModifiedVertices[i].Normal;
				glm::vec2 tcoords = mesh_component.ModifiedVertices[i].TexCoord;

				// Set the vertex data
				s_3DData->VBufferPtr->Pos = transform * glm::vec4(vpos, 1.0f);
				s_3DData->VBufferPtr->Normal = vnorm;
				s_3DData->VBufferPtr->TexCoord = tcoords;

				s_3DData->VBufferPtr->Color = material->Color;
				s_3DData->VBufferPtr->TexIndex = texture_index;
				s_3DData->VBufferPtr->EntityID = entity_id;

				++s_3DData->VBufferPtr;
				++s_3DData->RendererStats.VerticesCount;
			}

			// -- Setup Index Buffer --
			std::vector<uint> indices = mesh->m_Indices;
			for (uint i = 0; i < indices.size(); ++i)
			{
				s_3DData->Indices.push_back(s_3DData->IndicesCurrentOffset + indices[i]);				
				++s_3DData->IndicesDrawCount;
				++s_3DData->RendererStats.IndicesCount;
			}

			s_3DData->IndicesCurrentOffset += mesh->m_MaxIndex;
		}
	}
	
		

	// ----------------------- Private Drawing Methods ----------------------------------------------------
	uint Renderer3D::GetTextureIndex(const Ref<Texture2D>& texture)
	{
		uint ret = 0;
		if (texture)
		{
			// -- Find Texture if Exists --
			for (uint i = 1; i < s_3DData->TextureSlotIndex; ++i)
			{
				if (*s_3DData->TextureSlots[i] == *texture)
				{
					ret = i;
					break;
				}
			}

			// -- If it doesn't exists, add it to batch data --
			if (ret == 0)
			{
				// - New Batch if Needed -
				if (s_3DData->TextureSlotIndex >= s_3DData->MaxTextureSlots)
					NextBatch();

				// - Set Texture -
				ret = s_3DData->TextureSlotIndex;
				s_3DData->TextureSlots[s_3DData->TextureSlotIndex] = texture;
				++s_3DData->TextureSlotIndex;
			}
		}

		return ret;
	}
}
