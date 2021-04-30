#include "kspch.h"
#include "MaterialNodePin.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>


namespace Kaimos::MaterialEditor {
	
	
	// ----------------------- Public Class Methods -------------------------------------------------------
	MaterialNodePin::MaterialNodePin(MaterialNode* owner, PinDataType type, const std::string& name, float default_value) : m_OwnerNode(owner), m_Type(type), m_Name(name)
	{
		m_ID = (uint)Kaimos::Random::GetRandomInt();
		m_Value = CreateRef<float>(new float[4]{0.0f});
		m_DefaultValue = CreateRef<float>(new float[4]{ default_value });
	}

	MaterialNodePin::~MaterialNodePin()
	{
		for (MaterialNodePin* pin : m_InputPinsLinked)
			pin->DeleteLink();

		m_InputPinsLinked.clear();

		DeleteLink();
		m_OutputPinLinked = nullptr;
		m_OwnerNode = nullptr;
		m_Value.reset();
		m_DefaultValue.reset();
		m_Value = m_DefaultValue = nullptr;
	}

	void MaterialNodePin::DrawPinUI(float& value_to_modify, bool& allow_node_drag)
	{
		ImNodes::BeginInputAttribute(m_ID);
		ImGui::Text(m_Name.c_str());
		ImNodes::EndInputAttribute();

		if (m_OutputPinLinked)
		{
			SetValue(m_OutputPinLinked->m_Value.get());
			value_to_modify = m_Value.get()[0];
		}
		else
		{
			ImGui::PushID(m_ID);

			ImGui::SameLine(); ImGui::SetNextItemWidth(30.0f);
			ImGui::DragFloat("###float_val", &m_Value.get()[0], 0.2f);

			if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
				allow_node_drag = false;

			value_to_modify = m_Value.get()[0];
			memcpy(m_DefaultValue.get(), m_Value.get(), 4);
			ImGui::PopID();
		}
	}

	void MaterialNodePin::ResetToDefault()
	{
		memcpy(m_Value.get(), m_DefaultValue.get(), 16);
	}

	void MaterialNodePin::SetValue(float* value)
	{
		memcpy(m_Value.get(), value, 16);
	}

	

	// ----------------------- Public Node Pin Methods ----------------------------------------------------
	void MaterialNodePin::LinkPin(MaterialNodePin* output_pin)
	{
		if (output_pin && output_pin->m_Type == m_Type)
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

			// Connect output pin to this pin and pushback this pin into the output's inputs list if(pin.type == type)
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
