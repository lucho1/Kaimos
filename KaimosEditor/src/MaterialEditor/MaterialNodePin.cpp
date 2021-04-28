#include "kspch.h"
#include "MaterialNodePin.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>


namespace Kaimos {


	// ----------------------- Public Class Methods -------------------------------------------------------
	MaterialNodePin::~MaterialNodePin()
	{
		m_InputPinsLinked.clear();
		m_OutputPinLinked = nullptr;
		m_OwnerNode = nullptr;
	}

	void MaterialNodePin::DrawPinUI(float& value_to_modify, bool& allow_node_drag)
	{
		ImNodes::BeginInputAttribute(m_ID);
		ImGui::Text(m_Name.c_str());
		ImNodes::EndInputAttribute();


		if (m_OutputPinLinked)
		{
			m_Value = m_OutputPinLinked->m_Value;
			value_to_modify = m_Value;
		}
		else
		{
			ImGui::PushID(m_ID);

			ImGui::SameLine(); ImGui::SetNextItemWidth(30.0f);
			ImGui::DragFloat("###float_val", &m_Value, 0.2f);

			if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
				allow_node_drag = false;

			value_to_modify = m_DefaultValue = m_Value;
			ImGui::PopID();
		}
	}

	void MaterialNodePin::ResetToDefault()
	{
		m_Value = m_DefaultValue;
	}

	

	// ----------------------- Public Node Pin Methods ----------------------------------------------------
	void MaterialNodePin::LinkPin(MaterialNodePin* output_pin)
	{
		if (output_pin)
		{
			// Check if this pin's node is connected to the node of the output_pin
			MaterialNodePin* node_output = m_OwnerNode->GetOutputPin();
			if (node_output)
			{
				for (Ref<MaterialNodePin> othernode_input : *output_pin->m_OwnerNode->GetInputPins())
					if (othernode_input->m_OutputPinLinked && othernode_input->m_OutputPinLinked->m_ID == node_output->m_ID)
						return;
			}

			// Check if this pin has an output, then erase this pin from the output's inputs list
			if (m_OutputPinLinked)
				m_OutputPinLinked->DeleteInputPin(m_ID);

			// Connect output pin to this pin and pushback this pin into the output's inputs list
			m_OutputPinLinked = output_pin;
			output_pin->m_InputPinsLinked.push_back(this);
		}
	}


	void MaterialNodePin::DeleteLink()
	{
		if (m_OutputPinLinked)
		{
			m_OutputPinLinked->m_InputPinsLinked.erase(std::find(m_OutputPinLinked->m_InputPinsLinked.begin(), m_OutputPinLinked->m_InputPinsLinked.end(), this));
			m_OutputPinLinked = nullptr;
			ResetToDefault();
		}
	}


	void MaterialNodePin::DeleteInputPin(uint input_pinID)
	{
		std::vector<MaterialNodePin*>::const_iterator it = m_InputPinsLinked.begin();
		for (; it != m_InputPinsLinked.end(); ++it)
		{
			if ((*it)->m_ID == input_pinID)
			{
				m_InputPinsLinked.erase(it);
				break;
			}
		}
	}
}
