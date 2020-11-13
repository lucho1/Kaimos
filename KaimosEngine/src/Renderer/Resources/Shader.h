#ifndef _SHADER_H_
#define _SHADER_H_

#include <glm/glm.hpp>

namespace Kaimos {

	class Shader
	{
	public:

		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetUMat4(const std::string& name,  const glm::mat4& value) = 0;
		virtual void SetUInt(const std::string& name, float value) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};


	class ShaderLibrary
	{
	public:

		void Add(const Ref<Shader>& shader);
		void Add(const std::string name, const Ref<Shader>& shader);
		
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;

	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders; // name & shader itself
	};
}

#endif