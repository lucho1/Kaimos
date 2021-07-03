#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"


namespace Kaimos
{
	enum class RESOURCE_TYPE { NONE = 0, MODEL };

	class Resource
	{
	public:

		const std::string& GetFilepath()	const { return m_Filepath; }
		const std::string& GetFilename()	const { return m_Filename; }
		const std::string& GetExtension()	const { return m_Extension; }
		const std::string& GetName()		const { return m_Name; }

		uint GetID()						const { return m_ID; }
		RESOURCE_TYPE GetType()				const { return m_Type; }

	protected:

		Resource(RESOURCE_TYPE type, const std::string& filepath, uint id) : m_Type(type)
		{
			SetFilesystemStrings(filepath);
			if(id == 0)
				m_ID = (uint)Kaimos::Random::GetRandomInt();
		}

		~Resource() = default;

	private:

		void SetFilesystemStrings(const std::string& filepath)
		{
			size_t assets_pos = filepath.find("assets");
			if (assets_pos != std::string::npos)
				m_Filepath = filepath.substr(assets_pos, filepath.size());
			else
			{
				KS_ENGINE_ERROR("Loading an out-of-project Resource!");
				return;
			}

			m_AbsolutFilepath = filepath;
			std::filesystem::path path = filepath;
			if (std::filesystem::exists(path))
			{
				if (path.has_filename())
					m_Filename = path.filename().string();

				if (path.has_extension())
					m_Extension = path.extension().string();

				if (path.has_stem())
					m_Name = path.stem().string();
			}
		}

	private:

		std::string m_AbsolutFilepath = "Unpathed";
		std::string m_Filepath = "Unpathed";
		std::string m_Filename = "Unnamed";
		std::string m_Extension = "Unformatted";
		std::string m_Name = "Unnamed";

		uint m_ID = 0;
		RESOURCE_TYPE m_Type = RESOURCE_TYPE::NONE;
	};
}

#endif //_RESOURCE_H_
