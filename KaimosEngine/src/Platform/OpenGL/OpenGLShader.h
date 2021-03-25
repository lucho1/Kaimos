#ifndef _OPENGLSHADER_H_
#define _OPENGLSHADER_H_

#include "Renderer/Resources/Shader.h"

#include <glm/glm.hpp>

// TODO: Remove this shit here!
typedef unsigned int GLenum;

namespace Kaimos {

	class OpenGLShader : public Shader
	{
	public:

		// --- Public Class Methods ---
		OpenGLShader(const std::string& name, const std::string& vertex_src, const std::string& fragment_Src);
		OpenGLShader(const std::string& filepath);
		virtual ~OpenGLShader();

		// --- Public Shader Methods ---
		virtual void Bind() const override;
		virtual void Unbind() const override;

		// --- Getters ---
		inline virtual const std::string& GetName() const override { return m_Name; }
		
	public:

		// --- Uniforms Set/Upload ---
		// Upload:	API-Specific Call (opengl -> glUniform...)
		// Set:		high-level call/concept, it might be set inside a uniform buffer, might set it individually (not API-tied)...
		virtual void SetUFloat(const std::string& name, float value)						override;
		virtual void SetUFloat3(const std::string& name, const glm::vec3& value)			override;
		virtual void SetUFloat4(const std::string& name, const glm::vec4& value)			override;
		virtual void SetUMat4(const std::string& name, const glm::mat4& value)				override;
		virtual void SetUInt(const std::string& name, int value)							override;
		virtual void SetUIntArray(const std::string& name, int* values_array, uint size)	override;

		void UploadUniformIntArray(const std::string& name, const int* values_array, uint size);
		void UploadUniformInt(const std::string& name, const int& value);
		void UploadUniformFloat(const std::string& name, const float& value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:

		// --- Private OGL Shader Methods ---
		const std::string ReadShaderFile(const std::string& filepath);
		const std::unordered_map<GLenum, std::string> PreProcessShader(const std::string& source);
		
		void CompileShader(const std::unordered_map<GLenum, std::string>&shader_sources);

	private:

		uint m_ShaderID = 0;
		std::string m_Name = "Unnamed Shader";
	};
}

#endif //_OPENGLSHADER_H_
