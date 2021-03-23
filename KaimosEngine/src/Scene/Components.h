#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include "Renderer/Resources/Texture.h"
#include "SceneCamera.h"
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
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color = glm::vec4(1.0f);
		Ref<Texture2D> SpriteTexture = nullptr;
		std::string TextureFilepath = "";
		float TextureTiling = 1.0f;
		glm::vec2 TextureUVOffset = glm::vec2(0.0f);

		inline void RemoveTexture()
		{
			SpriteTexture = nullptr;
			TextureFilepath.clear();
		}

		void SetTexture(const std::string& filepath)
		{
			Ref<Texture2D> texture = Texture2D::Create(filepath);
			if (texture)
			{
				SpriteTexture = texture;
				TextureFilepath = filepath;
			}
			else
				KS_ENGINE_WARN("Couldn't Load Texture from '{0}'", filepath.c_str());
		}

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
		SpriteRendererComponent(const glm::vec4& color, Ref<Texture2D> texture) : Color(color), SpriteTexture(texture) {}
		SpriteRendererComponent(const glm::vec4& color, const std::string& file) : Color(color) { SetTexture(file); }
	};

	struct CameraComponent
	{
		Kaimos::SceneCamera Camera = {};
		bool Primary = true; // TODO: Move it to scene on Camera Rework!
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
