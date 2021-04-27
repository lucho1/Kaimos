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


		// ----------- NODES ------------------------------------------------------------------
		// -- Create Node Key --
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Node"))
				CreateNode();
		
			ImGui::EndPopup();
		}

		// -- Draw Central (Material) Node --
		ImNodes::BeginNode(m_CentralNode->GetID());

		ImNodes::BeginInputAttribute(m_TextureTilingPin->ID); // Texture Tiling Input
		ImGui::Text(m_TextureTilingPin->Name.c_str());
		ImNodes::EndInputAttribute();

		if (m_TextureTilingPin->OutputPinLinked)
		{
			m_MaterialToModify->TextureTiling = m_TextureTilingPin->OutputPinLinked->Value;
		}
		else
		{
			ImGui::SameLine(); ImGui::SetNextItemWidth(width);
			if(ImGui::DragFloat("##spcomptextiling", &m_TextureTilingPin->Value, 0.2f))
				m_MaterialToModify->TextureTiling = m_TextureTilingPin->Value;
		}

		ImNodes::EndNode();

		// -- Draw Nodes & its Pins --
		for (Ref<MaterialNode>& node : m_Nodes)
		{
			if (node->GetID() == m_CentralNode->GetID())
				continue;

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
			{
				if (pin->OutputPinLinked)
					ImNodes::Link(pin->ID, pin->ID, pin->OutputPinLinked->ID); // Links have the same ID than its input pin
			}
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

				if (connect)
				{
					in_pin->OutputPinLinked = out_pin;
					out_pin->InputPinsLinked.push_back(in_pin);

					m_TextureTilingPin->DefaultValue = m_TextureTilingPin->Value;
				}
			}
		}

		// -- Check for Links & Nodes Destroys --
		int destroyed_id;
		if (ImNodes::IsLinkDestroyed(&destroyed_id))										// If any link is dragged to an empty space
			DeleteLink((uint)destroyed_id);

		int item_hovered;
		if (ImGui::GetIO().MouseDoubleClicked[2] && ImNodes::IsLinkHovered(&item_hovered))	// If a link is right-clicked twice
			DeleteLink((uint)item_hovered);
		
		if (ImGui::GetIO().MouseDoubleClicked[2] && ImNodes::IsNodeHovered(&item_hovered))	// If a node is right-clicked twice
			DeleteNode((uint)item_hovered);

		if (Input::IsKeyDown(KEY::DEL))														// If there is a nodes/link selection and SUPR or DEL is pressed
		{
			int selected_links = ImNodes::NumSelectedLinks();
			if (selected_links > 0)
			{
				int* del_links = new int[selected_links];
				ImNodes::GetSelectedLinks(del_links);
				for (uint i = 0; i < selected_links; ++i)
					DeleteLink((uint)del_links[i]);

				delete[] del_links;
			}

			const int selected_nodes = ImNodes::NumSelectedNodes();
			if (selected_nodes > 0)
			{
				int* del_nodes = new int[selected_nodes];
				ImNodes::GetSelectedNodes(del_nodes);
				for (uint i = 0; i < selected_nodes; ++i)
					DeleteNode((uint)del_nodes[i]);

				delete[] del_nodes;
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


	
	// ----------------------- Private Material Editor Methods --------------------------------------------
	void MaterialEditorPanel::CreateNode()
	{
		uint id = (uint)Kaimos::Random::GetRandomInt();
		Ref<MaterialNode> mat_node = CreateRef<MaterialNode>(id, "Node_" + std::to_string(id));

		mat_node->AddPin(false);
		mat_node->AddPin(true);
		mat_node->AddPin(true);

		m_Nodes.push_back(mat_node);
	}

	void MaterialEditorPanel::DeleteNode(uint node_id)
	{
		if (node_id == m_CentralNode->GetID())
			return;

		std::vector<Ref<MaterialNode>>::const_iterator it = m_Nodes.begin();
		for (; it != m_Nodes.end(); ++it)
		{
			if ((*it)->GetID() == node_id)
			{
				m_Nodes.erase(it);
				return;
			}
		}
	}

	void MaterialEditorPanel::DeleteLink(uint pin_id)
	{
		MaterialNodePin* in_pin = FindNodePin(pin_id);
		if (in_pin)
		{
			in_pin->OutputPinLinked->InputPinsLinked.erase(std::find(in_pin->OutputPinLinked->InputPinsLinked.begin(), in_pin->OutputPinLinked->InputPinsLinked.end(), in_pin));
			in_pin->OutputPinLinked = nullptr;

			m_MaterialToModify->TextureTiling = in_pin->Value = in_pin->DefaultValue;
		}
	}

	MaterialNodePin* MaterialEditorPanel::FindNodePin(uint pin_id)
	{
		for (Ref<MaterialNode>& node : m_Nodes)
		{
			if (node->GetID() != m_CentralNode->GetID() && node->GetOutputPin()->ID == pin_id)
				return node->GetOutputPin();

			int index = node->FindInputPinIndex(pin_id);
			if (index != -1)
				return node->GetInputPinAt(index);
		}

		return nullptr;
	}

	MaterialNode* MaterialEditorPanel::FindNode(uint node_id)
	{
		for (Ref<MaterialNode>& node : m_Nodes)
		{
			if (node->GetID() == node_id)
				return node.get();
		}

		return nullptr;
	}



	// ----------------------- Public Material Methods ----------------------------------------------------
	void MaterialEditorPanel::UnsetMaterialToModify() const
	{
		if (m_MaterialToModify)
			m_MaterialToModify->InMaterialEditor = false;

		m_MaterialToModify = nullptr;

		if(m_TextureTilingPin)
			m_TextureTilingPin->DefaultValue = m_TextureTilingPin->Value = 1.0f;
	}

	void MaterialEditorPanel::SetMaterialToModify(SpriteRendererComponent* sprite_component) const
	{
		if(m_MaterialToModify)
			m_MaterialToModify->InMaterialEditor = false;

		m_MaterialToModify = sprite_component;
		m_MaterialToModify->InMaterialEditor = true;

		m_TextureTilingPin->DefaultValue = m_TextureTilingPin->Value = m_MaterialToModify->TextureTiling;
	}
}
