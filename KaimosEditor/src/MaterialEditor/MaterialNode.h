#ifndef _MATERIALNODE_H_
#define _MATERIALNODE_H_

#include "Core/Core.h"
#include "NodeUtils.h"
#include "Core/Utils/Maths/RandomGenerator.h"
#include "Scene/ECS/Components.h"


namespace Kaimos::MaterialEditor {

	// ---- Forward Declarations & Enums ----
	class MaterialNodePin;
	enum class MaterialNodeType	{ NONE, MAIN, OPERATION, CONSTANT };
	enum class PinDataType		{ NONE, FLOAT, INT, VEC2, VEC4 };


	// ---- Base Material Node ----
	class MaterialNode
	{
	protected:

		// --- Protected Class Methods ---
		MaterialNode(const std::string& name, MaterialNodeType type) : m_Name(name), m_Type(type), m_ID((uint)Kaimos::Random::GetRandomInt()) {}

		// Defined in child classes according to what each node type does
		virtual float* CalculateNodeResult() = 0;

	public:

		// --- Public Class Methods ---
		~MaterialNode();
		virtual void DrawNodeUI();

		// --- Public Material Node Methods ---
		MaterialNodePin* FindInputPin(uint pinID);
		void AddPin(bool input, PinDataType pin_type, const std::string& name, float default_value = 1.0f);
		void AddPin(bool input, Ref<MaterialNodePin>& pin);


		// --- Node Result Methods ---
		template<typename T>
		T& GetOutputResult()
		{
			if (m_NodeOutputPin)
				return NodeUtils::GetDataFromType<T>(m_NodeOutputPin->GetValue(), m_NodeOutputPin->GetType());

			KS_ERROR_AND_ASSERT("Invalid GetOutput() Operation in MaterialNode");
			return *(T*)nullptr;
		}

		float* GetInputValue(uint input_index);


	public:

		// --- Getters ---
		uint GetID()												const { return m_ID; }
		MaterialNodeType GetType()									const { return m_Type; }
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

		virtual float* CalculateNodeResult() override { return nullptr; }

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
		ConstantMaterialNode(ConstantNodeType constant_type);

	private:

		virtual float* CalculateNodeResult() override;
		ConstantNodeType m_ConstantType = ConstantNodeType::NONE;
	};



	// ---- Operation Node ----
	enum class OperationNodeType { NONE, ADDITION, MULTIPLICATION };
	class OperationMaterialNode : public MaterialNode
	{
	public:
		OperationMaterialNode(OperationNodeType operation_type, PinDataType operation_data_type);

	private:

		virtual float* CalculateNodeResult() override;
		float* ProcessOperation(PinDataType data_type, const float* a, const float* b) const;

		OperationNodeType m_OperationType = OperationNodeType::NONE;
	};
}

#endif //_MATERIALNODE_H_
