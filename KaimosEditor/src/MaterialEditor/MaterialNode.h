#ifndef _MATERIALNODE_H_
#define _MATERIALNODE_H_

#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"
#include "Scene/ECS/Components.h"


namespace Kaimos {

	class MaterialNodePin;


	// ---- Base Material Node ----
	class MaterialNode
	{
	public:

		// --- Public Class Methods ---
		MaterialNode(uint id, const std::string& name) : m_ID(id), m_Name(name) {}
		~MaterialNode();

		virtual void DrawNodeUI();


		// --- Public Material Node Methods ---
		void AddPin(bool input, float default_value = 1.0f);
		void AddPin(bool input, Ref<MaterialNodePin>& pin);

		MaterialNodePin* FindInputPin(uint pinID);

	public:

		// --- Getters ---
		uint GetID()												const { return m_ID; }
		const std::string& GetName()								const { return m_Name; }

		MaterialNodePin* GetOutputPin()								const { return m_NodeOutputPin.get(); }
		const std::vector<Ref<MaterialNodePin>>* GetInputPins()		const { return &m_NodeInputPins; }

	protected:

		// --- Variables ---
		uint m_ID = 0;
		std::string m_Name = "unnamed";

		std::vector<Ref<MaterialNodePin>> m_NodeInputPins;
		Ref<MaterialNodePin> m_NodeOutputPin = nullptr;
	};



	// ---- Main Material Node ----
	class MainMaterialNode : public MaterialNode
	{
	public:

		// --- Public Class Methods ---
		MainMaterialNode();
		~MainMaterialNode();

		virtual void DrawNodeUI() override;


		// --- Public Main Material Node Methods ---
		void DettachMaterial();
		void AttachMaterial(SpriteRendererComponent* sprite_component);

		bool HasMaterialAttached() const { return m_AttachedMaterial != nullptr; }
			
	private:

		// --- Protected Variables ---
		mutable SpriteRendererComponent* m_AttachedMaterial = nullptr;
		Ref<MaterialNodePin> m_TextureTilingPin = nullptr;
		Ref<MaterialNodePin> m_TextureOffsetPinX = nullptr;
		Ref<MaterialNodePin> m_TextureOffsetPinY = nullptr;
	};
}

#endif //_MATERIALNODE_H_
