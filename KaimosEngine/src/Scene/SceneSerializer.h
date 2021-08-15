#ifndef _SCENE_SERIALIZER_H_
#define _SCENE_SERIALIZER_H_

#include "Scene.h"

namespace Kaimos {

	class CameraController;
	class SceneSerializer
	{
	public:

		// --- Public Class Methods ---
		SceneSerializer(const Ref<Scene>& scene) : m_Scene(scene) {}

		// --- Public Serialization Methods ---
		void Serialize(const std::string& filepath, const CameraController& scene_cam) const;
		void SerializeRuntime(const std::string& filepath)		const { KS_FATAL_ERROR("Method Not Implemented"); }

		// --- Public Deserialization Methods ---
		bool Deserialize(const std::string& filepath, CameraController& scene_cam) const;
		bool DeserializeRuntime(const std::string& filepath)	const { KS_FATAL_ERROR("Method Not Implemented"); }

	private:
		Ref<Scene> m_Scene;
	};
}

#endif //_SCENE_SERIALIZER_H_
