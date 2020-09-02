#ifndef _SHADER_H_
#define _SHADER_H_

namespace Kaimos {

	class Shader
	{
	public:

		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

	private:

		uint m_ShaderID;
	};

}

#endif