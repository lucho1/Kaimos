#include "kspch.h"
#include "MaterialNodePin.h"

#include <imgui.h>
#include <imnodes.h>


namespace Kaimos::MaterialEditor {


	// ---------------------------- NODE PIN --------------------------------------------------------------
	NodePin::NodePin(MaterialNode* owner, PinDataType pin_data_type, const std::string& name) : m_OwnerNode(owner), m_PinDataType(pin_data_type), m_Name(name)
	{
		m_ID = (uint)Kaimos::Random::GetRandomInt();
		m_Value = CreateRef<float>(new float[4]);
		std::fill(m_Value.get(), m_Value.get() + 4, 0.0f);
	}

	void NodePin::DeleteLink(int input_pin_id)
	{
		if (IsInput())
			((NodeInputPin*)this)->DisconnectOutputPin();
		else if (input_pin_id != -1)
			((NodeOutputPin*)this)->DisconnectInputPin(input_pin_id);
	}




	// ---------------------------- OUTPUT PIN ------------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	NodeOutputPin::~NodeOutputPin()
	{
		for (NodeInputPin* pin : m_InputsLinked)
		{
			pin->DisconnectOutputPin(); // This already calls for DisconnectInputPin(pin->GetID()) in its (this) output
			pin = nullptr;
		}

		m_InputsLinked.clear();
	}


	void NodeOutputPin::DrawUI()
	{
		ImNodes::BeginOutputAttribute(m_ID);

		float indent = ImNodes::GetNodeDimensions(m_OwnerNode->GetID()).x / 1.8f;
		ImGui::NewLine(); ImGui::SameLine(indent);
		ImGui::Text(m_Name.c_str());

		if (!m_VertexParameter)
			DrawOutputResult(indent);

		ImNodes::EndOutputAttribute();

		if(!m_VertexParameter)
			SetValue(m_OwnerNode->CalculateNodeResult()); // TODO: Don't calculate this each frame
	}



	// ----------------------- Private Pin Methods --------------------------------------------------------
	void NodeOutputPin::DrawOutputResult(float text_indent)
	{
		ImGui::NewLine(); ImGui::SameLine(text_indent);

		switch (m_PinDataType)
		{
			case PinDataType::FLOAT:
			{
				float res = NodeUtils::GetDataFromType<float>(m_Value.get(), m_PinDataType);
				ImGui::Text("Value: %.1f", res); return;
			}
			case PinDataType::INT:
			{
				int res = NodeUtils::GetDataFromType<int>(m_Value.get(), m_PinDataType);
				ImGui::Text("Value: %i", res); return;
			}
			case PinDataType::VEC2:
			{
				glm::vec2 res = NodeUtils::GetDataFromType<glm::vec2>(m_Value.get(), m_PinDataType);
				ImGui::Text("Value: %.1f, %.1f", res.x, res.y); return;
			}
			case PinDataType::VEC3:
			{
				glm::vec3 res = NodeUtils::GetDataFromType<glm::vec3>(m_Value.get(), m_PinDataType);
				ImGui::Text("Value: %.1f, %.1f, %.1f", res.x, res.y, res.z); return;
			}
			case PinDataType::VEC4:
			{
				glm::vec4 res = NodeUtils::GetDataFromType<glm::vec4>(m_Value.get(), m_PinDataType);
				ImGui::Text("Value: %.2f, %.2f, %.2f, %.2f", res.x, res.y, res.z, res.w); return;
			}
		}

		KS_ERROR_AND_ASSERT("Tried to draw a non-supported PinType!");
	}



	// ----------------------- Public Pin Methods ---------------------------------------------------------
	void NodeOutputPin::LinkPin(NodePin* input_pin)
	{
		if (input_pin && input_pin->IsInput())
			static_cast<NodeInputPin*>(input_pin)->LinkPin(this);
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
	
	

	// ---------------------------- INPUT PIN -------------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	NodeInputPin::NodeInputPin(MaterialNode* owner, PinDataType pin_data_type, const std::string& name, float default_value) : NodePin(owner, pin_data_type, name)
	{
		m_DefaultValue = CreateRef<float>(new float[4]);
		std::fill(m_DefaultValue.get(), m_DefaultValue.get() + 4, default_value);
		ResetToDefault();
	}


	NodeInputPin::~NodeInputPin()
	{
		DisconnectOutputPin();
		m_OutputLinked = nullptr;
		m_DefaultValue.reset();
	}


	void NodeInputPin::DrawUI(bool& allow_node_drag, float* value_to_modify, bool is_vtxattribute)
	{
		ImNodes::BeginInputAttribute(m_ID);
		ImGui::Text(m_Name.c_str());
		ImNodes::EndInputAttribute();

		if(value_to_modify)
			memcpy(value_to_modify, m_Value.get(), NodeUtils::GetDataSizeFromType(m_PinDataType));

		if (m_OutputLinked)
			SetValue(m_OutputLinked->m_Value.get()); // TODO: Don't calculate this each frame
		else if(!is_vtxattribute)
		{
			ImGui::PushID(m_ID);

			NodeUtils::DrawPinWidget(m_PinDataType, m_Value.get());
			SetDefaultValue(m_Value.get());

			if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
				allow_node_drag = false;

			ImGui::PopID();
		}
	}


	
	// ----------------------- Public Pin Methods ---------------------------------------------------------
	void NodeInputPin::LinkPin(NodePin* output_pin)
	{
		if (output_pin && !output_pin->IsInput() && output_pin->GetType() == m_PinDataType)
		{
			// Check if this pin's node is connected to the node of the output_pin	// TODO: Handle unavailable connections (loops, ...)				// TODO!
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


	void NodeInputPin::DisconnectOutputPin()
	{
		if (m_OutputLinked)
		{
			m_OutputLinked->DisconnectInputPin(m_ID);
			m_OutputLinked = nullptr;
			ResetToDefault();
		}
	}


	float* NodeInputPin::CalculateInputValue()
	{
		if (m_OutputLinked)
			return m_OutputLinked->m_OwnerNode->CalculateNodeResult();

		return m_Value.get();
	}

}
