#include "kspch.h"
#include "MaterialNodePin.h"
#include "MaterialNode.h"
#include "Scene/KaimosYAMLExtension.h"

#include <imgui.h>
#include <imnodes.h>
#include <yaml-cpp/yaml.h>


namespace Kaimos::MaterialEditor {


	// ---------------------------- NODE PIN --------------------------------------------------------------
	NodePin::NodePin(MaterialNode* owner, PinDataType pin_data_type, const std::string& name) : m_OwnerNode(owner), m_PinDataType(pin_data_type), m_Name(name)
	{
		m_ID = (uint)Kaimos::Random::GetRandomInt();
	}

	void NodePin::DeleteLink(int input_pin_id)
	{
		if (IsInput())
			((NodeInputPin*)this)->DisconnectOutputPin();
		else if (input_pin_id != -1)
			((NodeOutputPin*)this)->DisconnectInputPin(input_pin_id);
	}


	void NodePin::SerializeBasePin(YAML::Emitter& output_emitter) const
	{
		// -- Serialize Variables --
		output_emitter << YAML::Key << "Pin" << YAML::Value << m_ID;
		output_emitter << YAML::Key << "Name" << YAML::Value << m_Name.c_str();
		output_emitter << YAML::Key << "DataType" << YAML::Value << (int)m_PinDataType;

		// -- Serialize Pin Value --
		output_emitter << YAML::Key << "Value" << YAML::Value << m_Value;
	}




	// ---------------------------- OUTPUT PIN ------------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	NodeOutputPin::~NodeOutputPin()
	{
		for (NodeInputPin* pin : m_InputsLinked)
		{
			pin->DisconnectOutputPin(true); // This already calls for DisconnectInputPin(pin->GetID()) in its (this) output
			pin = nullptr;
		}

		m_InputsLinked.clear();
	}


	void NodeOutputPin::DrawUI()
	{
		ImGui::PushID(m_ID);
		ImNodes::BeginOutputAttribute(m_ID);

		float indent = ImNodes::GetNodeDimensions(m_OwnerNode->GetID()).x / 1.8f;
		ImGui::NewLine(); ImGui::SameLine(indent);
		ImGui::Text(m_Name.c_str());

		if (!m_VertexParameter)
			DrawOutputResult(indent);

		ImNodes::EndOutputAttribute();
		if (!m_VertexParameter)
		{
			// TODO: Don't calculate this each frame
			glm::vec4 node_value = m_OwnerNode->CalculateNodeResult();
			SetValue(node_value);
		}

		ImGui::PopID();
	}



	// ----------------------- Private Pin Methods --------------------------------------------------------
	void NodeOutputPin::DrawOutputResult(float text_indent)
	{
		ImGui::NewLine(); ImGui::SameLine(text_indent);

		switch (m_PinDataType)
		{
			case PinDataType::FLOAT:	ImGui::Text("Value: %.1f", m_Value.x); return;
			case PinDataType::INT:		ImGui::Text("Value: %i", static_cast<int>(m_Value.x)); return;
			case PinDataType::VEC2:		ImGui::Text("Value: %.1f, %.1f", m_Value.x, m_Value.y); return;
			case PinDataType::VEC3:		ImGui::Text("Value: %.1f, %.1f, %.1f", m_Value.x, m_Value.y, m_Value.z); return;
			case PinDataType::VEC4:		ImGui::Text("Value: %.2f, %.2f, %.2f, %.2f", m_Value.x, m_Value.y, m_Value.z, m_Value.w); return;
		}

		KS_FATAL_ERROR("Tried to draw a non-supported PinType!");
	}


	void NodeOutputPin::SetOutputDataType(PinDataType datatype_to_set)
	{
		// This function is meant to check if there are still nodes of the passed type
		// Check for inputs of the type passed
		bool input_found = false;
		for (uint i = 0; i < m_OwnerNode->GetInputsQuantity(); ++i)
		{
			if (m_OwnerNode->GetInputPin(i)->GetType() == datatype_to_set)
			{
				input_found = true;
				break;
			}
		}

		if (input_found)
		{
			// If there are inputs & pin type is float, change to passed type if needed
			if (m_PinDataType == PinDataType::FLOAT)
			{
				m_PinDataType = datatype_to_set;
				DisconnectAllInputPins();
			}
		}
		else
		{
			// If no inputs left of that type & pin type is the same as passed, change to float
			if (m_PinDataType == datatype_to_set)
			{
				m_PinDataType = PinDataType::FLOAT;
				DisconnectAllInputPins();
			}
		}
	}



	// ----------------------- Public Pin Methods ---------------------------------------------------------
	void NodeOutputPin::LinkPin(NodePin* input_pin)
	{
		if (input_pin && input_pin->IsInput())
			static_cast<NodeInputPin*>(input_pin)->LinkPin(this);
	}


	void NodeOutputPin::DisconnectAllInputPins()
	{
		for (NodeInputPin* pin : m_InputsLinked)
		{
			pin->DisconnectOutputPin(); // This already calls for DisconnectInputPin(pin->GetID()) in its (this) output
			pin = nullptr;
		}

		m_InputsLinked.clear();
	}


	void NodeOutputPin::DisconnectInputPin(uint input_pinID)
	{
		std::vector<NodeInputPin*>::const_iterator it = m_InputsLinked.begin();
		for (; it != m_InputsLinked.end(); ++it)
		{
			if ((*it)->GetID() == input_pinID)
			{
				m_InputsLinked.erase(it);
				break;
			}
		}
	}


	void NodeOutputPin::SerializePin(YAML::Emitter& output_emitter) const
	{
		// -- Begin Pin Map --
		output_emitter << YAML::Key << "OutputPin";
		output_emitter << YAML::BeginMap;

		// -- Serialize Base Pin & Variables --
		SerializeBasePin(output_emitter);
		output_emitter << YAML::Key << "IsVertexParam" << YAML::Value << m_VertexParameter;

		// -- Serialize Inputs Linked --
		output_emitter << YAML::Key << "InputPinsLinkedIDs" << YAML::Value << YAML::BeginSeq;
		for (uint i = 0; i < m_InputsLinked.size(); ++i)
		{
			std::string input_keyval = "InputPinLinked" + std::to_string(i);
			output_emitter << YAML::Value << m_InputsLinked[i]->GetID();
		}

		// -- End Inputs Sequence & Pin Map --
		output_emitter << YAML::EndSeq;
		output_emitter << YAML::EndMap;
	}
	
	

	// ---------------------------- INPUT PIN -------------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	NodeInputPin::NodeInputPin(MaterialNode* owner, PinDataType pin_data_type, bool allows_multi_type, const std::string& name, float default_value)
		: NodePin(owner, pin_data_type, name), m_AllowsMultipleTypes(allows_multi_type)
	{
		m_DefaultValue = glm::vec4(default_value);
		ResetToDefault();
	}

	NodeInputPin::NodeInputPin(MaterialNode* owner, const std::string& name, uint id, PinDataType pin_data_type, const glm::vec4& value, const glm::vec4& default_value, bool allows_multi_type)
		: NodePin(owner, pin_data_type, name, id, value), m_AllowsMultipleTypes(allows_multi_type)
	{
		m_DefaultValue = glm::vec4(default_value);
	}


	NodeInputPin::~NodeInputPin()
	{
		DisconnectOutputPin(true);
		m_OutputLinked = nullptr;
	}


	void NodeInputPin::DrawUI(bool& allow_node_drag, bool is_vtxattribute, bool modify_value, glm::vec4& value_to_modify, float widget_speed, float widget_min, float widget_max, const char* widget_format)
	{
		ImGui::PushID(m_ID);
		ImNodes::BeginInputAttribute(m_ID);
		ImGui::Text(m_Name.c_str());
		ImNodes::EndInputAttribute();

		if(modify_value)
			value_to_modify = m_Value;

		if (m_OutputLinked)
			SetValue(m_OutputLinked->m_Value); // TODO: Don't calculate this each frame
		else if(!is_vtxattribute)
		{
			//ImGui::PushID(m_ID);

			NodeUtils::DrawPinWidget(m_PinDataType, m_Value, widget_speed, widget_min, widget_max, widget_format);
			SetDefaultValue(m_Value);

			if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
				allow_node_drag = false;

			//ImGui::PopID();
		}

		ImGui::PopID();
	}


		
	// ----------------------- Private Pin Methods --------------------------------------------------------
	bool NodeInputPin::CheckLinkage(NodePin* output_pin)
	{
		if (!output_pin || output_pin->IsInput())
			return false;

		PinDataType other_pintype = output_pin->GetType();
		if (m_OwnerNode->GetType() == MaterialNodeType::OPERATION)
		{
			// If we are in operation node
			bool ret = false;
			PinDataType op_pintype = (static_cast<OperationMaterialNode*>(m_OwnerNode))->GetVecOperationType();

			// Check for data type to be float or operation type
			if(m_PinDataType == PinDataType::FLOAT || m_PinDataType == op_pintype)
			{
				// Check the same for the other pin
				if (other_pintype == PinDataType::FLOAT || other_pintype == op_pintype)
				{
					// If so, change type
					m_PinDataType = other_pintype;

					// This is meant to check if there are nodes of the passed type, so we pass operation type
					m_OwnerNode->CheckOutputType(op_pintype);
					return true;
				}
				else
					return false;
			}
		}

		return other_pintype == m_PinDataType;
	}



	// ----------------------- Public Pin Methods ---------------------------------------------------------
	void NodeInputPin::LinkPin(NodePin* output_pin)
	{
		if (CheckLinkage(output_pin))
		{
			// Check if this pin's node is connected to the node of the output_pin	// TODO: Handle unavailable connections (loops, ...) // TODO!
			//NodeOutputPin* node_output = m_OwnerNode->GetOutputPin();
			//if (node_output)
			//{
			//	for (Ref<NodeInputPin> othernode_input : *output_pin->m_OwnerNode->GetInputPins())
			//		if (othernode_input->m_OutputPinLinked && othernode_input->m_OutputPinLinked->m_ID == node_output->m_ID)
			//			return;
			//}

			// If this pin has output erase it from output's inputs
			if (m_OutputLinked)
				m_OutputLinked->DisconnectInputPin(m_ID);

			// Connect output to this pin & pushback this pin into the output's inputs
			m_OutputLinked = static_cast<NodeOutputPin*>(output_pin);
			m_OutputLinked->m_InputsLinked.push_back(this);
		}
	}


	void NodeInputPin::DisconnectOutputPin(bool is_destroying)
	{
		if (!m_OutputLinked)
			return;

		if (m_OwnerNode->GetType() == MaterialNodeType::OPERATION && !is_destroying)
		{
			OperationMaterialNode* op_node = static_cast<OperationMaterialNode*>(m_OwnerNode);
			OperationNodeType op_type = op_node->GetOperationType();

			if (op_type == OperationNodeType::FLOATVEC_MULTIPLY || op_type == OperationNodeType::FLOATVEC_DIVIDE)
			{
				// If we're in a vec-float operation (multiple type), change back to float
				m_PinDataType = PinDataType::FLOAT;
				
				// Also, ask output pin to change its type if needed
				m_OwnerNode->CheckOutputType(op_node->GetVecOperationType());
			}
		}

		// Disconnect from linked output & reset
		m_OutputLinked->DisconnectInputPin(m_ID);
		m_OutputLinked = nullptr;
		ResetToDefault();
	}


	glm::vec4 NodeInputPin::CalculateInputValue()
	{
		if (m_OutputLinked)
			return m_OutputLinked->m_OwnerNode->CalculateNodeResult();

		return m_Value;
	}

	bool NodeInputPin::IsTimed() const
	{
		if (m_OwnerNode->GetType() == MaterialNodeType::CONSTANT && static_cast<ConstantMaterialNode*>(m_OwnerNode)->IsTimeNode())
				return true;

		if (m_OutputLinked)
		{
			MaterialNode* connected_node = m_OutputLinked->m_OwnerNode;
			if (connected_node->GetType() == MaterialNodeType::CONSTANT && static_cast<ConstantMaterialNode*>(connected_node)->IsTimeNode())
					return true;

			return connected_node->IsNodeTimeDependant();
		}

		return false;
	}


	void NodeInputPin::SerializePin(YAML::Emitter& output_emitter) const
	{
		// -- Begin Pin Map --
		output_emitter << YAML::BeginMap;

		// -- Serialize Base Pin & Variables --
		SerializeBasePin(output_emitter);
		output_emitter << YAML::Key << "AllowsMultipleTypes" << YAML::Value << m_AllowsMultipleTypes;

		// -- Serialize Pin Default Value --
		output_emitter << YAML::Key << "DefValue" << YAML::Value << m_DefaultValue;

		// -- End Pin Map --
		output_emitter << YAML::EndMap;
	}

}
