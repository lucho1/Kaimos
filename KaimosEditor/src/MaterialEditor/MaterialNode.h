#ifndef _MATERIALNODE_H_
#define _MATERIALNODE_H_

#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"
#include "Scene/ECS/Components.h"


namespace Kaimos::MaterialEditor {

	enum class MaterialNodeType { NONE, MAIN, FLOAT_DUMMY, OPERATION, CONSTANT };
	class MaterialNodePin;
	enum class PinDataType;


	// ---- Base Material Node ----
	class MaterialNode
	{
	public:

		// --- Public Class Methods ---
		MaterialNode(uint id, const std::string& name, MaterialNodeType type) : m_ID(id), m_Name(name), m_Type(type) {}
		~MaterialNode();

		virtual void DrawNodeUI();


		// --- Public Material Node Methods ---
		MaterialNodePin* FindInputPin(uint pinID);
		void AddPin(bool input, PinDataType pin_type, const std::string& name, float default_value = 1.0f);
		void AddPin(bool input, Ref<MaterialNodePin>& pin);

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

		MaterialNodeType m_Type = MaterialNodeType::NONE;

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

		// --- Variables ---
		mutable SpriteRendererComponent* m_AttachedMaterial = nullptr;
		Ref<MaterialNodePin> m_TextureTilingPin = nullptr;
		Ref<MaterialNodePin> m_TextureOffsetPinX = nullptr;
		Ref<MaterialNodePin> m_TextureOffsetPinY = nullptr;
	};



	// ---- Constant Node ----
	enum class ConstantNodeType { NONE, TCOORDS, DELTATIME };

	class ConstantMaterialNode : public MaterialNode
	{
	public:

		// --- Public Class Methods ---
		ConstantMaterialNode(ConstantNodeType constant_type);

	private:

		// --- Variables ---
		ConstantNodeType m_ConstantType = ConstantNodeType::NONE;
	};



	// ---- Operation Node ----
	enum class OperationNodeType { NONE, ADDITION, MULTIPLICATION };

	class OperationMaterialNode : public MaterialNode
	{
	public:

		// --- Public Class Methods ---
		OperationMaterialNode(OperationNodeType operation_type, PinDataType operation_data_type);

	private:

		// --- Variables ---
		OperationNodeType m_OperationType = OperationNodeType::NONE;
	};
}

#endif //_MATERIALNODE_H_
