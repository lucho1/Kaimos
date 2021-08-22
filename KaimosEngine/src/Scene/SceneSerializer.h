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
		void Serialize(const std::string& filepath) const;

		// --- Public Deserialization Methods ---
		bool Deserialize(const std::string& filepath) const;

	private:
		Ref<Scene> m_Scene;
	};
}

#endif //_SCENE_SERIALIZER_H_
