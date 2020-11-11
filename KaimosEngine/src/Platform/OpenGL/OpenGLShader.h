#ifndef _OPENGLSHADER_H_
#define _OPENGLSHADER_H_

#include "Renderer/Shader.h"

#include <glm/glm.hpp>

// TODO: Remove this shit here!
typedef unsigned int GLenum;

namespace Kaimos {

	class OpenGLShader : public Shader
	{
	public:

		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		OpenGLShader(const std::string& filepath);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual const std::string& GetName() const override { return m_Name; }
		
		void UploadUniformInt(const std::string& name, const int& value);
		void UploadUniformFloat(const std::string& name, const float& value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:

		void CompileShader(const std::unordered_map<GLenum, std::string>&shaderSources);
		const std::unordered_map<GLenum, std::string> PreProcessShader(const std::string& source);
		const std::string ReadShaderFile(const std::string& filepath);

	private:

		uint m_ShaderID;
		std::string m_Name;
	};

}
#endif