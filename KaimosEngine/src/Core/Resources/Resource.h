#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"

namespace Kaimos::Resources {

	enum class RESOURCE_TYPE { NONE = 0, MODEL };
	enum class RESOURCES_FORMATS { NONE = 0, FBX, OBJ }; // Add PNG, JPG, ... When supporting textures and materials

	static RESOURCES_FORMATS GetFormatFromExtension(const std::string& extension)
	{
		if (extension == ".FBX" || extension == ".fbx")
			return RESOURCES_FORMATS::FBX;
		if(extension == ".OBJ" || extension == ".obj")
			return RESOURCES_FORMATS::OBJ;
		else
		{
			KS_ENGINE_WARN("Invalid Extension Passed! ({0})", extension.c_str());
			return RESOURCES_FORMATS::NONE;
		}
	}

	static bool IsExtensionValid(const std::string& extension)
	{
		return GetFormatFromExtension(extension) != RESOURCES_FORMATS::NONE;
	}


	class Resource
	{
	protected:

		// --- Public Class Methods ---
		Resource(RESOURCE_TYPE type, const std::string& filepath, uint id) : m_Type(type)
		{
			m_ID = (id == 0 ? (uint)Kaimos::Random::GetRandomInt() : id);

			// -- Check if it's within project folders --
			size_t assets_pos = filepath.find("assets");
			if (assets_pos != std::string::npos)
				m_Filepath = filepath.substr(assets_pos, filepath.size());
			else
			{
				KS_ENGINE_ERROR("Loading an out-of-project Resource!");
			}

			// -- Set Paths --
			std::filesystem::path path = filepath;
			if (std::filesystem::exists(path))
			{
				if (path.has_filename())
					m_Filename = path.filename().string();

				if (path.has_stem())
					m_Name = path.stem().string();

				if (path.is_absolute())
					m_AbsolutFilepath = path.string();
				else
					m_AbsolutFilepath = std::filesystem::current_path().string() + path.string();

				if (path.has_extension())
				{
					m_Extension = path.extension().string();
					m_Format = GetFormatFromExtension(m_Extension);
				}
			}
			else
			{
				KS_ENGINE_ERROR("Unexisting Path Loading Resource: {0}", filepath.c_str());
			}
		}

		~Resource() = default;

	public:

		// --- Getters ---
		const std::string& GetAbsFilepath()	const { return m_AbsolutFilepath; }
		const std::string& GetFilepath()	const { return m_Filepath; }
		const std::string& GetFilename()	const { return m_Filename; }
		const std::string& GetExtension()	const { return m_Extension; }
		const std::string& GetName()		const { return m_Name; }

		uint GetID()						const { return m_ID; }
		RESOURCE_TYPE GetType()				const { return m_Type; }

	private:

		// --- Private Variables ---
		mutable std::string m_AbsolutFilepath = "Unpathed";
		mutable std::string m_Filepath = "Unpathed";
		mutable std::string m_Filename = "Unnamed";
		mutable std::string m_Extension = "Unformatted";
		mutable std::string m_Name = "Unnamed";

		uint m_ID = 0;
		RESOURCE_TYPE m_Type = RESOURCE_TYPE::NONE;
		RESOURCES_FORMATS m_Format = RESOURCES_FORMATS::NONE;
	};
}

#endif //_RESOURCE_H_
