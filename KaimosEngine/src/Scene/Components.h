#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include <glm/glm.hpp>

namespace Kaimos {

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::mat4 Transform = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform) : Transform(transform) {}

		operator const glm::mat4&() const { return Transform; } // This allows to use TransformComponent as a matrix (overloading magic), it implicitly makes the cast
		operator glm::mat4&() { return Transform; }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color = glm::vec4(1.0f);

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
	};

	struct CameraComponent
	{
		Kaimos::SceneCamera Camera;
		bool Primary = true; // TODO: Move it to scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* EntityInstance = nullptr;

		std::function<void()> InstantiateFunction;
		std::function<void()> DestroyInstanceFunction;
		
		std::function<void(ScriptableEntity*)> OnCreateFunction;
		std::function<void(ScriptableEntity*, Timestep)> OnUpdateFunction;
		std::function<void(ScriptableEntity*)> OnDestroyFunction;

		template<typename T>
		void Bind()
		{
			InstantiateFunction = [&]() { EntityInstance = new T(); };
			DestroyInstanceFunction = [&]() { delete (T*)EntityInstance; EntityInstance = nullptr; };

			OnCreateFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnCreate(); };
			OnUpdateFunction = [](ScriptableEntity* instance, Timestep dt) { ((T*)instance)->OnUpdate(dt); };
			OnDestroyFunction = [](ScriptableEntity* instance) { ((T*)instance)->OnDestroy(); };
		}
	};
}

#endif // _COMPONENTS_H_