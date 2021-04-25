#include "kspch.h"
#include "MaterialEditorPanel.h"

#include "Core/Utils/Maths/RandomGenerator.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>
#include <glm/gtc/type_ptr.hpp>


namespace Kaimos {

	
	// ----------------------- Public Class Methods -------------------------------------------------------
	void MaterialEditorPanel::OnUIRender()
	{
		ImGui::Begin("Kaimos Material Editor");
		if (!m_MaterialToModify)
		{
			ImGui::End();
			return;
		}

		// -- Begin Editor --
		ImNodes::BeginNodeEditor();

		bool set_draggable = true;
		float indent = 5.0f, width = 30.0f;

		// -- Central Node Beginning --
		ImNodes::BeginNode(1);
		ImNodes::BeginNodeTitleBar();
		ImGui::Text("Central");
		ImNodes::EndNodeTitleBar();

		// -- Color Node --
		ImGui::Text("Color");
		ImGuiColorEditFlags color_flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs;
		ImGui::SameLine();
		ImGui::ColorEdit4("##spcompcolor", glm::value_ptr(m_MaterialToModify->Color), color_flags);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
			set_draggable = false;

		// -- Texture Node --
		if (m_MaterialToModify->SpriteTexture)
		{
			std::string tex_path = m_MaterialToModify->TextureFilepath;
			std::string tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1));

			ImGui::Text("Texture");
			ImGui::Indent(indent);
			ImGui::Text("Texture %i: '%s'", m_MaterialToModify->SpriteTexture->GetTextureID(), tex_name.c_str());
			ImGui::SameLine();
			ImGui::Text("(%ix%i)", m_MaterialToModify->SpriteTexture->GetWidth(), m_MaterialToModify->SpriteTexture->GetHeight());
		}


		// -- Texture Tiling Node --
		ImGui::Text("Texture Tiling");
		ImGui::SameLine(); ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##spcomptextiling", &m_MaterialToModify->TextureTiling, 0.2f);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
			set_draggable = false;


		// -- Texture UV Offset Node --
		ImGui::Text("Texture Offset");
		ImGui::SameLine(); ImGui::SetNextItemWidth(width * 2.0f);
		ImGui::DragFloat2("##spcomptexoffx", glm::value_ptr(m_MaterialToModify->TextureUVOffset), 0.2f);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
			set_draggable = false;

		// -- Central Node Dragging & Ending --
		ImNodes::SetNodeDraggable(1, set_draggable);
		ImNodes::EndNode();


		// ------ NODES --------------------------------------------------------
		// -- Create Node with V Key --
		if (Input::IsKeyDown(KEY::V))
			CreateNode();

		// -- Draw Nodes & its Pins --
		for (Ref<MaterialNode>& node : m_Nodes)
		{
			ImNodes::BeginNode(node->GetID());

			ImNodes::BeginOutputAttribute(node->GetOutputPin()->ID);
			ImGui::Indent(40.0f);
			ImGui::Text(node->GetOutputPin()->Name.c_str());
			ImNodes::EndOutputAttribute();

			for (Ref<MaterialNodePin> pin : *node->GetInputPins())
			{
				ImNodes::BeginInputAttribute(pin->ID);
				ImGui::Text(pin->Name.c_str());
				ImNodes::EndInputAttribute();
			}

			ImNodes::EndNode();
		}

		// -- Draw Links between Node Pins --
		for (Ref<MaterialNode>& node : m_Nodes)
		{
			for (Ref<MaterialNodePin> pin : *node->GetInputPins())
				if (pin->OutputPinLinked)
					ImNodes::Link(pin->ID, pin->ID, pin->OutputPinLinked->ID);
		}

		// -- End Material Node Editor --
		ImNodes::EndNodeEditor();
		
		// -- Create Links between Node Pins --
		int start, end;
		if (ImNodes::IsLinkCreated(&start, &end))
		{
			MaterialNodePin* out_pin = FindNodePin(start);	// Output pin (start)
			MaterialNodePin* in_pin = FindNodePin(end);		// Input pin (end)

			if (in_pin && out_pin)
			{
				// Check if the node of the input pin is connected to the node of the output pin
				bool connect = true;
				MaterialNodePin* inpin_nodeoutput = in_pin->OwnerNode->GetOutputPin();
				if (inpin_nodeoutput)
				{
					for (Ref<MaterialNodePin> outnode_inpin : *out_pin->OwnerNode->GetInputPins())
						if (outnode_inpin->OutputPinLinked && outnode_inpin->OutputPinLinked->ID == inpin_nodeoutput->ID)
							connect = false;
				}

				if(connect)
					in_pin->OutputPinLinked = out_pin;
			}
		}


		// -- End Editor --
		ImGui::End();
	}


	void MaterialEditorPanel::LoadIniEditorSettings() const
	{
		UnsetMaterialToModify();
		ImNodes::LoadCurrentEditorStateFromIniFile("imnode.ini");
	}
	

	void MaterialEditorPanel::SaveIniEditorSettings() const
	{
		ImNodes::SaveCurrentEditorStateToIniFile("imnode.ini");
	}



	// ----------------------- Public Material Methods ----------------------------------------------------
	void MaterialEditorPanel::CreateNode()
	{
		uint id = (uint)Kaimos::Random::GetRandomInt();
		Ref<MaterialNode> mat_node = CreateRef<MaterialNode>(id, "Node_" + std::to_string(id));

		mat_node->AddPin(false);
		mat_node->AddPin(true);
		mat_node->AddPin(true);

		m_Nodes.push_back(mat_node);
	}

	MaterialNodePin* MaterialEditorPanel::FindNodePin(uint pin_id)
	{
		for (Ref<MaterialNode>& node : m_Nodes)
		{
			if (node->GetOutputPin()->ID == pin_id)
				return node->GetOutputPin();

			int index = node->FindInputPinIndex(pin_id);
			if (index != -1)
				return node->GetInputPinAt(index);
		}

		return nullptr;
	}

	void MaterialEditorPanel::UnsetMaterialToModify() const
	{
		if (m_MaterialToModify)
			m_MaterialToModify->InMaterialEditor = false;

		m_MaterialToModify = nullptr;
	}

	void MaterialEditorPanel::SetMaterialToModify(SpriteRendererComponent* sprite_component) const
	{
		if(m_MaterialToModify)
			m_MaterialToModify->InMaterialEditor = false;

		m_MaterialToModify = sprite_component;
		m_MaterialToModify->InMaterialEditor = true;
	}
}
