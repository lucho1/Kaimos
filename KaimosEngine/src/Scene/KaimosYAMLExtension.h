#ifndef _KAIMOSYAMLEXT_
#define _KAIMOSYAMLEXT_

#include <yaml-cpp/yaml.h>

// ---------------------------------------------------------------------------------------------------
// ----------------------- YAML Additions Static Methods ---------------------------------------------
namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& vec);
		static bool decode(const Node& node, glm::vec2& vec);
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& vec);
		static bool decode(const Node& node, glm::vec3& vec);
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& vec);
		static bool decode(const Node& node, glm::vec4& vec);
	};
}
// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------


namespace Kaimos {

	// ----------------------- YAML Operators Methods ----------------------------------------------------
	YAML::Emitter& operator<<(YAML::Emitter& output, const glm::vec2& vec);
	YAML::Emitter& operator<<(YAML::Emitter& output, const glm::vec3& vec);
	YAML::Emitter& operator<<(YAML::Emitter& output, const glm::vec4& vec);
}

#endif _KAIMOSYAMLEXT_
