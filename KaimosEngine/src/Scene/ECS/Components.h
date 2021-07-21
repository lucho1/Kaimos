#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include "Core/Resources/ResourceManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/Cameras/Camera.h"
#include "Renderer/Resources/Mesh.h"
#include "ScriptableEntity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


namespace Kaimos {

	// ---- TAG COMPONENT ------------------------------------------
	struct TagComponent
	{
		// --- Variables ---
		std::string Tag = "Unnamed";
		bool Rename = false;

		// --- Constructors ---
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};



	// ---- TRANSFORM COMPONENT ------------------------------------
	struct TransformComponent
	{
		// --- Variables ---
		bool EntityActive = true;
		glm::vec3 Translation = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		// --- Constructors ---
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TransformComponent(const glm::vec3& pos)												: Translation(pos)								{}
		TransformComponent(const glm::vec3& pos, const glm::vec3& rot)							: Translation(pos), Rotation(rot)				{}
		TransformComponent(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale)	: Translation(pos), Rotation(rot), Scale(scale)	{}

		// --- Getters ---
		const glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}

		const glm::vec3 GetUpVector()				const { return glm::rotate(glm::quat(Rotation), glm::vec3(0.0f, 1.0f, 0.0f)); }
		const glm::vec3 GetRightVector()			const { return glm::rotate(glm::quat(Rotation), glm::vec3(1.0f, 0.0f, 0.0f)); }
		const glm::vec3 GetForwardVector()			const { return glm::rotate(glm::quat(Rotation), glm::vec3(0.0f, 0.0f, -1.0f)); }
	};



	// ---- CAMERA COMPONENT ---------------------------------------
	struct CameraComponent
	{
		// --- Variables ---
		Kaimos::Camera Camera = {};
		bool Primary = false;
		bool FixedAspectRatio = false;

		// --- Constructors ---
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};



	// ---- SCRIPT COMPONENT ---------------------------------------
	struct NativeScriptComponent
	{
		// --- Variables ---
		ScriptableEntity* EntityInstance = nullptr;

		// --- Functions ---
		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* script_comp) { delete script_comp->EntityInstance; script_comp->EntityInstance = nullptr; };
		}
	};



	// ---- SPRITE COMPONENT ---------------------------------------
	struct SpriteRendererComponent
	{
		// --- Variables ---
		uint SpriteMaterialID = 0;
		QuadVertex QuadVertices[4];
		bool PositionTimed = false, NormalsTimed = false, TexCoordsTimed = false;


		// --- Constructors ---
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;


		// --- Material Functions ---
		void SetMaterial(uint material_id)
		{
			if (material_id == SpriteMaterialID || Renderer::GetMaterialIfExists(material_id) == 0)
				return;

			SpriteMaterialID = material_id;
			UpdateVertices();
		}

		void RemoveMaterial()
		{
			SpriteMaterialID = Renderer::GetDefaultMaterialID();
			SetupVertices();
		}


		// --- Vertices Functions ---
		void SetupVertices()
		{
			QuadVertices[0].Pos =	{ -0.5f, -0.5f, 0.0f };
			QuadVertices[1].Pos =	{ 0.5f, -0.5f, 0.0f };
			QuadVertices[2].Pos =	{ 0.5f,  0.5f, 0.0f };
			QuadVertices[3].Pos =	{ -0.5f,  0.5f, 0.0f };

			QuadVertices[0].TexCoord =	{ 0.0f, 0.0f };
			QuadVertices[1].TexCoord =	{ 1.0f, 0.0f };
			QuadVertices[2].TexCoord =	{ 1.0f, 1.0f };
			QuadVertices[3].TexCoord =	{ 0.0f, 1.0f };

			QuadVertices[0].Normal = QuadVertices[1].Normal = QuadVertices[2].Normal = QuadVertices[3].Normal = { 0.0f,  0.0f, -1.0f };
		}

		void UpdateVertices()
		{
			// Get Mat
			Ref<Material> material = Renderer::GetMaterial(SpriteMaterialID);
			if (!material)
				return;

			SetupVertices();
			for (QuadVertex& vertex : QuadVertices)
			{
				material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::POSITION, glm::vec4(vertex.Pos, 0.0f));
				material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::NORMAL, glm::vec4(vertex.Normal, 0.0f));
				material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::TEX_COORDS, glm::vec4(vertex.TexCoord, 0.0f, 0.0f));

				vertex.Pos = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::POSITION);
				vertex.Normal = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::NORMAL);
				vertex.TexCoord = material->GetVertexAttributeResult<glm::vec2>(MaterialEditor::VertexParameterNodeType::TEX_COORDS);

				PositionTimed = material->IsVertexAttributeTimed(MaterialEditor::VertexParameterNodeType::POSITION);
				NormalsTimed = material->IsVertexAttributeTimed(MaterialEditor::VertexParameterNodeType::NORMAL);
				TexCoordsTimed = material->IsVertexAttributeTimed(MaterialEditor::VertexParameterNodeType::TEX_COORDS);
			}
		}

		void UpdateTimedVertices()
		{
			// Get Mesh & Mat
			Ref<Material> material = Renderer::GetMaterial(SpriteMaterialID);
			if (!material)
				return;

			SetupVertices();
			for (QuadVertex& vertex : QuadVertices)
			{
				if (PositionTimed)
				{
					material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::POSITION, glm::vec4(vertex.Pos, 0.0f));
					vertex.Pos = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::POSITION);
				}

				if (NormalsTimed)
				{
					material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::NORMAL, glm::vec4(vertex.Normal, 0.0f));
					vertex.Normal = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::NORMAL);
				}

				if (TexCoordsTimed)
				{
					material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::TEX_COORDS, glm::vec4(vertex.TexCoord, 0.0f, 0.0f));
					vertex.TexCoord = material->GetVertexAttributeResult<glm::vec2>(MaterialEditor::VertexParameterNodeType::TEX_COORDS);
				}
			}
		}
	};



	// ---- MESH COMPONENT -----------------------------------------
	struct MeshRendererComponent
	{
		// --- Variables ---
		uint MaterialID = 0, MeshID = 0;
		std::vector<Vertex> ModifiedVertices;
		bool PositionTimed = false, NormalsTimed = false, TexCoordsTimed = false;


		// --- Constructors ---
		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;


		// --- Material Functions ---
		void SetMaterial(uint material_id)
		{
			if (material_id == MaterialID || Renderer::GetMaterialIfExists(material_id) == 0)
				return;

			MaterialID = material_id;
			UpdateModifiedVertices();
		}

		void RemoveMaterial()
		{
			MaterialID = Renderer::GetDefaultMaterialID();
			UpdateModifiedVertices();
		}


		// --- Mesh Functions ---
		void SetMesh(uint mesh_id)
		{
			if (mesh_id == MeshID || !Resources::ResourceManager::MeshExists(mesh_id))
				return;

			MeshID = mesh_id;
			UpdateModifiedVertices();
		}

		void RemoveMesh()
		{
			MeshID = 0;
			ModifiedVertices.clear();
			PositionTimed = NormalsTimed = TexCoordsTimed = false;
		}
		

		// --- Vertices Functions ---
		void UpdateModifiedVertices()
		{
			// Get Mesh & Mat
			Ref<Material> material = Renderer::GetMaterial(MaterialID);
			Ref<Mesh> mesh = Resources::ResourceManager::GetMesh(MeshID);

			if (!material || !mesh)
			{
				ModifiedVertices.clear();
				return;
			}

			ModifiedVertices = mesh->GetVertices();
			for (Vertex& vertex : ModifiedVertices)
			{
				material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::POSITION, glm::vec4(vertex.Pos, 0.0f));
				material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::NORMAL, glm::vec4(vertex.Normal, 0.0f));
				material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::TEX_COORDS, glm::vec4(vertex.TexCoord, 0.0f, 0.0f));

				vertex.Pos = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::POSITION);
				vertex.Normal = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::NORMAL);
				vertex.TexCoord = material->GetVertexAttributeResult<glm::vec2>(MaterialEditor::VertexParameterNodeType::TEX_COORDS);

				PositionTimed = material->IsVertexAttributeTimed(MaterialEditor::VertexParameterNodeType::POSITION);
				NormalsTimed = material->IsVertexAttributeTimed(MaterialEditor::VertexParameterNodeType::NORMAL);
				TexCoordsTimed = material->IsVertexAttributeTimed(MaterialEditor::VertexParameterNodeType::TEX_COORDS);
			}
		}

		void UpdateTimedVertices()
		{
			// Get Mesh & Mat
			Ref<Material> material = Renderer::GetMaterial(MaterialID);
			Ref<Mesh> mesh = Resources::ResourceManager::GetMesh(MeshID);

			if (!material || !mesh)
			{
				ModifiedVertices.clear();
				return;
			}

			ModifiedVertices = mesh->GetVertices();
			for (Vertex& vertex : ModifiedVertices)
			{
				if (PositionTimed)
				{
					material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::POSITION, glm::vec4(vertex.Pos, 0.0f));
					vertex.Pos = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::POSITION);
				}

				if (NormalsTimed)
				{
					material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::NORMAL, glm::vec4(vertex.Normal, 0.0f));
					vertex.Normal = material->GetVertexAttributeResult<glm::vec3>(MaterialEditor::VertexParameterNodeType::NORMAL);
				}

				if (TexCoordsTimed)
				{
					material->UpdateVertexParameter(MaterialEditor::VertexParameterNodeType::TEX_COORDS, glm::vec4(vertex.TexCoord, 0.0f, 0.0f));
					vertex.TexCoord = material->GetVertexAttributeResult<glm::vec2>(MaterialEditor::VertexParameterNodeType::TEX_COORDS);
				}
			}
		}
	};
}

#endif //_COMPONENTS_H_
