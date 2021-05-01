#include "kspch.h"
#include "MaterialNodePin.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>


namespace Kaimos::MaterialEditor {


	// ---------------------------- NODE PIN --------------------------------------------------------------
	NodePin::NodePin(MaterialNode* owner, PinDataType type, const std::string& name) : m_OwnerNode(owner), m_Type(type), m_Name(name)
	{
		m_ID = (uint)Kaimos::Random::GetRandomInt();
		m_Value = CreateRef<float>(new float[4]{ 0.0f });
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
		ImNodes::BeginOutputAttribute(GetID());
		ImGui::Indent(50.0f);
		ImGui::Text(GetName().c_str());

		DrawOutputResult();

		ImNodes::EndOutputAttribute();
		SetValue(m_OwnerNode->CalculateNodeResult()); // TODO: Don't calculate this each frame
	}



	// ----------------------- Private Pin Methods --------------------------------------------------------
	void NodeOutputPin::DrawOutputResult()
	{
		switch (GetType())
		{
			case PinDataType::FLOAT:
			{
				float res = m_OwnerNode->GetOutputResult<float>();
				ImGui::Text("Value: %.1f", res); return;
			}
			case PinDataType::INT:
			{
				int res = m_OwnerNode->GetOutputResult<int>();
				ImGui::Text("Value: %i", res); return;
			}
			case PinDataType::VEC2:
			{
				glm::vec2 res = m_OwnerNode->GetOutputResult<glm::vec2>();
				ImGui::Text("Value: %.1f, %.1f", res.x, res.y); return;
			}
			case PinDataType::VEC4:
			{
				glm::vec4 res = m_OwnerNode->GetOutputResult<glm::vec4>();
				ImGui::Text("Value: %.1f, %.1f, %.1f, %.1f", res.x, res.y, res.z, res.w); return;
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
	NodeInputPin::~NodeInputPin()
	{
		DisconnectOutputPin();
		m_OutputLinked = nullptr;
		m_DefaultValue.reset();
	}


	void NodeInputPin::DrawUI(bool& allow_node_drag, float* value_to_modify) // TODO: Deharcode this, maybe a f(x) that sets a size according to type?
	{
		ImNodes::BeginInputAttribute(GetID());
		ImGui::Text(GetName().c_str());
		ImNodes::EndInputAttribute();

		if(value_to_modify)
			memcpy(value_to_modify, m_Value.get(), NodeUtils::GetDataSizeFromType(GetType()));		

		if (m_OutputLinked)
			SetValue(m_OutputLinked->GetValue().get()); // TODO: Don't calculate this each frame
		else
		{
			ImGui::PushID(GetID());

			NodeUtils::DrawPinWidget(GetType(), m_Value.get());
			SetDefaultValue(m_Value.get());

			if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
				allow_node_drag = false;

			ImGui::PopID();
		}
	}


	
	// ----------------------- Public Pin Methods ---------------------------------------------------------
	void NodeInputPin::LinkPin(NodePin* output_pin)
	{
		if (output_pin && !output_pin->IsInput() && output_pin->GetType() == GetType())
		{
			// Check if this pin's node is connected to the node of the output_pin					// TODO!
			//NodeOutputPin* node_output = m_OwnerNode->GetOutputPin();
			//if (node_output)
			//{
			//	for (Ref<NodeInputPin> othernode_input : *output_pin->m_OwnerNode->GetInputPins())
			//		if (othernode_input->m_OutputPinLinked && othernode_input->m_OutputPinLinked->m_ID == node_output->m_ID)
			//			return;
			//}

			// If this pin has output erase it from output's inputs
			if (m_OutputLinked)
				m_OutputLinked->DisconnectInputPin(GetID());

			// Connect output to this pin & pushback this pin into the output's inputs
			m_OutputLinked = static_cast<NodeOutputPin*>(output_pin);
			m_OutputLinked->m_InputsLinked.push_back(this);
		}
	}


	void NodeInputPin::DisconnectOutputPin()
	{
		if (m_OutputLinked)
		{
			m_OutputLinked->DisconnectInputPin(GetID());
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
