#include "kspch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

namespace Kaimos {

	static GLenum ShaderTypeFromString(const std::string& ShaderType)
	{
		if (ShaderType == "VERTEX_SHADER")
			return GL_VERTEX_SHADER;
		if (ShaderType == "FRAGMENT_SHADER" || ShaderType == "PIXEL_SHADER")
			return GL_FRAGMENT_SHADER;

		KS_ENGINE_ASSERT(false, "Unknown Shader Type '{0}'", ShaderType.c_str());
		return 0;
	}

	static char* StringFromShaderType(const GLenum& ShaderType)
	{
		if (ShaderType == GL_VERTEX_SHADER)
			return "Vertex";
		if (ShaderType == GL_FRAGMENT_SHADER)
			return "Fragment/Pixel";

		KS_ENGINE_ASSERT(false, "Unknown Shader Type '{0}'", ShaderType);
		return 0;
	}

	
	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		KS_PROFILE_FUNCTION();

		// Compile Shader
		CompileShader(PreProcessShader(ReadShaderFile(filepath)));

		// Extract the shader name from filepath --> Basically the substring between last '/' or '\' and the last '.' (assets/textureSh.glsl = textureSh)
		// rfind is the same but will find exactly the character you pass (find_last will find any of the characters passed)
		size_t lastSlash = filepath.find_last_of("/\\");
		size_t lastDot = filepath.rfind('.');

		// lastSlash + 1 is to get "TextureSh" and not "/TextureSh" (and npos is in case we don't have slashes or previous paths)
		// If no '.', then we take the end of the string until the last slash (assets/TextureSh --> TextureSh), otherwise, we take from the '.' pos to the lastSlash (remember we are dealing with sizes,
		// if we begin the substr() at last "/" and end it at last "." and not "." - last"/", we will have errors of empty characters because the end pos will be bigger than it has to actually be!
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		lastDot = (lastDot == std::string::npos ? filepath.size() : lastDot) - lastSlash;
		m_Name = filepath.substr(lastSlash, lastDot);

		// This might be better:
		//std::filesystem::path path = filepath;
		//m_Name = path.stem().string(); // Returns the file's name stripped of the extension.
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) : m_Name(name)
	{
		KS_PROFILE_FUNCTION();

		// Set a source for the shader
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		
		// Compile shader source
		CompileShader(sources);
	}

	
	OpenGLShader::~OpenGLShader()
	{
		KS_PROFILE_FUNCTION();
		glDeleteProgram(m_ShaderID);
	}


	const std::string OpenGLShader::ReadShaderFile(const std::string& filepath)
	{
		KS_PROFILE_FUNCTION();

		// Input File Stream (to open a file) --> We give the filepath, tell it to process it as an input file
		// and to be read as binary (because we don't want to do any processing into it, it will be completely in the format we want it to be, otherwise is read as text, as strings)
		std::ifstream file(filepath, std::ios::in | std::ios::binary); // "or bitwise operator" (|), states the type of input stream, "we are opening the file as an input stream (read-only) and as a binary
		std::string ret;

		if (file)
		{
			// - First we see what's the size of the file -
			file.seekg(0, std::ios::end);			// seekg sets position in input sequence (in this case, place cursor at the very end of the file)
			
			size_t file_size = file.tellg();		// tellg says us where the file cursor is (since it's at the file end, it's the size of the file)
			if (file_size != -1)
			{
				// - Then create a string as big as the file size -				
				ret.resize(file_size);
				file.seekg(0, std::ios::beg);		// Place the cursor back to beginning of the file

				// - And finally load it all into that string -
				file.read(&ret[0], file_size);	// Put it into the string (passing a ptr to the string beginning), and with the size of the string

				// - Close the string -
				//file.close(); // This is actually not needed, ifstream closes itself due to RAII
				return ret;
			}
			else
				KS_ENGINE_ERROR("Couldn't read Shader file at path '{0}'", filepath);
		}
		else
			KS_ENGINE_ERROR("Couldn't open Shader file at path '{0}'", filepath);

		return ret;
	}

	const std::unordered_map<GLenum, std::string> OpenGLShader::PreProcessShader(const std::string& source)
	{
		KS_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> ret; // shader sources to return

		const char* typeToken = "#type"; // Token to designate the beginning of a new shader (check any .glsl file)
		size_t typeTokenLength = strlen(typeToken); // Length of the token
		size_t pos = source.find(typeToken, 0); // Start of shader type declaration line - Find the position of the token (to find the shader beginning) to use it as a cursor

		while (pos != std::string::npos) // While the position/cursor is not the end of the shader file/source code (we have to find all places where there is a #type word)
		{
			size_t eol = source.find_first_of("\r\n", pos);	// End of shader type declaration line - Find a new line or a carraige return from pos cursor
															// On Windows platform, a new line is represented by Carriage Return Line Feed (CRLF), a combination
															// of Enter key on keyboard and new line character. In other words "\r\n"... Basically, at the end of each line there are 2 chars on windows, "\r\n"

			// TODO: Fix assertions pls and make sure this works
			//KS_ENGINE_ASSERT(eol != std::string::npos, "Syntax Error!"); // If end of line is the end of shader (null, since pos isn't), there's a syntax error
			
			size_t begin = pos + typeTokenLength + 1;	// Start of shader type name (after '#type')
														// In this case, we are at the shader beginning (pos) + the length of "#type" + 1, this situates the begin
														// just in the place where the shader type is specificated, so if we hace "(whatever pos is)#type vertex",
														// begin is placed in pos + "#type" length + 1, so we are on "vertex" word beginning

			std::string shaderType = source.substr(begin, eol - begin);	// Since eol is gotten from pos cursor (so from the very beginning of the line), we need to get
																		// what's between "begin" (pos + # type + 1) and eol - begin (the whole line minus the "#type " space


			KS_ENGINE_ASSERT(ShaderTypeFromString(shaderType), "Invalid ShaderType specification or not supported"); // Assert if shader type invalid or not supported
			
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);			// Start of shader code after shader type declaration line - Find, from the end of previous line, the next line, which will be the first that we will find not being "\r\n" (an end of line)
			//KS_ENGINE_ASSERT(nextLinePos != std::string::npos, "Syntax Error");
			pos = source.find(typeToken, nextLinePos);							// Start of next shader type declaration line - Find the next "#type" word from the next line of the previous "#type X" statement, and put 'pos' in there (new size for pos, we are now getting, finally, the whole shader strings code)
			
			ret[ShaderTypeFromString(shaderType)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);

			//ret[ShaderTypeFromString(shaderType)] = source.substr(nextLinePos,
			//														pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
			// Here we have to put, into the final shader string (the final shader source code), everything between 'nextLinePos' (the line just below the "#type X" line) and
			// 'pos', which is now at the end of the shader (being this just before the next "#type X" or the end of the file). That's done with 'substr()'
			// So pos - nextLinePos... Except when nextLinePos is the end of the shader file, case in which we use, to calculate, the shader file size - 1 (so we don't have errors or 0s around)
			// Then we ho back up, where loop still runs and decides what to do based on what pos is (end of file, a new #type token...)
		}


		return ret;
	}

	void OpenGLShader::CompileShader(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		KS_PROFILE_FUNCTION();

		// Get a program object.
		GLuint program = glCreateProgram();
		std::vector<GLenum> glShaderIDs;
		glShaderIDs.reserve(shaderSources.size());

		// Instead, you could create an array, which is much better than a vector:
		//KS_ENGINE_ASSERT(shaderSources.size() <= 2, "Only 2 shader types are supported currently!");
		//std::array<GLenum, 2> glShaderIDs;
		// int glShaderIDIndex = 0; // In this case, the glShaderIDs.push_back() - downwards -, should be substituted by a glShaderIDs[glShaderIDIndex++] = shader;
		for (auto&& [key, value] : shaderSources)
		{
			GLenum type = key;
			const std::string& source = value;

			// Create an empty shader handle
			GLuint shader = glCreateShader(type);

			// Send the vertex shader source code to OGL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* GLShaderSource = source.c_str();
			glShaderSource(shader, 1, &GLShaderSource, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				// Use the infoLog to print error & assert
				KS_ENGINE_CRITICAL("{0} Shader Compilation Error: {1}", StringFromShaderType(type), infoLog.data());
				KS_ENGINE_ASSERT(false, "Shader Compilation Failure!");
				break;
			}

			// Attach our shaders to our program
			glAttachShader(program, shader);
			glShaderIDs.push_back(shader);
		}

		m_ShaderID = program;

		// Vertex and fragment shaders are successfully compiled. Now time to link them together into a program
		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);

			// Don't leak shaders either.
			for(auto id : glShaderIDs)
				glDeleteShader(id);

			// Use the infoLog to print error & assert
			KS_ENGINE_CRITICAL("Shader Linking Error: {0}", infoLog.data());
			KS_ENGINE_ASSERT(false, "Shader Program Link Failure!");
			return;
		}

		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
	}


	// --- Shader Operations ---
	void OpenGLShader::Bind() const
	{
		KS_PROFILE_FUNCTION();
		glUseProgram(m_ShaderID);
	}

	void OpenGLShader::Unbind() const
	{
		KS_PROFILE_FUNCTION();
		glUseProgram(0);
	}

	void OpenGLShader::SetUFloat(const std::string& name, float value)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetUFloat3(const std::string& name, const glm::vec3& value)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetUFloat4(const std::string& name, const glm::vec4& value)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetUMat4(const std::string& name, const glm::mat4& value)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformMat4(name, value);
	}
	
	void OpenGLShader::SetUInt(const std::string& name, int value)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetUIntArray(const std::string& name, int* values_array, uint size)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformIntArray(name, values_array, size);
	}


	// --- Uniforms Upload ---
	void OpenGLShader::UploadUniformInt(const std::string& name, const int& value)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1i(loc, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, const int* values_array, uint size)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1iv(loc, size, values_array);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, const float& value)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1f(loc, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform2f(loc, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform3f(loc, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform4f(loc, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
