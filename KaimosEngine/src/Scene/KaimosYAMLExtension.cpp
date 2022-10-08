#include "kspch.h"
#include "KaimosYAMLExtension.h"


// ---------------------------------------------------------------------------------------------------
// ----------------------- YAML Additions Static Methods ---------------------------------------------
namespace YAML {


	Node convert<glm::vec2>::encode(const glm::vec2& vec)
	{
		Node node;
		node.push_back(vec.x);
		node.push_back(vec.y);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	bool convert<glm::vec2>::decode(const Node& node, glm::vec2& vec)
	{
		if (!node.IsSequence() || node.size() != 2)
			return false;

		vec.x = node[0].as<float>();
		vec.y = node[1].as<float>();
		return true;
	}


	Node convert<glm::vec3>::encode(const glm::vec3& vec)
	{
		Node node;
		node.push_back(vec.x);
		node.push_back(vec.y);
		node.push_back(vec.z);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	bool convert<glm::vec3>::decode(const Node& node, glm::vec3& vec)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		vec.x = node[0].as<float>();
		vec.y = node[1].as<float>();
		vec.z = node[2].as<float>();
		return true;
	}


	Node convert<glm::vec4>::encode(const glm::vec4& vec)
	{
		Node node;
		node.push_back(vec.x);
		node.push_back(vec.y);
		node.push_back(vec.z);
		node.push_back(vec.w);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	bool convert<glm::vec4>::decode(const Node& node, glm::vec4& vec)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		vec.x = node[0].as<float>();
		vec.y = node[1].as<float>();
		vec.z = node[2].as<float>();
		vec.w = node[3].as<float>();
		return true;
	}
}
// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------



namespace Kaimos {

	// ----------------------- YAML Operators Methods ----------------------------------------------------
	YAML::Emitter& operator<<(YAML::Emitter& output, const glm::vec2& vec)
	{
		output << YAML::Flow;
		output << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
		return output;
	}

	YAML::Emitter& operator<<(YAML::Emitter& output, const glm::vec3& vec)
	{
		// Flow, instead of serializing each value separatedly (x; y; z) will put them together as [x, y, z]
		output << YAML::Flow;
		output << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return output;
	}

	YAML::Emitter& operator<<(YAML::Emitter& output, const glm::vec4& vec)
	{
		output << YAML::Flow;
		output << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return output;
	}
}
