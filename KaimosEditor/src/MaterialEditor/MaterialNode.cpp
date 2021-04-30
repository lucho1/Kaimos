#include "kspch.h"
#include "MaterialNode.h"
#include "MaterialNodePin.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>
#include <glm/gtc/type_ptr.hpp>


namespace Kaimos::MaterialEditor {


	// ---------------------------- BASE MAT NODE ---------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	MaterialNode::~MaterialNode()
	{
		for (Ref<MaterialNodePin>& pin : m_NodeInputPins)
		{
			pin.reset();
			pin = nullptr;
		}

		m_NodeInputPins.clear();
		m_NodeOutputPin.reset();
		m_NodeOutputPin = nullptr;
	}

	void MaterialNode::DrawNodeUI()
	{
		// -- Draw Node & Header --
		ImNodes::BeginNode(m_ID);

		ImNodes::BeginNodeTitleBar();
		ImGui::Text(m_Name.c_str());
		ImNodes::EndNodeTitleBar();

		// -- Draw Output Pin --
		ImNodes::BeginOutputAttribute(m_NodeOutputPin->GetID());
		ImGui::Indent(50.0f);
		ImGui::Text(m_NodeOutputPin->GetName().c_str());
		ImNodes::EndOutputAttribute();

		ImGui::Indent(50.0f);
		m_NodeOutputPin->SetValue(CalculateNodeResult());

		glm::vec4 res = GetOutputResult<glm::vec4>();
		ImGui::Text("Value: %.1f, %.1f, %.1f, %.1f", res.x, res.y, res.z, res.w);


		// -- Draw Input Pins --
		bool set_node_draggable = true;
		for (Ref<MaterialNodePin>& pin : m_NodeInputPins)
		{
			float dummy = 0.0f;
			pin->DrawPinUI(dummy, set_node_draggable);
		}

		// -- End Node Drawing --
		ImNodes::SetNodeDraggable(m_ID, set_node_draggable);
		ImNodes::EndNode();

		// -- Draw Links --
		for (Ref<MaterialNodePin>& pin : m_NodeInputPins)
		{
			if (pin->GetOutputPinLinked())
				ImNodes::Link(pin->GetID(), pin->GetID(), pin->GetOutputPinLinked()->GetID());	// Links have the same ID than its input pin
		}
	}



	// ----------------------- Public Material Node Methods -----------------------------------------------
	MaterialNodePin* MaterialNode::FindInputPin(uint pinID)
	{
		for (uint i = 0; i < m_NodeInputPins.size(); ++i)
			if (m_NodeInputPins[i]->GetID() == pinID)
				return m_NodeInputPins[i].get();

		return nullptr;
	}

	void MaterialNode::AddPin(bool input, PinDataType pin_type, const std::string& name, float default_value)
	{
		if (input)
			m_NodeInputPins.push_back(CreateRef<MaterialNodePin>(this, pin_type, name, default_value));
		else if (!m_NodeOutputPin)
			m_NodeOutputPin = CreateRef<MaterialNodePin>(this, pin_type, name, default_value);
	}

	void MaterialNode::AddPin(bool input, Ref<MaterialNodePin>& pin)
	{
		if (input)
			m_NodeInputPins.push_back(pin);
		else if (!m_NodeOutputPin)
			m_NodeOutputPin = pin;
	}

	float* MaterialNode::GetInputValue(uint input_index)
	{
		if (m_NodeInputPins[input_index]->GetOutputPinLinked())
			return m_NodeInputPins[input_index]->GetOutputPinLinked()->GetNode()->CalculateNodeResult();

		return &m_NodeInputPins[input_index]->GetValue()[0];
	}



	// ---------------------------- MAIN MAT NODE ---------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	MainMaterialNode::MainMaterialNode() : MaterialNode("Main Node", MaterialNodeType::MAIN)
	{
		m_TextureTilingPin = CreateRef<MaterialNodePin>(this, PinDataType::FLOAT, "Texture Tiling", 1.0f);
		m_TextureOffsetPinX = CreateRef<MaterialNodePin>(this, PinDataType::VEC2, "Texture Offset X", 0.0f);
		m_TextureOffsetPinY = CreateRef<MaterialNodePin>(this, PinDataType::VEC2, "Texture Offset Y", 0.0f);

		AddPin(true, m_TextureTilingPin);
		AddPin(true, m_TextureOffsetPinX);
		AddPin(true, m_TextureOffsetPinY);
	}

	MainMaterialNode::~MainMaterialNode()
	{
		m_TextureTilingPin.reset();
		m_TextureOffsetPinX.reset();
		m_TextureOffsetPinY.reset();
		m_TextureOffsetPinX = m_TextureOffsetPinY = m_TextureTilingPin = nullptr;
	}


	void MainMaterialNode::DrawNodeUI()
	{
		// -- Draw Node & Header --
		ImNodes::BeginNode(m_ID);

		ImNodes::BeginNodeTitleBar();
		ImGui::Text(m_Name.c_str());
		ImNodes::EndNodeTitleBar();

		// -- Draw Input Pins --
		bool set_node_draggable = true;
		m_TextureTilingPin->DrawPinUI(m_AttachedMaterial->TextureTiling, set_node_draggable);
		m_TextureOffsetPinX->DrawPinUI(m_AttachedMaterial->TextureUVOffset.x, set_node_draggable);
		m_TextureOffsetPinY->DrawPinUI(m_AttachedMaterial->TextureUVOffset.y, set_node_draggable);

		ImNodes::SetNodeDraggable(m_ID, set_node_draggable);
		ImNodes::EndNode();

		// -- Draw Links --
		for (Ref<MaterialNodePin>& pin : m_NodeInputPins)
		{
			if (pin->GetOutputPinLinked())
				ImNodes::Link(pin->GetID(), pin->GetID(), pin->GetOutputPinLinked()->GetID());	// Links have the same ID than its input pin
		}
	}

	
	// ----------------------- Public Main Material Node Methods ------------------------------------------
	void MainMaterialNode::DettachMaterial()
	{
		if(m_AttachedMaterial)
			m_AttachedMaterial->InMaterialEditor = false;

		m_AttachedMaterial = nullptr;

		// -- Main Node Pins --
		m_TextureTilingPin->ResetToDefault();
		m_TextureOffsetPinX->ResetToDefault();
		m_TextureOffsetPinY->ResetToDefault();
	}

	void MainMaterialNode::AttachMaterial(SpriteRendererComponent* sprite_component)
	{
		if (m_AttachedMaterial)
			m_AttachedMaterial->InMaterialEditor = false;

		m_AttachedMaterial = sprite_component;
		m_AttachedMaterial->InMaterialEditor = true;

		// -- Main Node Pins --
		m_TextureTilingPin->ResetToDefault();
		m_TextureOffsetPinX->ResetToDefault();
		m_TextureOffsetPinY->ResetToDefault();
	}



	// ---------------------------- CONSTANT NODE ---------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	ConstantMaterialNode::ConstantMaterialNode(ConstantNodeType constant_type) : MaterialNode("Constant Node", MaterialNodeType::CONSTANT)
	{
		m_ConstantType = constant_type;
		switch (m_ConstantType)
		{
			case ConstantNodeType::TCOORDS:
			{
				m_Name = "Texture Coordinates Node";
				AddPin(false, PinDataType::VEC2, "TCoords");
				break;
			}
			
			case ConstantNodeType::DELTATIME:
			{
				m_Name = "Delta Time";
				AddPin(false, PinDataType::FLOAT, "Time");
				break;
			}

			default: { KS_ERROR_AND_ASSERT("Attempted to create a non-supported Constant Node"); }
		}
	}


	float* ConstantMaterialNode::CalculateNodeResult()
	{
		float* ret = nullptr;
		switch (m_ConstantType)
		{
			case ConstantNodeType::TCOORDS:
			{
				float val[2] = { 0.2f, 0.2f };
				ret = static_cast<float*>(val);
				break;
			}

			case ConstantNodeType::DELTATIME:
			{
				float val = 2.0f;
				ret = static_cast<float*>(&val);
				break;
			}

			default: { KS_ERROR_AND_ASSERT("Invalid Constant Node Type"); break; }
		}

		m_NodeOutputPin->SetValue(ret);
		return m_NodeOutputPin->GetValue();
	}



	// ---------------------------- OPERATION NODE --------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	OperationMaterialNode::OperationMaterialNode(OperationNodeType operation_type, PinDataType operation_data_type) : MaterialNode("Operation Node", MaterialNodeType::OPERATION)
	{
		m_OperationType = operation_type;
		switch (m_OperationType)
		{
			case OperationNodeType::ADDITION:			{ m_Name = "Sum Node";  break; }
			case OperationNodeType::MULTIPLICATION:		{ m_Name = "Multiply Node"; break; }
			default:									{ KS_ERROR_AND_ASSERT("Attempted to create a non-supported Operation Node"); }
		}

		AddPin(true, operation_data_type, "Value 1");
		AddPin(true, operation_data_type, "Value 2");
		AddPin(false, operation_data_type, "Out");
	}	


	float* OperationMaterialNode::CalculateNodeResult()
	{
		PinDataType data_type = m_NodeInputPins[0]->GetType();
		float* ret = GetInputValue(0);

		for (uint i = 1; i < m_NodeInputPins.size(); ++i)
			ret = ProcessOperation(data_type, ret, GetInputValue(i));

		return ret;
	}


	float* OperationMaterialNode::ProcessOperation(PinDataType data_type, const float* a, const float* b) const
	{
		switch (m_OperationType)
		{
			case OperationNodeType::ADDITION:		return NodeUtils::SumValues(data_type, a, b);
			case OperationNodeType::MULTIPLICATION:	return NodeUtils::MultiplyValues(data_type, a, b);
		}

		KS_ERROR_AND_ASSERT("Attempted to perform a non-supported operation in OperationNode!");
		return nullptr;
	}

}
