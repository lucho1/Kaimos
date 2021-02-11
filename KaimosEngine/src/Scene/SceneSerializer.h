#ifndef _SCENE_SERIALIZER_H_
#define _SCENE_SERIALIZER_H_

#include "Scene.h"

namespace Kaimos {

	class SceneSerializer
	{
	public:

		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);

	private:
		Ref<Scene> m_Scene;
	};

}
#endif //_SCENE_SERIALIZER_H_