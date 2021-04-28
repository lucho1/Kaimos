#include "kspch.h"
#include "MaterialNode.h"
#include "MaterialNodePin.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>


namespace Kaimos::MaterialEditor {
	

	// ---------------------------- BASE MAT NODE ---------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	MaterialNode::~MaterialNode()
	{
		for (Ref<MaterialNodePin>& pin : m_NodeInputPins)
			pin.reset();

		m_NodeOutputPin.reset();
		m_NodeInputPins.clear();
	}

	void MaterialNode::DrawNodeUI()
	{
		// -- Draw Node --
		ImNodes::BeginNode(m_ID);

		// -- Draw Output Pin --
		ImNodes::BeginOutputAttribute(m_NodeOutputPin->GetID());
		ImGui::Indent(40.0f);
		ImGui::Text(m_NodeOutputPin->GetName().c_str());
		ImNodes::EndOutputAttribute();

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
			if (pin->GetOutputPin())
				ImNodes::Link(pin->GetID(), pin->GetID(), pin->GetOutputPin()->GetID());	// Links have the same ID than its input pin
		}
	}



	// ----------------------- Public Material Node Methods -----------------------------------------------
	void MaterialNode::AddPin(bool input, float default_value)
	{
		if (input)
			m_NodeInputPins.push_back(CreateRef<MaterialNodePin>(this, (uint)Kaimos::Random::GetRandomInt(), "InputP", default_value));
		else if (!m_NodeOutputPin)
			m_NodeOutputPin = CreateRef<MaterialNodePin>(this, (uint)Kaimos::Random::GetRandomInt(), "OutputP", 0.0f);
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
			if (m_NodeInputPins[i]->GetID() == pinID)
				return m_NodeInputPins[i].get();

		return nullptr;
	}




	// ---------------------------- MAIN MAT NODE ---------------------------------------------------------
	// ----------------------- Public Class Methods -------------------------------------------------------
	MainMaterialNode::MainMaterialNode() : MaterialNode((uint)Kaimos::Random::GetRandomInt(), "Main Node")
	{
		m_TextureTilingPin = CreateRef<MaterialNodePin>(this, (uint)Kaimos::Random::GetRandomInt(), "Texture Tiling", 1.0f);
		m_TextureOffsetPinX = CreateRef<MaterialNodePin>(this, (uint)Kaimos::Random::GetRandomInt(), "Texture Offset X", 0.0f);		
		m_TextureOffsetPinY = CreateRef<MaterialNodePin>(this, (uint)Kaimos::Random::GetRandomInt(), "Texture Offset Y", 0.0f);

		AddPin(true, m_TextureTilingPin);
		AddPin(true, m_TextureOffsetPinX);
		AddPin(true, m_TextureOffsetPinY);
	}

	MainMaterialNode::~MainMaterialNode()
	{
		m_TextureTilingPin.reset();
		m_TextureOffsetPinX.reset();
		m_TextureOffsetPinY.reset();
	}


	void MainMaterialNode::DrawNodeUI()
	{
		// -- Draw Node --
		ImNodes::BeginNode(m_ID);

		ImNodes::BeginNodeTitleBar();
		ImGui::Text("Main Node");
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
			if (pin->GetOutputPin())
				ImNodes::Link(pin->GetID(), pin->GetID(), pin->GetOutputPin()->GetID());	// Links have the same ID than its input pin
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
}
