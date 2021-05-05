#include "kspch.h"
#include "MaterialNode.h"
#include "MaterialNodePin.h"

#include "Core/Application/Application.h"
#include "Scene/ECS/Components.h"

#include <imgui.h>
#include <imnodes.h>
#include <glm/gtc/type_ptr.hpp>


namespace Kaimos::MaterialEditor {


	// ---------------------------- BASE MAT NODE ---------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	MaterialNode::~MaterialNode()
	{
		for (Ref<NodeInputPin>& pin : m_NodeInputPins)
		{
			pin.reset();
			pin = nullptr;
		}

		m_NodeInputPins.clear();
		m_NodeOutputPin.reset();
	}


	void MaterialNode::DrawNodeUI()
	{
		// -- Draw Node & Header --
		ImNodes::BeginNode(m_ID);

		ImNodes::BeginNodeTitleBar();
		ImGui::NewLine(); ImGui::SameLine(ImGui::GetItemRectSize().x / 4.0f);
		ImGui::Text(m_Name.c_str());
		ImNodes::EndNodeTitleBar();

		// -- Draw Output Pin --
		if(m_NodeOutputPin)
			m_NodeOutputPin->DrawUI();

		// -- Draw Input Pins --
		bool set_node_draggable = true;
		for (Ref<NodeInputPin>& pin : m_NodeInputPins)
			pin->DrawUI(set_node_draggable);
		

		// -- End Node Drawing --
		ImNodes::SetNodeDraggable(m_ID, set_node_draggable);
		ImNodes::EndNode();

		// -- Draw Links --
		for (Ref<NodeInputPin>& pin : m_NodeInputPins)
		{
			if (pin->IsConnected())
				ImNodes::Link(pin->GetID(), pin->GetID(), pin->GetOutputLinkedID());	// Links have the same ID than its input pin
		}
	}


	NodePin* MaterialNode::FindPinInNode(uint pinID)
	{
		if (m_NodeOutputPin && m_NodeOutputPin->GetID() == pinID)
			return static_cast<NodePin*>(m_NodeOutputPin.get());

		NodeInputPin* pin = FindInputPin(pinID);
		if (pin)
			return static_cast<NodePin*>(pin);

		return nullptr;
	}



	// ----------------------- Public Material Node Methods -----------------------------------------------
	NodeInputPin* MaterialNode::FindInputPin(uint pinID)
	{
		for (uint i = 0; i < m_NodeInputPins.size(); ++i)
			if (m_NodeInputPins[i]->GetID() == pinID)
				return m_NodeInputPins[i].get();

		return nullptr;
	}

	void MaterialNode::AddInputPin(PinDataType pin_data_type, const std::string& name, float default_value)
	{
		m_NodeInputPins.push_back(CreateRef<NodeInputPin>(this, pin_data_type, name, default_value));
	}

	void MaterialNode::AddOutputPin(PinDataType pin_data_type, const std::string& name, float default_value)
	{
		m_NodeOutputPin = CreateRef<NodeOutputPin>(this, pin_data_type, name);
	}

	float* MaterialNode::GetInputValue(uint input_index)
	{
		if (input_index < m_NodeInputPins.size())
			return m_NodeInputPins[input_index]->CalculateInputValue();

		KS_ERROR_AND_ASSERT("Tried to access an out-of-bounds input!");
		return nullptr;
	}




	// ---------------------------- MAIN MAT NODE ---------------------------------------------------------
	MainMaterialNode::MainMaterialNode(Material* attached_material)
		: MaterialNode("Main Node", MaterialNodeType::MAIN), m_AttachedMaterial(attached_material)
	{
		m_TextureCoordinatesPin = CreateRef<NodeInputPin>(this, PinDataType::VEC2, "Texture Coordinates", 0.0f);
		m_VertexPositionPin = CreateRef<NodeInputPin>(this, PinDataType::VEC3, "Vertex Position", 0.0f);
		m_VertexNormalPin = CreateRef<NodeInputPin>(this, PinDataType::VEC3, "Vertex Normal", 0.0f);

		m_TextureTilingPin = CreateRef<NodeInputPin>(this, PinDataType::FLOAT, "Texture Tiling", 1.0f);
		m_TextureOffsetPin = CreateRef<NodeInputPin>(this, PinDataType::VEC2, "Texture Offset", 0.0f);
		m_ColorPin = CreateRef<NodeInputPin>(this, PinDataType::VEC4, "Color", 1.0f);

		m_NodeInputPins.push_back(m_TextureCoordinatesPin);
		m_NodeInputPins.push_back(m_VertexPositionPin);
		m_NodeInputPins.push_back(m_VertexNormalPin);

		m_NodeInputPins.push_back(m_TextureTilingPin);
		m_NodeInputPins.push_back(m_TextureOffsetPin);
		m_NodeInputPins.push_back(m_ColorPin);
	}


	MainMaterialNode::~MainMaterialNode()
	{
		//DettachMaterial();
		m_TextureCoordinatesPin.reset();
		m_VertexPositionPin.reset();
		m_VertexNormalPin.reset();
		m_TextureTilingPin.reset();
		m_TextureOffsetPin.reset();
		m_ColorPin.reset();
	}


	void MainMaterialNode::DrawNodeUI()
	{
		// -- Push Node Colors --
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(179, 51, 51, 255));
		ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(230, 76, 76, 255));
		ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(230, 76, 76, 255));

		// -- Draw Node & Header --
		ImNodes::BeginNode(m_ID);

		ImNodes::BeginNodeTitleBar();
		ImGui::Text(m_Name.c_str());
		ImNodes::EndNodeTitleBar();

		// -- Draw Input Pins --
		bool set_node_draggable = true;

		m_TextureCoordinatesPin->DrawUI(set_node_draggable, nullptr, true);
		m_VertexPositionPin->DrawUI(set_node_draggable, nullptr, true);
		m_VertexNormalPin->DrawUI(set_node_draggable, nullptr, true);

		m_TextureTilingPin->DrawUI(set_node_draggable, &m_AttachedMaterial->TextureTiling);
		m_TextureOffsetPin->DrawUI(set_node_draggable, glm::value_ptr(m_AttachedMaterial->TextureUVOffset));
		m_ColorPin->DrawUI(set_node_draggable, glm::value_ptr(m_AttachedMaterial->Color));

		ImNodes::SetNodeDraggable(m_ID, set_node_draggable);
		ImNodes::EndNode();

		// -- Draw Links --
		for (Ref<NodeInputPin>& pin : m_NodeInputPins)
		{
			if (pin->IsConnected())
				ImNodes::Link(pin->GetID(), pin->GetID(), pin->GetOutputLinkedID());	// Links have the same ID than its input pin
		}

		// -- Pop Node Colors --
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
	}

	void MainMaterialNode::SyncValuesWithMaterial()
	{
		m_TextureTilingPin->SetInputValue(&m_AttachedMaterial->TextureTiling);
		m_TextureOffsetPin->SetInputValue(glm::value_ptr(m_AttachedMaterial->TextureUVOffset));
		m_ColorPin->SetInputValue(glm::value_ptr(m_AttachedMaterial->Color));
	}




	// ---------------------------- VERTEX PARAMETER NODE -------------------------------------------------
	VertexParameterMaterialNode::VertexParameterMaterialNode(VertexParameterNodeType parameter_type) : MaterialNode("VtxParam Node", MaterialNodeType::VERTEX_PARAMETER), m_ParameterType(parameter_type)
	{
		switch (m_ParameterType)
		{
			case VertexParameterNodeType::TEX_COORDS:
			{
				m_Name = "TCoords";
				AddOutputPin(PinDataType::VEC2, "XY");
				break;
			}

			case VertexParameterNodeType::POSITION:
			{
				m_Name = "VertexPos";
				AddOutputPin(PinDataType::VEC3, "XYZ");
				break;
			}

			case VertexParameterNodeType::NORMAL:
			{
				m_Name = "VertexNorm";
				AddOutputPin(PinDataType::VEC3, "XYZ");
				break;
			}

			default: { KS_ERROR_AND_ASSERT("Attempted to create a non-supported Vertex parameter Node"); }
		}
	}

	void VertexParameterMaterialNode::SetNodeOutputResult(float* value)
	{
		m_NodeOutputPin->SetOutputValue(value);
	}

	float* VertexParameterMaterialNode::CalculateNodeResult()
	{
		return this->m_NodeOutputPin->GetValue().get();
	}

	void VertexParameterMaterialNode::AddOutputPin(PinDataType pin_data_type, const std::string& name, float default_value)
	{
		m_NodeOutputPin = CreateRef<NodeOutputPin>(this, pin_data_type, name, true);
	}




	// ---------------------------- CONSTANT NODE ---------------------------------------------------------
	ConstantMaterialNode::ConstantMaterialNode(ConstantNodeType constant_type) : MaterialNode("Constant Node", MaterialNodeType::CONSTANT), m_ConstantType(constant_type)
	{
		switch (m_ConstantType)
		{
			case ConstantNodeType::DELTATIME:
			{
				m_Name = "Delta Time";
				AddOutputPin(PinDataType::FLOAT, "Time");
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
			case ConstantNodeType::DELTATIME:
			{
				float val = Application::Get().GetTime();
				ret = static_cast<float*>(&val);
				break;
			}

			default: { KS_ERROR_AND_ASSERT("Invalid Constant Node Type"); break; }
		}

		m_NodeOutputPin->SetOutputValue(ret);
		return m_NodeOutputPin->GetValue().get();
	}




	// ---------------------------- OPERATION NODE --------------------------------------------------------
	OperationMaterialNode::OperationMaterialNode(OperationNodeType operation_type, PinDataType operation_data_type) : MaterialNode("Operation Node", MaterialNodeType::OPERATION)
	{
		m_OperationType = operation_type;
		switch (m_OperationType)
		{
			case OperationNodeType::ADDITION:			{ m_Name = "Sum Node";  break; }
			case OperationNodeType::MULTIPLICATION:		{ m_Name = "Multiply Node"; break; }
			default:									{ KS_ERROR_AND_ASSERT("Attempted to create a non-supported Operation Node"); }
		}

		AddInputPin(operation_data_type, "Value 1");
		AddInputPin(operation_data_type, "Value 2");
		AddOutputPin(operation_data_type, "Out");
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
