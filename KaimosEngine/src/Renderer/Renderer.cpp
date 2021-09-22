#include "kspch.h"
#include "Renderer.h"

#include "Core/Resources/ResourceManager.h"

#include "OpenGL/Resources/OGLShader.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/Shader.h"
#include "Resources/Texture.h"
#include "Resources/Light.h"
#include "Resources/Framebuffer.h"

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
		bool PBR_Pipeline = false;
		uint CameraUIDisplayOption = 0;
		
		// Shaders & Materials
		ShaderLibrary Shaders;
		uint DefaultMaterialID = 0;
		std::unordered_map<uint, Ref<Material>> Materials;

		// Textures
		// Although 32 is MaxTextures on OpenGL, the last ones must be for Environment Mapping
		uint TextureSlotIndex = 2;									// Slot 0 -> White Texture, Slot 1 -> Normal Texture
		static const uint MaxTextureSlots = 29;						// TODO: RenderCapabilities - Variables based on what the hardware can do
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		Ref<Texture2D> WhiteTexture = nullptr, NormalTexture = nullptr;

		// Environment Mapping
		Ref<VertexArray> CubeVArray = nullptr;
		Ref<VertexArray> QuadVArray = nullptr;

		uint EnvironmentMapResolution = 1024;
		uint EnvrionmentPrefilteredMapResolution = 128;
		uint EnvironmentIrradianceMapResolution = 32;
		std::string EnvironmentMapFilepath = "";
		Ref<HDRTexture2D> EnvironmentHDRMap = nullptr;
		Ref<CubemapTexture> EnvironmentCubemap = nullptr, IrradianceCubemap = nullptr, PrefilterCubemap = nullptr;
		Ref<LUTTexture> BRDF_LutTexture = nullptr;
		Ref<Framebuffer> EnvironmentMapFBO = nullptr;
	};

	static RendererData* s_RendererData = nullptr;
	static bool s_CompileEnvironmentMap = false;


	// ----------------------- Public Class Methods -------------------------------------------------------
	void Renderer::CreateRenderer()
	{
		KS_PROFILE_FUNCTION();
		KS_INFO("\n\n--- CREATING KAIMOS RENDERER ---");

		// -- Setup --
		s_RendererData = new RendererData();
		SetCubemapVertices();
		SetQuadVertices();

		// -- Shaders Creation --
		s_RendererData->Shaders.Load("BatchedShader", "assets/shaders/BatchRenderingShader.glsl");
		s_RendererData->Shaders.Load("PBR_BatchedShader", "assets/shaders/PBR_BatchRenderingShader.glsl");
		s_RendererData->Shaders.Load("EquirectangularToCubemap", "assets/shaders/ibl/EquirectangularToCubemapShader.glsl");
		s_RendererData->Shaders.Load("CubemapConvolution", "assets/shaders/ibl/CubemapConvolutionShader.glsl");
		s_RendererData->Shaders.Load("IBL_Prefiltered", "assets/shaders/ibl/IBL_PrefilteringShader.glsl");
		s_RendererData->Shaders.Load("BRDF_Integration", "assets/shaders/ibl/BRDFConvolutionShader.glsl");
		s_RendererData->Shaders.Load("SkyboxShader", "assets/shaders/SkyboxShader.glsl");

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

		RenderCommand::EnableDepth();
		RenderCommand::EnableCubemapFiltering();
	}

	void Renderer::Shutdown()
	{
		KS_INFO("\n\n--- SHUTTING DOWN KAIMOS RENDERER ---");
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
		RemoveEnvironmentMap();

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
	bool Renderer::BeginScene(const glm::mat4& view_projection_matrix, const glm::vec3& camera_pos, const std::vector<std::pair<Ref<Light>, glm::vec3>>& dir_lights, const std::vector<std::pair<Ref<PointLight>, glm::vec3>>& point_lights)
	{
		KS_PROFILE_FUNCTION();
		if (s_CompileEnvironmentMap)
		{
			CompileEnvironmentMap();
			s_CompileEnvironmentMap = false;
			return false;
		}

		Ref<Shader> shader = s_RendererData->PBR_Pipeline ? GetShader("PBR_BatchedShader") : GetShader("BatchedShader");
		if (shader)
		{
			// Set Common Shader Uniforms
			shader->Bind();
			shader->SetUniformMat4("u_ViewProjection", view_projection_matrix);
			shader->SetUniformFloat3("u_ViewPos", camera_pos);
			shader->SetUniformFloat3("u_SceneColor", s_RendererData->SceneColor);

			// Bind Environment Textures
			if (s_RendererData->IrradianceCubemap && s_RendererData->PrefilterCubemap && s_RendererData->BRDF_LutTexture)
			{
				s_RendererData->IrradianceCubemap->Bind(29);
				s_RendererData->PrefilterCubemap->Bind(30);
				s_RendererData->BRDF_LutTexture->Bind(31);
				shader->SetUniformInt("u_IrradianceMap", 29);
				shader->SetUniformInt("u_PrefilterSpecularMap", 30);
				shader->SetUniformInt("u_BRDF_LUTMap", 31);
			}

			// Set Directional Lights Uniforms
			uint dir_lights_num = dir_lights.size() >= s_RendererData->MaxDirLights ? s_RendererData->MaxDirLights : dir_lights.size();
			shader->SetUniformInt("u_DirectionalLightsNum", dir_lights_num);

			for (uint i = 0; i < dir_lights_num; ++i)
			{
				Ref<Light> light = dir_lights[i].first;
				std::string light_array_uniform = "u_DirectionalLights[" + std::to_string(i) + "].";

				shader->SetUniformFloat4(light_array_uniform + "Radiance", light->Radiance);
				shader->SetUniformFloat3(light_array_uniform + "Direction", dir_lights[i].second);
				shader->SetUniformFloat(light_array_uniform + "Intensity", light->Intensity);
				shader->SetUniformFloat(light_array_uniform + "SpecularStrength", light->SpecularStrength);
			}

			// Set Point Lights Uniforms
			uint point_lights_num = point_lights.size() >= s_RendererData->MaxPointLights ? s_RendererData->MaxPointLights : point_lights.size();
			shader->SetUniformInt("u_PointLightsNum", point_lights_num);

			for (uint i = 0; i < point_lights_num; ++i)
			{
				Ref<PointLight> light = point_lights[i].first;
				std::string light_array_uniform = "u_PointLights[" + std::to_string(i) + "].";

				// if()else() with pbr_pipeline passing (or not) values to it
				shader->SetUniformFloat4(light_array_uniform + "Radiance", light->Radiance);
				shader->SetUniformFloat3(light_array_uniform + "Position", point_lights[i].second);
				shader->SetUniformFloat(light_array_uniform + "Intensity", light->Intensity);
				shader->SetUniformFloat(light_array_uniform + "FalloffFactor", light->FalloffMultiplier);
				shader->SetUniformFloat(light_array_uniform + "SpecularStrength", light->SpecularStrength);

				if (s_RendererData->PBR_Pipeline)
					shader->SetUniformFloat(light_array_uniform + "Radius", light->GetMinRadius());
				else
				{
					shader->SetUniformFloat(light_array_uniform + "MinRadius", light->GetMinRadius());
					shader->SetUniformFloat(light_array_uniform + "MaxRadius", light->GetMaxRadius());
					shader->SetUniformFloat(light_array_uniform + "AttL", light->GetLinearAttenuationFactor());
					shader->SetUniformFloat(light_array_uniform + "AttQ", light->GetQuadraticAttenuationFactor());
				}
			}
		}
		else
			KS_FATAL_ERROR("Renderer: Tried to Render with a null Shader!");

		return true;
	}

	void Renderer::EndScene(const glm::mat4& view_matrix, const glm::mat4& projection_matrix)
	{
		KS_PROFILE_FUNCTION();
		Ref<Shader> skybox_shader = GetShader("SkyboxShader");
		if (skybox_shader && s_RendererData->EnvironmentCubemap)
		{
			skybox_shader->Bind();
			glm::mat4 view_proj = projection_matrix * glm::mat4(glm::mat3(view_matrix));
			skybox_shader->SetUniformMat4("u_ViewProjection", view_proj);
			skybox_shader->SetUniformFloat3("u_SceneColor", s_RendererData->SceneColor);

			s_RendererData->EnvironmentCubemap->Bind();
			//s_RendererData->IrradianceCubemap->Bind();
			//s_RendererData->PrefilterCubemap->Bind();
			skybox_shader->SetUniformInt("u_Cubemap", 0);
			RenderCube();
			skybox_shader->Unbind();
		}
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
					mat_graph->DeserializeGraph(graph_subnode, material);

					// Finally, assign graph & texture to material
					material->SetGraphUniqueRef(mat_graph);
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
	uint Renderer::GetCameraUIDisplayOption()
	{
		return s_RendererData->CameraUIDisplayOption;
	}

	void Renderer::SetCameraUIDisplayOption(uint option)
	{
		if(option < 3)
			s_RendererData->CameraUIDisplayOption = option;
	}

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

	bool Renderer::IsSceneInPBRPipeline()
	{
		return s_RendererData->PBR_Pipeline;
	}

	void Renderer::SetPBRPipeline(bool pbr_pipeline)
	{
		if (s_RendererData->PBR_Pipeline != pbr_pipeline)
		{
			s_RendererData->PBR_Pipeline = pbr_pipeline;
			for (auto& mat : s_RendererData->Materials)
				mat.second->InPBRPipeline = pbr_pipeline;
		}
	}

	uint Renderer::GetEnvironmentMapID()
	{
		if (s_RendererData->EnvironmentHDRMap)
			return s_RendererData->EnvironmentHDRMap->GetTextureID();

		return 0;
	}

	glm::ivec2 Renderer::GetEnvironmentMapSize()
	{
		if (s_RendererData->EnvironmentHDRMap)
			return glm::ivec2(s_RendererData->EnvironmentHDRMap->GetWidth(), s_RendererData->EnvironmentHDRMap->GetHeight());

		return glm::ivec2(0);
	}

	uint Renderer::GetEnvironmentMapResolution()
	{
		return s_RendererData->EnvironmentMapResolution;
	}

	uint Renderer::GetEnviroPrefilterMapResolution()
	{
		return s_RendererData->EnvrionmentPrefilteredMapResolution;
	}

	uint Renderer::GetEnviroIrradianceMapResolution()
	{
		return s_RendererData->EnvironmentIrradianceMapResolution;
	}

	std::string Renderer::GetEnvironmentMapFilepath()
	{
		if (s_RendererData->EnvironmentHDRMap)
			return s_RendererData->EnvironmentHDRMap->GetFilepath();

		return "";
	}

	void Renderer::SetEnvironmentMapFilepath(const std::string& filepath, uint environment_map_resolution, uint prefiltered_map_resolution, uint irradiance_map_resolution)
	{
		// -- Check Path Validity --
		KS_PROFILE_FUNCTION();
		if (!Resources::ResourceManager::CheckValidPathForHDRTexture(filepath))
		{
			KS_ERROR("Renderer Error: Couldn't load HDR texture file: {0}", filepath);
			return;
		}

		// -- Check is not the same Path --
		if (s_RendererData->EnvironmentHDRMap && s_RendererData->EnvironmentMapFilepath == filepath)
		{
			KS_WARN("The selected Environment Map is already in use!");
			return;
		}

		// -- Set Data & Recompile --
		KS_TRACE("Compiling Environment Map, please wait...");
		s_RendererData->EnvironmentMapResolution = environment_map_resolution;
		s_RendererData->EnvrionmentPrefilteredMapResolution = prefiltered_map_resolution;
		s_RendererData->EnvironmentIrradianceMapResolution = irradiance_map_resolution;
		s_RendererData->EnvironmentMapFilepath = filepath;
		s_CompileEnvironmentMap = true;
	}

	void Renderer::ForceEnvironmentMapRecompile(uint environment_map_resolution, uint prefiltered_map_resolution, uint irradiance_map_resolution)
	{
		// -- Check Map, Set Data & Recompile --
		if (s_RendererData->EnvironmentHDRMap && Resources::ResourceManager::CheckValidPathForHDRTexture(s_RendererData->EnvironmentMapFilepath))
		{
			KS_TRACE("Recompiling Environment Map, please wait...");
			s_RendererData->EnvironmentMapResolution = environment_map_resolution;
			s_RendererData->EnvrionmentPrefilteredMapResolution = prefiltered_map_resolution;
			s_RendererData->EnvironmentIrradianceMapResolution = irradiance_map_resolution;
			s_CompileEnvironmentMap = true;
		}
		else
			KS_WARN("Unexisting or Invalid Environment Map, couldn't recompile it");
	}

	void Renderer::CompileEnvironmentMap()
	{
		KS_PROFILE_FUNCTION();

		// -- Get Needed Shaders --
		Ref<Shader> recttocube_shader		= GetShader("EquirectangularToCubemap");
		Ref<Shader> irradiance_shader		= GetShader("CubemapConvolution");
		Ref<Shader> prefilter_shader		= GetShader("IBL_Prefiltered");
		Ref<Shader> brdf_integration_shader	= GetShader("BRDF_Integration");

		if (!recttocube_shader || !irradiance_shader || !prefilter_shader || !brdf_integration_shader)
		{
			KS_WARN("Couldn't find or get the necessary Shaders to setup the Environment Map, aborting...");
			return;
		}

		// -- Remove Previous Environment Map --
		//Sleep(2000);
		RemoveEnvironmentMap();

		// -- Setup Variables --
		// We're dealing with cubes, so all faces have = size or resolution
		uint envmap_res = s_RendererData->EnvironmentMapResolution;
		uint prefiltermap_res = s_RendererData->EnvrionmentPrefilteredMapResolution;
		uint irradiancemap_res = s_RendererData->EnvironmentIrradianceMapResolution;
		uint lut_res = 128;

		glm::mat4 capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 capture_views[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),	glm::vec3(1.0f,  0.0f,  0.0f),	glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),	glm::vec3(-1.0f,  0.0f,  0.0f),	glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),	glm::vec3(0.0f,  1.0f,  0.0f),	glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),	glm::vec3(0.0f, -1.0f,  0.0f),	glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),	glm::vec3(0.0f,  0.0f,  1.0f),	glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),	glm::vec3(0.0f,  0.0f, -1.0f),	glm::vec3(0.0f, -1.0f,  0.0f))
		};


		// -- Create Environment FBO, HDR Map & Cubemap --
		s_RendererData->EnvironmentMapFBO = Framebuffer::CreateEmptyAndBind(envmap_res, envmap_res, true); // URGENT TODO: width, height
		s_RendererData->EnvironmentMapFBO->Bind();
		s_RendererData->EnvironmentHDRMap = HDRTexture2D::Create(s_RendererData->EnvironmentMapFilepath);
		s_RendererData->EnvironmentCubemap = CubemapTexture::Create(envmap_res, envmap_res, true);


		// -- Equirectangular to Cubemap Step --
		recttocube_shader->Bind();
		recttocube_shader->SetUniformInt("u_EquirectangularMap", 0);

		// Bind HDRMap + FBO & Draw from 6 Perspectives
		uint env_cubemap_id = s_RendererData->EnvironmentCubemap->GetTextureID();
		s_RendererData->EnvironmentHDRMap->Bind();
		s_RendererData->EnvironmentMapFBO->Bind();

		for (uint i = 0; i < 6; ++i)
		{
			recttocube_shader->SetUniformMat4("u_ViewProjection", capture_projection * capture_views[i]);
			s_RendererData->EnvironmentMapFBO->AttachColorTexture(TEXTURE_TARGET::TEXTURE_CUBEMAP, i, env_cubemap_id);
			RenderCommand::Clear();
			RenderCube();
		}

		// Unbind
		s_RendererData->EnvironmentMapFBO->Unbind();
		s_RendererData->EnvironmentCubemap->GenerateMipMap();
		recttocube_shader->Unbind();


		// -- Irradiance Convolution Step --
		// Create Map
		s_RendererData->IrradianceCubemap = CubemapTexture::Create(irradiancemap_res, irradiancemap_res);
		s_RendererData->EnvironmentMapFBO->ResizeAndBindRenderBuffer(irradiancemap_res, irradiancemap_res);

		// Bind Shader, Cubemap & FBO + Render from 6 perspectives
		uint irr_cubemap_id = s_RendererData->IrradianceCubemap->GetTextureID();

		irradiance_shader->Bind();
		irradiance_shader->SetUniformInt("u_EnvironmentMapResolution", envmap_res);
		irradiance_shader->SetUniformInt("u_Cubemap", 0);
		s_RendererData->EnvironmentCubemap->Bind();
		s_RendererData->EnvironmentMapFBO->Bind(irradiancemap_res, irradiancemap_res);

		for (uint i = 0; i < 6; ++i)
		{
			irradiance_shader->SetUniformMat4("u_ViewProjection", capture_projection * capture_views[i]);
			s_RendererData->EnvironmentMapFBO->AttachColorTexture(TEXTURE_TARGET::TEXTURE_CUBEMAP, i, irr_cubemap_id);
			RenderCommand::Clear();
			RenderCube();
		}

		// Unbind
		s_RendererData->EnvironmentMapFBO->Unbind();
		irradiance_shader->Unbind();


		// -- IBL Specular Prefilter Step --
		// Create Map & Mipmaps
		s_RendererData->PrefilterCubemap = CubemapTexture::Create(prefiltermap_res, prefiltermap_res, true);
		s_RendererData->PrefilterCubemap->GenerateMipMap();

		// Bind Shader, Cubemap & FBO
		prefilter_shader->Bind();
		prefilter_shader->SetUniformInt("u_EnvironmentMapResolution", envmap_res);
		prefilter_shader->SetUniformInt("u_EnvironmentMap", 0);

		s_RendererData->EnvironmentCubemap->Bind();
		s_RendererData->EnvironmentMapFBO->Bind();

		// Render from 6 perspectives for each mip-level to generate
		uint prefiltermap_id = s_RendererData->PrefilterCubemap->GetTextureID();
		uint mip_levels = 13;

		for (uint mip = 0; mip < mip_levels; ++mip)
		{
			// Calculate mip resolution
			uint mip_res = prefiltermap_res * glm::pow(0.5f, mip);

			// Resize & Bind FBO to match mip resolution
			s_RendererData->EnvironmentMapFBO->ResizeAndBindRenderBuffer(mip_res, mip_res);
			s_RendererData->EnvironmentMapFBO->Bind(mip_res, mip_res);

			// Set roughness
			float roughness = (float)mip / (float)(mip_levels - 1);
			prefilter_shader->SetUniformFloat("u_Roughness", roughness);

			// Render 6 perspectives
			for (uint i = 0; i < 6; ++i)
			{
				prefilter_shader->SetUniformMat4("u_ViewProjection", capture_projection * capture_views[i]);
				s_RendererData->EnvironmentMapFBO->AttachColorTexture(TEXTURE_TARGET::TEXTURE_CUBEMAP, i, prefiltermap_id, mip);
				RenderCommand::Clear();
				RenderCube();
			}
		}

		// Unbind
		s_RendererData->EnvironmentMapFBO->Unbind();
		prefilter_shader->Unbind();


		// -- BRDF Convolution Step --
		s_RendererData->BRDF_LutTexture = LUTTexture::Create(lut_res);

		// Bind & Resize FBO/RBO & Attach Texture
		s_RendererData->EnvironmentMapFBO->ResizeAndBindRenderBuffer(lut_res, lut_res);
		s_RendererData->EnvironmentMapFBO->AttachColorTexture(TEXTURE_TARGET::TEXTURE_2D, 0, s_RendererData->BRDF_LutTexture->GetTextureID());
		s_RendererData->EnvironmentMapFBO->Bind(lut_res, lut_res);

		// Render Quad
		brdf_integration_shader->Bind();
		RenderCommand::Clear();
		RenderQuad();

		// Unbind
		brdf_integration_shader->Unbind();


		// -- Create FBO Red Texture (for mouse picking) --
		s_RendererData->EnvironmentMapFBO->CreateAndAttachRedTexture(1, envmap_res, envmap_res);

		// -- Final Unbind + LOG --
		s_RendererData->EnvironmentMapFBO->Unbind();
		KS_TRACE("Successfully Changed the Environment Map");
	}

	void Renderer::RemoveEnvironmentMap()
	{
		if (s_RendererData->EnvironmentHDRMap)
			s_RendererData->EnvironmentHDRMap.reset();

		if (s_RendererData->EnvironmentMapFBO)
			s_RendererData->EnvironmentMapFBO.reset();
		
		if (s_RendererData->EnvironmentCubemap)
			s_RendererData->EnvironmentCubemap.reset();

		if (s_RendererData->IrradianceCubemap)
			s_RendererData->IrradianceCubemap.reset();

		if (s_RendererData->PrefilterCubemap)
			s_RendererData->PrefilterCubemap.reset();

		if (s_RendererData->BRDF_LutTexture)
			s_RendererData->BRDF_LutTexture.reset();
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
		uint tex_count = 0;
		if (material->HasAlbedo())
			++tex_count;
		if (material->HasNormal())
			++tex_count;

		if (s_RendererData->PBR_Pipeline)
		{
			if (material->HasMetallic())
				++tex_count;
			if (material->HasRoughness())
				++tex_count;
			if (material->HasAmbientOcc())
				++tex_count;
		}
		else if (material->HasSpecular())
				++tex_count;

		if (s_RendererData->TextureSlotIndex >= (s_RendererData->MaxTextureSlots - tex_count - 1))
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

	void Renderer::SetCubemapVertices()
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};

		BufferLayout cube_layout = {{ SHADER_DATATYPE::FLOAT3, "a_Position" }, { SHADER_DATATYPE::FLOAT2, "a_TexCoord" }, { SHADER_DATATYPE::FLOAT3, "a_Normal" }};
		s_RendererData->CubeVArray = VertexArray::Create();
		Ref<VertexBuffer> cube_vbuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		cube_vbuffer->SetLayout(cube_layout);
		s_RendererData->CubeVArray->AddVertexBuffer(cube_vbuffer);
		
		s_RendererData->CubeVArray->Unbind();
		cube_vbuffer->Unbind();
	}

	void Renderer::SetQuadVertices()
	{
		float vertices[] = {
			// Pos				// TCoords
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		};
		
		uint indices[] = {
			0, 1, 3,
			1, 2, 3
		};

		s_RendererData->QuadVArray = VertexArray::Create();
		Ref<VertexBuffer> quad_vbuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		
		Ref<IndexBuffer> index_buffer = IndexBuffer::Create(indices, 6);
		BufferLayout quad_layout = { { SHADER_DATATYPE::FLOAT3, "a_Position" }, { SHADER_DATATYPE::FLOAT2, "a_TexCoord" } };

		quad_vbuffer->SetLayout(quad_layout);
		s_RendererData->QuadVArray->AddVertexBuffer(quad_vbuffer);
		s_RendererData->QuadVArray->SetIndexBuffer(index_buffer);

		s_RendererData->QuadVArray->Unbind();
		quad_vbuffer->Unbind();
		index_buffer->Unbind();
	}

	void Renderer::RenderQuad()
	{
		s_RendererData->QuadVArray->Bind();
		RenderCommand::DrawIndexed(s_RendererData->QuadVArray);
		//s_RendererData->QuadVArray->Unbind();
	}

	void Renderer::RenderCube()
	{
		s_RendererData->CubeVArray->Bind();
		RenderCommand::DrawUnindexed(s_RendererData->CubeVArray, 36);
		//s_RendererData->CubeVArray->Unbind();
	}
}
