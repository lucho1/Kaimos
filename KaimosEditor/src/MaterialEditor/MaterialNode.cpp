#include "kspch.h"
#include "MaterialNode.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>


namespace Kaimos {
		
	// ----------------------- Public Class Methods -------------------------------------------------------
	MaterialNode::~MaterialNode()
	{
		for (Ref<MaterialNodePin>& pin : m_NodeInputPins)
			pin.reset();

		if (m_NodeOutputPin)
		{
			for (auto& pin : m_NodeOutputPin->InputPinsLinked)
			{
				pin->Value = pin->DefaultValue;
				pin->OutputPinLinked = nullptr;
				pin = nullptr;
			}

			m_NodeOutputPin->InputPinsLinked.clear();
		}

		m_NodeOutputPin.reset();
		m_NodeInputPins.clear();
	}


	void MaterialNode::DrawNodeUI()
	{
		// -- Draw Node --
		ImNodes::BeginNode(m_ID);

		// -- Draw Output Pin --
		ImNodes::BeginOutputAttribute(m_NodeOutputPin->ID);
		ImGui::Indent(40.0f);
		ImGui::Text(m_NodeOutputPin->Name.c_str());
		ImNodes::EndOutputAttribute();

		// -- Draw Input Pins --
		for (Ref<MaterialNodePin>& pin : m_NodeInputPins)
		{
			ImNodes::BeginInputAttribute(pin->ID);
			ImGui::Text(pin->Name.c_str());
			ImNodes::EndInputAttribute();
		}

		// -- End Node Drawing --
		ImNodes::EndNode();

		// -- Draw Links --
		for (Ref<MaterialNodePin>& pin : m_NodeInputPins)
		{
			if (pin->OutputPinLinked)
				ImNodes::Link(pin->ID, pin->ID, pin->OutputPinLinked->ID);	// Links have the same ID than its input pin
		}
	}



	// ----------------------- Public Material Node Methods -----------------------------------------------
	void MaterialNode::AddPin(bool input, float default_value)
	{
		if (input)
			m_NodeInputPins.push_back(CreateRef<MaterialNodePin>(this, (uint)Kaimos::Random::GetRandomInt(), "InputP", default_value));
		else if (!m_NodeOutputPin)
			m_NodeOutputPin = CreateRef<MaterialNodePin>(this, (uint)Kaimos::Random::GetRandomInt(), "OutputP");
	}


	void MaterialNode::AddPin(bool input, Ref<MaterialNodePin>& pin)
	{
		if (input)
			m_NodeInputPins.push_back(pin);
		else if (!m_NodeOutputPin)
			m_NodeOutputPin = pin;
	}


	MaterialNodePin* MaterialNode::FindInputPin(uint pinID)
	{
		for (uint i = 0; i < m_NodeInputPins.size(); ++i)
			if (m_NodeInputPins[i]->ID == pinID)
				return m_NodeInputPins[i].get();

		return nullptr;
	}





	// ----------------------- Public Material Node Methods -----------------------------------------------
	MainMaterialNode::MainMaterialNode() : MaterialNode((uint)Kaimos::Random::GetRandomInt(), "Main Node")
	{
		m_TextureTilingPin = CreateRef<MaterialNodePin>(this, (uint)Kaimos::Random::GetRandomInt(), "Texture Tiling");
		AddPin(true, m_TextureTilingPin);
	}

	MainMaterialNode::~MainMaterialNode()
	{
		m_TextureTilingPin.reset();
	}



	void MainMaterialNode::DrawNodeUI()
	{
		// -- Draw Node --
		ImNodes::BeginNode(m_ID);

		// -- Draw Input Pins --
		ImNodes::BeginInputAttribute(m_TextureTilingPin->ID); // Texture Tiling Input
		ImGui::Text(m_TextureTilingPin->Name.c_str());
		ImNodes::EndInputAttribute();

		if (m_TextureTilingPin->OutputPinLinked)
			m_MaterialToModify->TextureTiling = m_TextureTilingPin->OutputPinLinked->Value;
		else
		{
			ImGui::SameLine(); ImGui::SetNextItemWidth(30.0f);
			if (ImGui::DragFloat("##spcomptextiling", &m_TextureTilingPin->Value, 0.2f))
				m_MaterialToModify->TextureTiling = m_TextureTilingPin->Value;
		}

		ImNodes::EndNode();


		// -- Draw Links --
		for (Ref<MaterialNodePin>& pin : m_NodeInputPins)
		{
			if (pin->OutputPinLinked)
				ImNodes::Link(pin->ID, pin->ID, pin->OutputPinLinked->ID);	// Links have the same ID than its input pin
		}
	}
}
