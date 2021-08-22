#include "kspch.h"
#include "Renderer.h"

#include "OpenGL/Resources/OGLShader.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/Shader.h"
#include "Resources/Texture.h"
#include "Resources/Light.h"

#include "Renderer2D.h"
#include "Renderer3D.h"

#include <yaml-cpp/yaml.h>


namespace Kaimos {

	struct RendererData
	{
		// Renderer Stuff
		glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);
		glm::vec3 SceneColor = glm::vec3(1.0f);
		const uint MaxDirLights = 10, MaxPointLights = 100;
		
		// Shaders & Materials
		ShaderLibrary Shaders;
		uint DefaultMaterialID = 0;
		std::unordered_map<uint, Ref<Material>> Materials;

		// Textures
		uint TextureSlotIndex = 2;									// Slot 0 -> White Texture, Slot 1 -> Normal Texture
		static const uint MaxTextureSlots = 32;						// TODO: RenderCapabilities - Variables based on what the hardware can do
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		Ref<Texture2D> WhiteTexture = nullptr, NormalTexture = nullptr;
	};

	static RendererData* s_RendererData = nullptr;


	// ----------------------- Public Class Methods -------------------------------------------------------
	void Renderer::CreateRenderer()
	{
		KS_PROFILE_FUNCTION();
		KS_INFO("\n\n--- CREATING KAIMOS RENDERER ---");
		s_RendererData = new RendererData();

		// -- Default Textures Creation --
		uint white_data = 0xffffffff; // Full Fs for every channel there (2x4 channels - rgba -)
		s_RendererData->WhiteTexture = Texture2D::Create(1, 1);
		s_RendererData->WhiteTexture->SetData(&white_data, sizeof(white_data)); // or sizeof(uint)

		uint normal_data = 0xffff8080;
		s_RendererData->NormalTexture = Texture2D::Create(1, 1);
		s_RendererData->NormalTexture->SetData(&normal_data, sizeof(normal_data)); // or sizeof(uint)

		// -- Texture Slots Filling --
		s_RendererData->TextureSlots[0] = s_RendererData->WhiteTexture;
		s_RendererData->TextureSlots[1] = s_RendererData->NormalTexture;
		int texture_samplers[s_RendererData->MaxTextureSlots];

		for (uint i = 0; i < s_RendererData->MaxTextureSlots; ++i)
			texture_samplers[i] = i;

		// -- Shaders Creation --
		s_RendererData->Shaders.Load("BatchedShader", "assets/shaders/BatchRenderingShader.glsl");

		// -- Shaders Uniform of Texture Slots --
		s_RendererData->Shaders.ForEachShader([&](const Ref<Shader>& shader)
			{
				shader->Bind();
				shader->SetUniformIntArray("u_Textures", texture_samplers, s_RendererData->MaxTextureSlots);
				shader->Unbind();
			});
	}

	void Renderer::Init()
	{
		KS_PROFILE_FUNCTION();
		KS_INFO("\n\n--- INITIALIZING KAIMOS RENDERER ---");
		
		// -- Default Material Creation --
		CreateDefaultMaterial(); // Make sure we create it (in case we didn't deserialized)

		// -- Renderer Initializations --
		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();
	}

	void Renderer::Shutdown()
	{
		KS_INFO("\n\n--- SHUTTING DOWN KAIMOS RENDERER ---");
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();

		// Not sure if this is necessary since maybe delete s_RendererData is enough
		for (auto& mat : s_RendererData->Materials)
			mat.second.reset();

		for (auto& texture : s_RendererData->TextureSlots)
			texture.reset();
		
		s_RendererData->Materials.clear();
		s_RendererData->WhiteTexture.reset();
		s_RendererData->NormalTexture.reset();
		delete s_RendererData;
	}



	// ----------------------- Public Renderer Methods -------------------------------------------------------
	// Takes all scene parameters & makes sure shaders we use get the right uniforms
	void Renderer::BeginScene(const Camera& camera)
	{
		s_RendererData->ViewProjectionMatrix = camera.GetViewProjection();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertex_array, const glm::mat4& transformation)
	{
		shader->Bind();
		shader->SetUniformMat4("u_ViewProjection", s_RendererData->ViewProjectionMatrix);
		shader->SetUniformMat4("u_Model", transformation);

		// -- Vertex Array bound here since RenderCommands should NOT do multiple things, they are just commands (unless specifically supposed-to) --
		vertex_array->Bind();
		RenderCommand::DrawIndexed(vertex_array);
		//vertexArray->Unbind(); //TODO: ?
	}



	// ----------------------- Public Renderer Serialization Methods -----------------------------------------
	void Renderer::SerializeRenderer()
	{
		KS_PROFILE_FUNCTION();
		KS_TRACE("Serializing Kaimos Renderer");

		// -- Begin Renderer Map --
		YAML::Emitter output;
		output << YAML::BeginMap;
		output << YAML::Key << "KaimosSaveFile" << YAML::Value << "KaimosRenderer";
		
		// -- Serialize Materials (as Sequence) --
		output << YAML::Key << "DefaultMaterialID" << YAML::Value << s_RendererData->DefaultMaterialID;
		output << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;
		for (auto& mat : s_RendererData->Materials)
		{
			output << YAML::BeginMap;
			output << YAML::Key << "Material" << YAML::Value << mat.second->GetID();
			output << YAML::Key << "Name" << YAML::Value << mat.second->GetName();
			output << YAML::Key << "AttachedGraph";
			mat.second->m_AttachedGraph->SerializeGraph(output);
			output << YAML::EndMap;
		}

		// -- End Materials Sequence & Renderer Map --
		output << YAML::EndSeq;
		output << YAML::EndMap;

		// -- Save File --
		std::string filepath = INTERNAL_SETTINGS_PATH + std::string("KaimosRendererSettings.kaimossave");
		std::ofstream file(filepath.c_str());
		file << output.c_str();
	}

	void Renderer::DeserializeRenderer()
	{
		KS_PROFILE_FUNCTION();
		KS_TRACE("Deserializing Kaimos Renderer");

		// -- File Load --
		YAML::Node data;

		std::string filename = INTERNAL_SETTINGS_PATH + std::string("KaimosRendererSettings.kaimossave");
		std::ifstream f(filename.c_str());

		if (!f.good())
		{
			KS_WARN("Couldn't Deserialize Renderer, invalid or non-existent filepath (if it's the first time, the file might not exist yet)");
			return;
		}

		try { data = YAML::LoadFile(filename); }
		catch (const YAML::ParserException& exception)
		{
			KS_ERROR("Error Loading Renderer\nError: {0}", exception.what());
			return;
		}

		if (!data["KaimosSaveFile"])
		{
			KS_ERROR("Error Deserializing Renderer\nError: Wrong File (no 'KaimosSaveFile' node within save file)");
			return;
		}

		// -- Setup --
		if (data["DefaultMaterialID"])
			CreateDefaultMaterial(data["DefaultMaterialID"].as<uint>());
		else
			CreateDefaultMaterial();

		YAML::Node materials_node = data["Materials"];
		if (materials_node)
		{
			for (auto material_subnode : materials_node)
			{
				auto graph_subnode = material_subnode["AttachedGraph"];
				if (material_subnode["Material"] && graph_subnode)
				{
					// Get or Create Material with ID
					uint mat_id = material_subnode["Material"].as<uint>();
					Ref<Material> material = GetMaterial(mat_id);
					if (!material)
					{
						const std::string mat_name = material_subnode["Name"] ? material_subnode["Name"].as<std::string>() : "MAT_NONAME_ONLOAD";
						material = CreateMaterialWithID(mat_id, mat_name);
					}

					// Remove the Graph (if exists)
					material->RemoveGraph();

					// Create a graph with ID and call him to deseralize passing file
					uint graph_id = graph_subnode["MaterialGraph"].as<uint>();
					ScopePtr<MaterialEditor::MaterialGraph> mat_graph = CreateScopePtr<MaterialEditor::MaterialGraph>(new MaterialEditor::MaterialGraph(graph_id));

					std::string mat_texture_file, mat_normtexture_file, mat_spectexture_file;
					mat_graph->DeserializeGraph(graph_subnode, material, mat_texture_file, mat_normtexture_file, mat_spectexture_file);

					// Finally, assign graph & texture to material
					material->SetGraphUniqueRef(mat_graph);
					if(!mat_texture_file.empty())
						material->SetTexture(mat_texture_file);
					if (!mat_normtexture_file.empty())
						material->SetNormalTexture(mat_normtexture_file);
					if (!mat_spectexture_file.empty())
						material->SetSpecularTexture(mat_spectexture_file);
				}
			}
		}
	}



	// ----------------------- Event Methods -----------------------------------------------------------------
	void Renderer::OnWindowResize(uint width, uint height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}


	
	// ----------------------- Public Getters & Renderer Shaders Methods -------------------------------------
	uint Renderer::GetMaxTextureSlots()
	{
		return s_RendererData->MaxTextureSlots;
	}

	uint Renderer::GetCurrentTextureSlot()
	{
		return s_RendererData->TextureSlotIndex;
	}

	const glm::vec3 Renderer::GetSceneColor()
	{
		return s_RendererData->SceneColor;
	}

	void Renderer::SetSceneColor(const glm::vec3& color)
	{
		s_RendererData->SceneColor = color;
	}

	const uint Renderer::GetMaxDirLights()
	{
		return s_RendererData->MaxDirLights;
	}

	const uint Renderer::GetMaxPointLights()
	{
		return s_RendererData->MaxPointLights;
	}

	Ref<Shader> Renderer::GetShader(const std::string& name)
	{
		if (s_RendererData->Shaders.Exists(name))
			return s_RendererData->Shaders.Get(name);

		return nullptr;
	}



	// ----------------------- Public Renderer Materials Methods ---------------------------------------------
	void Renderer::ResetTextureSlotIndex()
	{
		s_RendererData->TextureSlotIndex = 2; // 0 is white texture, 1 is normal texture
	}

	void Renderer::BindTextures()
	{
		for (uint i = 0; i < s_RendererData->TextureSlotIndex; ++i)
			s_RendererData->TextureSlots[i]->Bind(i);
	}

	void Renderer::CheckMaterialFitsInBatch(const Ref<Material>& material, std::function<void()> NextBatchFunction)
	{
		bool start_new_batch = false;

		if (s_RendererData->TextureSlotIndex == (s_RendererData->MaxTextureSlots - 2))
			if (material->HasAlbedo() && material->HasNormal() && material->HasSpecular())
				start_new_batch = true;
		else if (s_RendererData->TextureSlotIndex == (s_RendererData->MaxTextureSlots - 1))
			if ((material->HasAlbedo() && material->HasNormal()) || (material->HasAlbedo() && material->HasSpecular()) || (material->HasNormal() && material->HasSpecular()))
				start_new_batch = true;

		if (start_new_batch)
		{
			NextBatchFunction();
			s_RendererData->TextureSlotIndex = 2; // 0 is white texture, 1 is normal texture
		}
	}

	uint Renderer::GetTextureIndex(const Ref<Texture2D>& texture, bool is_normal, std::function<void()> NextBatchFunction)
	{
		uint ret = is_normal ? 1 : 0;
		if (texture)
		{
			// -- Find Texture if Exists --
			for (uint i = 1; i < s_RendererData->TextureSlotIndex; ++i)
			{
				if (*s_RendererData->TextureSlots[i] == *texture)
				{
					ret = i;
					break;
				}
			}

			// -- If it doesn't exists, add it to batch data --
			if (ret == 0 || ret == 1)
			{
				// - New Batch if Needed -
				if (s_RendererData->TextureSlotIndex >= s_RendererData->MaxTextureSlots)
				{
					NextBatchFunction();
					s_RendererData->TextureSlotIndex = 2; // 0 is white texture, 1 is normal texture
				}

				// - Set Texture -
				ret = s_RendererData->TextureSlotIndex;
				s_RendererData->TextureSlots[s_RendererData->TextureSlotIndex] = texture;
				++s_RendererData->TextureSlotIndex;
			}
		}

		return ret;
	}



	// ----------------------- Public Renderer Materials Methods ---------------------------------------------
	Ref<Material> Renderer::CreateMaterial(const std::string& name)
	{
		Ref<Material> material = CreateRef<Material>(name);
		s_RendererData->Materials.insert({ material->GetID(), material });
		return material;
	}

	bool Renderer::IsDefaultMaterial(uint material_id)
	{
		return material_id == s_RendererData->DefaultMaterialID;
	}

	

	// ----------------------- Private Renderer Materials Methods --------------------------------------------
	bool Renderer::MaterialExists(uint material_id)
	{
		return s_RendererData->Materials.find(material_id) != s_RendererData->Materials.end();
	}

	void Renderer::CreateDefaultMaterial(uint default_mat_id)
	{
		if (!MaterialExists(s_RendererData->DefaultMaterialID))
		{
			if (default_mat_id == 0)
			{
				Ref<Material> material = CreateRef<Material>("DefaultMaterial");
				s_RendererData->Materials.insert({ material->GetID(), material });
				s_RendererData->DefaultMaterialID = material->GetID();
			}
			else
			{
				s_RendererData->Materials.insert({ default_mat_id, CreateRef<Material>(new Material(default_mat_id, "DefaultMaterial")) });
				s_RendererData->DefaultMaterialID = default_mat_id;
			}
		}
	}

	Ref<Material> Renderer::CreateMaterialWithID(uint material_id, const std::string& name)
	{
		if (material_id == s_RendererData->DefaultMaterialID)
		{
			if (!MaterialExists(s_RendererData->DefaultMaterialID))
				KS_FATAL_ERROR("Tried to get default material but does not exists");

			return s_RendererData->Materials[s_RendererData->DefaultMaterialID];
		}

		Ref<Material> mat = CreateRef<Material>(new Material(material_id, name));
		s_RendererData->Materials.insert({ material_id, mat });
		return mat;
	}



	// ----------------------- Public Renderer Materials Getters ---------------------------------------------
	Ref<Material> Renderer::GetMaterial(uint material_id)
	{
		if (MaterialExists(material_id))
			return s_RendererData->Materials[material_id];

		return nullptr;
	}

	Ref<Material> Renderer::GetMaterialFromIndex(uint index)
	{
		if (index < s_RendererData->Materials.size())
		{
			uint i = 0;
			for (auto& mat : s_RendererData->Materials)
			{
				if (i == index)
					return mat.second;

				++i;
			}
		}

		return nullptr;
	}

	uint Renderer::GetMaterialIfExists(uint material_id)
	{
		if (MaterialExists(material_id))
			return material_id;

		return 0;
	}

	uint Renderer::GetDefaultMaterialID()
	{
		return s_RendererData->DefaultMaterialID;
	}

	uint Renderer::GetMaterialsQuantity()
	{
		if (s_RendererData)
			return s_RendererData->Materials.size();

		return 0;
	}
}
