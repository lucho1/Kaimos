#ifndef _OPENGLSHADER_H_
#define _OPENGLSHADER_H_

#include "Renderer/Resources/Shader.h"

#include <glm/glm.hpp>

// TODO: Remove this shit here!
typedef unsigned int GLenum;

namespace Kaimos {

	class OGLShader : public Shader
	{
	public:

		// --- Public Class Methods ---
		OGLShader(const std::string& name, const std::string& vertex_src, const std::string& fragment_Src);
		OGLShader(const std::string& filepath);
		virtual ~OGLShader();

		// --- Public Shader Methods ---
		virtual void Bind() const override;
		virtual void Unbind() const override;

		// --- Getters ---
		inline virtual const std::string& GetName() const override { return m_Name; }
		
	public:

		// --- Uniforms Set/Upload ---
		// By now is like this, but maybe we'd like to divide in 2 functions, a API-Specific Call (opengl -> glUniform) and a
		// high-level call/concept (where it might be set inside a uniform buffer, might set it individually not API-tied...)
		virtual void SetUniformFloat(const std::string& name, float value)						override;
		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value)			override;
		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value)			override;
		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value)			override;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value)			override;
		virtual void SetUniformInt(const std::string& name, int value)							override;
		virtual void SetUniformIntArray(const std::string& name, int* values_array, uint size)	override;

	private:

		// --- Private OGL Shader Methods ---
		std::string ReadShaderFile(const std::string& filepath);
		const std::unordered_map<GLenum, std::string> PreProcessShader(std::string& source);
		
		void CompileShader(const std::unordered_map<GLenum, std::string>&shader_sources);
		int GetUniformLocation(const std::string& name);

	private:

		uint m_ShaderID = 0;
		std::string m_Name = "Unnamed Shader";
		std::unordered_map<std::string, int> m_UniformCache;
	};
}

#endif //_OPENGLSHADER_H_
