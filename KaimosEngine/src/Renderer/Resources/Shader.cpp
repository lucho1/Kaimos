#include "kspch.h"
#include "Shader.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OGLShader.h"

namespace Kaimos {


	// ----------------------- Public Shader Methods ------------------------------------------------------
	// Here we decide which rendering API we are using, thus which kind of class type we instantiate/return
	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLShader>(filepath);
			case RendererAPI::API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected, or failed!");
		return nullptr;
	}


	Ref<Shader> Shader::Create(const std::string&  name, const std::string& vertex_src, const std::string& fragment_src)
	{
		switch (Renderer::GetRendererAPI())
		{
			case RendererAPI::API::OPENGL:		return CreateRef<OGLShader>(name, vertex_src, fragment_src);
			case RendererAPI::API::NONE:		KS_ENGINE_ASSERT(false, "RendererAPI is set to NONE (unsupported)!"); return nullptr;
		}

		KS_ENGINE_ASSERT(false, "RendererAPI is unknown, not selected, or failed!");
		return nullptr;
	}




	// ---------------------------- SHADER LIBRARY --------------------------------------------------------
	// ----------------------- Public ShaderLib Methods ---------------------------------------------------
	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}


	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		Add(shader->GetName(), shader);
	}
	
	void ShaderLibrary::Add(const std::string name, const Ref<Shader>& shader)
	{
		KS_ENGINE_ASSERT(!Exists(name), "Shader Already Exists!"); // Check that the name doesn't exists already!
		m_Shaders[name] = shader;
	}


	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		Ref<Shader>shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		Ref<Shader>shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}


	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		KS_ENGINE_ASSERT(Exists(name), "Shader Not Found!"); // Check that the name doesn't exists already!
		return m_Shaders[name];
	}
}
