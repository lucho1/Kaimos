#include "kspch.h"
#include "OGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

namespace Kaimos {

	// ----------------------- Globals --------------------------------------------------------------------
	static GLenum ShaderTypeFromString(const std::string& shader_type)
	{
		if (shader_type == "VERTEX_SHADER")
			return GL_VERTEX_SHADER;
		if (shader_type == "FRAGMENT_SHADER" || shader_type == "PIXEL_SHADER")
			return GL_FRAGMENT_SHADER;

		KS_ENGINE_ASSERT(false, "Unknown Shader Type '{0}'", shader_type.c_str());
		return 0;
	}

	static char* StringFromShaderType(const GLenum& shader_type)
	{
		if (shader_type == GL_VERTEX_SHADER)
			return "Vertex";
		if (shader_type == GL_FRAGMENT_SHADER)
			return "Fragment/Pixel";

		KS_ENGINE_ASSERT(false, "Unknown Shader Type '{0}'", shader_type);
		return 0;
	}


	
	// ----------------------- Public Class Methods -------------------------------------------------------
	OGLShader::OGLShader(const std::string& filepath)
	{
		KS_PROFILE_FUNCTION();

		// -- Compile Shader --
		CompileShader(PreProcessShader(ReadShaderFile(filepath)));

		// -- Shader Name --
		// Shader name from filepath --> Substring between last '/' or '\' and the last '.' (assets/textureSh.glsl = textureSh)
		// rfind is the same but will find exactly the character you pass (find_last will find any of the characters passed)
		size_t last_slash = filepath.find_last_of("/\\");
		size_t last_dot = filepath.rfind('.');

		// lastSlash + 1 is to get "TextureSh" and not "/TextureSh" (and npos is in case we don't have slashes or previous paths)
		// If no '.', then we take the end of the string until the last slash (assets/TextureSh --> TextureSh), otherwise, we take from the '.' pos to the lastSlash (remember we are dealing with sizes,
		// if we begin the substr() at last "/" and end it at last "." and not "." - last"/", we will have errors of empty characters because the end pos will be bigger than it has to actually be!
		last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
		last_dot = (last_dot == std::string::npos ? filepath.size() : last_dot) - last_slash;
		m_Name = filepath.substr(last_slash, last_dot);

		// This might be better:
		//std::filesystem::path path = filepath;
		//m_Name = path.stem().string(); // Returns the file's name stripped of the extension.
	}


	OGLShader::OGLShader(const std::string& name, const std::string& vertex_src, const std::string& fragment_Src) : m_Name(name)
	{
		KS_PROFILE_FUNCTION();

		// -- Set a Source for the Shader --
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertex_src;
		sources[GL_FRAGMENT_SHADER] = fragment_Src;
		
		// -- Compile Shader Source --
		CompileShader(sources);
	}

	
	OGLShader::~OGLShader()
	{
		KS_PROFILE_FUNCTION();
		glDeleteProgram(m_ShaderID);
	}



	// ----------------------- Public Shader Methods ------------------------------------------------------
	void OGLShader::Bind() const
	{
		KS_PROFILE_FUNCTION();
		glUseProgram(m_ShaderID);
	}

	void OGLShader::Unbind() const
	{
		KS_PROFILE_FUNCTION();
		glUseProgram(0);
	}


	
	// ----------------------- Private OGL Shader Methods -------------------------------------------------
	const std::string OGLShader::ReadShaderFile(const std::string& filepath)
	{
		KS_PROFILE_FUNCTION();

		// -- Open Shader File --
		// Input File Stream (to open a file) --> We give the filepath, tell it to process it as an input file
		// and to be read as binary (because we don't want to do any processing into it, it will be completely in the format we want it to be, otherwise is read as text, as strings)
		std::ifstream file(filepath, std::ios::in | std::ios::binary); // "or bitwise operator" (|), states the type of input stream, "we are opening the file as an input stream (read-only) and as a binary
		std::string ret;

		if (file)
		{
			// -- File Size --
			file.seekg(0, std::ios::end);				// seekg sets position in input sequence (in this case, place cursor at the very end of the file)
			
			size_t file_size = file.tellg();			// tellg says us where the file cursor is (since it's at the file end, it's the size of the file)
			if (file_size != -1)
			{
				// -- Create a string with the size of file --				
				ret.resize(file_size);
				file.seekg(0, std::ios::beg);			// Place the cursor back to beginning of the file

				// -- Load it all into that string --
				file.read(&ret[0], file_size);			// Put it into the string (passing a ptr to the string beginning), and with the size of the string

				// -- Close String --
				//file.close();							// Actually not needed, ifstream closes itself due to RAII
				return ret;
			}
			else { // Sorry for this {, it's needed because of the next define call
				KS_ENGINE_ERROR("Couldn't read Shader file at path '{0}'", filepath);
			}
		}
		else {
			KS_ENGINE_ERROR("Couldn't open Shader file at path '{0}'", filepath);
		}

		return ret;
	}


	const std::unordered_map<GLenum, std::string> OGLShader::PreProcessShader(const std::string& source)
	{
		KS_PROFILE_FUNCTION();

		// -- Shader Sources to Return
		std::unordered_map<GLenum, std::string> ret;

		// -- Variables to Process Shader --
		const char* type_token = "#type";						// Token to designate the beginning of a new shader (check any .glsl file)
		size_t type_token_length = strlen(type_token);			// Length of the token
		size_t pos = source.find(type_token, 0);				// Start of shader type declaration line - Find the position of the token (to find the shader beginning) to use it as a cursor

		// While position/cursor is not the end of the shader file/source code (we have to find all places where there is a #type word)
		while (pos != std::string::npos)
		{
			// -- End of shader type declaration line: find a new line or a carraige return from pos cursor --
			// On Windows platform, a new line is represented by Carriage Return Line Feed (CRLF), a combination
			// of Enter key on keyboard and new line character (TLDR: At end of line there are 2 chars on windows, "\r\n")
			size_t eol = source.find_first_of("\r\n", pos);


			// -- Syntax Error: eol is shader end (null, since pos isn't) --
			KS_ENGINE_ASSERT(eol != std::string::npos, "Syntax Error");
			if (eol == std::string::npos)
			{
				KS_ENGINE_ERROR("Shader Syntax Error - Shader End Of Line is null");
				return {};
			}
			

			// -- Start of shader type name (after '#type') --
			// We are at shader beginning (pos) + length of "#type" + 1, this situates the begin in the place where the shader type is specified
			// "(whatever pos is)#type vertex" == begin in 'pos + "#type" length + 1' == "vertex" word beginning
			size_t begin = pos + type_token_length + 1;


			// eol is gotten from pos cursor (from the beginning of the line), we need to get what's between "begin" (pos + #type + 1) & eol - begin (the whole line minus the "#type " space)
			std::string shader_type = source.substr(begin, eol - begin); 


			// -- Error if shader type invalid or not supported --
			GLenum gl_shader_type = ShaderTypeFromString(shader_type);
			KS_ENGINE_ASSERT(gl_shader_type, "Invalid ShaderType specification or not supported");
			if (gl_shader_type == 0)
			{
				KS_ENGINE_ERROR("Shader Syntax Error - Invalid Shader Type specification or not supported");
				return {};
			}
			
			// -- Start of shader code after shader type declaration line --
			// Find, from the end of previous line, the next line, which will be the first that we will find not being "\r\n" (an end of line)
			size_t next_line_pos = source.find_first_not_of("\r\n", eol);

			// -- Syntax Error --
			KS_ENGINE_ASSERT(next_line_pos != std::string::npos, "Syntax Error");
			if (next_line_pos == std::string::npos)
			{
				KS_ENGINE_ERROR("Shader Syntax Error - The keyword #type (or the following lines), specificating the Shader Type, could not be found or was wrong");
				return {};
			}

			// Start of next shader type declaration line - Find the next "#type" word from the next line of the previous "#type X" statement,
			// and put 'pos' in there (new size for pos, we are now getting, finally, the whole shader strings code)
			pos = source.find(type_token, next_line_pos);
			ret[ShaderTypeFromString(shader_type)] = (pos == std::string::npos) ? source.substr(next_line_pos) : source.substr(next_line_pos, pos - next_line_pos);


			//ret[ShaderTypeFromString(shaderType)] = source.substr(nextLinePos,
			//														pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
			// Here we have to put, into the final shader string (the final shader source code), everything between 'next_line_pos' (the line just below the "#type X" line) and
			// 'pos', which is now at the end of the shader (being this just before the next "#type X" or the end of the file). That's done with 'substr()'
			// So pos - nextLinePos... Except when nextLinePos is the end of the shader file, case in which we use, to calculate, the shader file size - 1 (so we don't have errors or 0s around)
			// Then we ho back up, where loop still runs and decides what to do based on what pos is (end of file, a new #type token...)
		}

		return ret;
	}


	void OGLShader::CompileShader(const std::unordered_map<GLenum, std::string>& shader_sources)
	{
		KS_PROFILE_FUNCTION();

		// -- Create a Program Object --
		GLuint program = glCreateProgram();

		// -- Vector for the Shader Parts (vertex sh, pixel sh, ...)
		std::vector<GLenum> gl_shader_IDs;
		gl_shader_IDs.reserve(shader_sources.size());
		KS_ENGINE_ASSERT(shader_sources.size() <= 2, "Only 2 shader types are supported currently!");
		// Instead, an array can be used (much better): std::array<GLenum, 2> glShaderIDs;
		// int glShaderIDIndex = 0; // In this case, the gl_shader_IDs.push_back() should be substituted by a glShaderIDs[glShaderIDIndex++] = shader; (downwards)


		for (auto&& [key, value] : shader_sources)
		{
			GLenum type = key;
			const std::string& source = value;

			// -- Create empty Shader handle --
			GLuint shader = glCreateShader(type);

			// -- Send Shader source code to OGL --
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* GL_shader_source = source.c_str();
			glShaderSource(shader, 1, &GL_shader_source, 0);

			// -- Compile Shader --
			glCompileShader(shader);

			// -- Check Errors & Log if Any --
			GLint is_compiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
			if (is_compiled == GL_FALSE)
			{
				GLint max_length = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

				// -- max_length includes NULL character --
				std::vector<GLchar> info_log(max_length);
				glGetShaderInfoLog(shader, max_length, &max_length, &info_log[0]);

				// -- We don't need the shader anymore --
				glDeleteShader(shader);

				// -- InfoLog to print error & assert --
				KS_ENGINE_CRITICAL("{0} Shader Compilation Error: {1}", StringFromShaderType(type), info_log.data());
				KS_ENGINE_ASSERT(false, "Shader Compilation Failure!");
				break;
			}

			// -- Attach Shaders to Program --
			glAttachShader(program, shader);
			gl_shader_IDs.push_back(shader);
		}

		// -- Set ShaderID to Program Created --
		m_ShaderID = program;

		// -- Compilation Successful, Link Program --
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint is_linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&is_linked);
		if (is_linked == GL_FALSE)
		{
			// -- Same as Before --
			GLint max_length = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

			std::vector<GLchar> info_log(max_length);
			glGetProgramInfoLog(program, max_length, &max_length, &info_log[0]);
			glDeleteProgram(program);

			// -- Don't leak Shaders --
			for(auto id : gl_shader_IDs)
				glDeleteShader(id);

			// -- Print error & assert --
			KS_ENGINE_CRITICAL("Shader Linking Error: {0}", info_log.data());
			KS_ENGINE_ASSERT(false, "Shader Program Link Failure!");
			return;
		}

		// -- Detach Shaders after successful Linkage --
		for (auto id : gl_shader_IDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
	}


	
	// ----------------------- Uniforms -------------------------------------------------------------------
	void OGLShader::SetUFloat(const std::string& name, float value)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformFloat(name, value);
	}

	void OGLShader::SetUFloat3(const std::string& name, const glm::vec3& value)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformFloat3(name, value);
	}

	void OGLShader::SetUFloat4(const std::string& name, const glm::vec4& value)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformFloat4(name, value);
	}

	void OGLShader::SetUMat4(const std::string& name, const glm::mat4& value)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformMat4(name, value);
	}
	
	void OGLShader::SetUInt(const std::string& name, int value)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformInt(name, value);
	}

	void OGLShader::SetUIntArray(const std::string& name, int* values_array, uint size)
	{
		KS_PROFILE_FUNCTION();
		UploadUniformIntArray(name, values_array, size);
	}



	// ----------------------- Uniforms Upload ------------------------------------------------------------
	void OGLShader::UploadUniformInt(const std::string& name, const int& value)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1i(loc, value);
	}

	void OGLShader::UploadUniformIntArray(const std::string& name, const int* values_array, uint size)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1iv(loc, size, values_array);
	}

	void OGLShader::UploadUniformFloat(const std::string& name, const float& value)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1f(loc, value);
	}

	void OGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform2f(loc, value.x, value.y);
	}

	void OGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform3f(loc, value.x, value.y, value.z);
	}

	void OGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform4f(loc, value.x, value.y, value.z, value.w);
	}

	void OGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint loc = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}
