#ifndef _MATERIALNODE_H_
#define _MATERIALNODE_H_

#include "NodeUtils.h"
#include "Core/Core.h"
#include "Core/Utils/Maths/RandomGenerator.h"

namespace Kaimos { class Material; }


namespace Kaimos::MaterialEditor {

	// ---- Forward Declarations & Enums ----
	class NodePin;
	class NodeOutputPin;
	class NodeInputPin;
	enum class MaterialNodeType	{ NONE, MAIN, OPERATION, CONSTANT };
	enum class PinDataType		{ NONE, FLOAT, INT, VEC2, VEC4 };


	// ---- Base Material Node ----
	class MaterialNode
	{
	protected:

		// --- Protected Class Methods ---
		MaterialNode(const std::string& name, MaterialNodeType type)
			: m_Name(name), m_Type(type), m_ID((uint)Kaimos::Random::GetRandomInt()) {}
		

	public:

		// --- Public Class Methods ---
		~MaterialNode();

		virtual void DrawNodeUI();
		NodePin* FindPinInNode(uint pinID);

	protected:

		// --- Public Material Node Methods ---
		NodeInputPin* FindInputPin(uint pinID);
		void AddPin(bool input, PinDataType pin_type, const std::string& name, float default_value = 1.0f);		

		float* GetInputValue(uint input_index);

	public:

		// --- Getters ---
		uint GetID()												const { return m_ID; }
		MaterialNodeType GetType()									const { return m_Type; }
		const std::string& GetName()								const { return m_Name; }

		// Defined in child classes according to what each node type does
		virtual float* CalculateNodeResult() = 0;

	protected:

		// --- Variables ---
		uint m_ID = 0;
		std::string m_Name = "unnamed";
		MaterialNodeType m_Type = MaterialNodeType::NONE;

		std::vector<Ref<NodeInputPin>> m_NodeInputPins;
		Ref<NodeOutputPin> m_NodeOutputPin = nullptr;
	};



	// ---- Main Material Node ----
	class MainMaterialNode : public MaterialNode
	{
	public:

		// --- Public Class Methods ---
		MainMaterialNode(Material* attached_material);
		~MainMaterialNode();

		virtual void DrawNodeUI() override;
		void SyncValuesWithMaterial();
			
	private:

		virtual float* CalculateNodeResult() override { return nullptr; }

		// --- Variables ---
		Material* m_AttachedMaterial = nullptr;
		Ref<NodeInputPin> m_TextureTilingPin = nullptr;
		Ref<NodeInputPin> m_TextureOffsetPin = nullptr;
		Ref<NodeInputPin> m_ColorPin = nullptr;
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
