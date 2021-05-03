#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include "Renderer/Resources/Material.h"
#include "Renderer/Cameras/Camera.h"
#include "ScriptableEntity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


namespace Kaimos {

	struct TagComponent
	{
		std::string Tag = "Unnamed";
		bool Rename = false;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};

	struct TransformComponent
	{
		bool EntityActive = true;

		glm::vec3 Translation = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TransformComponent(const glm::vec3& pos)												: Translation(pos)								{}
		TransformComponent(const glm::vec3& pos, const glm::vec3& rot)							: Translation(pos), Rotation(rot)				{}
		TransformComponent(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale)	: Translation(pos), Rotation(rot), Scale(scale)	{}

		const glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}

		const glm::vec3 GetUpVector()				const { return glm::rotate(glm::quat(Rotation), glm::vec3(0.0f, 1.0f, 0.0f)); }
		const glm::vec3 GetRightVector()			const { return glm::rotate(glm::quat(Rotation), glm::vec3(1.0f, 0.0f, 0.0f)); }
		const glm::vec3 GetForwardVector()			const { return glm::rotate(glm::quat(Rotation), glm::vec3(0.0f, 0.0f, -1.0f)); }
	};

	struct SpriteRendererComponent
	{
		Ref<Material> SpriteMaterial = nullptr;

		inline void SetMaterial(Ref<Material>& material)
		{
			if (SpriteMaterial)
				RemoveMaterial();

			SpriteMaterial = material;
		}

		inline void RemoveMaterial()
		{
			SpriteMaterial = nullptr;
		}

		inline void RemoveTexture()
		{
			if(SpriteMaterial)
				SpriteMaterial->RemoveTexture();
		}

		void SetTexture(const std::string& filepath)
		{
			if(SpriteMaterial)
				SpriteMaterial->SetTexture(filepath);
		}

		SpriteRendererComponent()								= default;
		SpriteRendererComponent(const SpriteRendererComponent&)	= default;
	};

	struct CameraComponent
	{
		Kaimos::Camera Camera = {};
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* EntityInstance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);
		
		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* script_comp) { delete script_comp->EntityInstance; script_comp->EntityInstance = nullptr; };
		}
	};
}

#endif //_COMPONENTS_H_
