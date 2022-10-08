#ifndef _SHADER_H_
#define _SHADER_H_

#include <glm/glm.hpp>

namespace Kaimos {

	class Shader
	{
	public:

		// --- Public Class Methods ---
		virtual ~Shader() = default;
		
		// --- Public Shader Methods ---
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertex_src, const std::string& fragment_src);

		// --- Getters ---
		virtual const std::string& GetName() const = 0;

	public:

		// --- Uniforms ---
		virtual void SetUniformFloat(const std::string& name, float value) = 0;
		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetUniformMat4(const std::string& name,  const glm::mat4& value) = 0;
		virtual void SetUniformInt(const std::string& name, int value) = 0;
		virtual void SetUniformIntArray(const std::string& name, int* values_array, uint size) = 0;
	};



	class ShaderLibrary
	{
	public:

		// --- Public ShaderLib Methods ---
		bool Exists(const std::string& name) const;

		void Add(const Ref<Shader>& shader);
		void Add(const std::string name, const Ref<Shader>& shader);
		
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		// --- Getters ---
		Ref<Shader> Get(const std::string& name);

		// --- Public Functionality Methods ---
		void ForEachShader(std::function<void(const Ref<Shader>&)> for_body);

	private:

		std::unordered_map<std::string, Ref<Shader>> m_Shaders; // name & shader reference
	};
}

#endif //_SHADER_H_
