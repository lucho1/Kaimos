#include "kspch.h"
#include "MaterialEditorPanel.h"

#include "Core/Utils/Maths/RandomGenerator.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>
#include <glm/gtc/type_ptr.hpp>


namespace Kaimos {
	
	
	// ----------------------- Public Class Methods -------------------------------------------------------
	void MaterialEditorPanel::Start()
	{
		m_MainMatNode = CreateRef<MainMaterialNode>();
		m_Nodes.push_back(m_MainMatNode);
	}

	void MaterialEditorPanel::CleanUp()
	{
		for (Ref<MaterialNode>& matnode_ref : m_Nodes)
			matnode_ref.reset();

		m_Nodes.clear();
		m_MainMatNode.reset();
	}	
	

	void MaterialEditorPanel::OnUIRender()
	{
		ImGui::Begin("Kaimos Material Editor");
		if (!m_MainMatNode->m_MaterialToModify)
		{
			ImGui::End();
			return;
		}

		// -- Begin Editor --
		ImNodes::BeginNodeEditor();

		bool set_draggable = true;
		float indent = 5.0f, width = 30.0f;

		
		// ----------- CENTRAL HARDCODED NODE -------------------------------------------------
		// -- Central Node Beginning --
		ImNodes::BeginNode(1);
		ImNodes::BeginNodeTitleBar();
		ImGui::Text("Central");
		ImNodes::EndNodeTitleBar();

		// -- Color Node --
		ImGui::Text("Color");
		ImGuiColorEditFlags color_flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs;
		ImGui::SameLine();
		ImGui::ColorEdit4("##spcompcolor", glm::value_ptr(m_MainMatNode->m_MaterialToModify->Color), color_flags);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
			set_draggable = false;

		// -- Texture Node --
		if (m_MainMatNode->m_MaterialToModify->SpriteTexture)
		{
			std::string tex_path = m_MainMatNode->m_MaterialToModify->TextureFilepath;
			std::string tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1));

			ImGui::Text("Texture");
			ImGui::Indent(indent);
			ImGui::Text("Texture %i: '%s'", m_MainMatNode->m_MaterialToModify->SpriteTexture->GetTextureID(), tex_name.c_str());
			ImGui::SameLine();
			ImGui::Text("(%ix%i)", m_MainMatNode->m_MaterialToModify->SpriteTexture->GetWidth(), m_MainMatNode->m_MaterialToModify->SpriteTexture->GetHeight());
		}

		// -- Texture Tiling Node --
		ImGui::Text("Texture Tiling");
		ImGui::SameLine(); ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##spcomptextiling", &m_MainMatNode->m_MaterialToModify->TextureTiling, 0.2f);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
			set_draggable = false;


		// -- Texture UV Offset Node --
		ImGui::Text("Texture Offset");
		ImGui::SameLine(); ImGui::SetNextItemWidth(width * 2.0f);
		ImGui::DragFloat2("##spcomptexoffx", glm::value_ptr(m_MainMatNode->m_MaterialToModify->TextureUVOffset), 0.2f);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
			set_draggable = false;

		// -- Central Node Dragging & Ending --
		ImNodes::SetNodeDraggable(1, set_draggable);
		ImNodes::EndNode();


		// ----------- NODES ------------------------------------------------------------------
		// -- Right-Click Popup --
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Node"))
				CreateNode();
		
			ImGui::EndPopup();
		}

		if(Input::IsKeyDown(KEY::V))
			CreateNode();

		// -- Draw Nodes, Pins & Links --
		for (Ref<MaterialNode>& node : m_Nodes)
			node->DrawNodeUI();

		// -- End Material Node Editor --
		ImNodes::EndNodeEditor();
		
		// -- Create Links between Node Pins --
		int start, end;
		if (ImNodes::IsLinkCreated(&start, &end))
		{
			// Input pin is always end while Output pin is always start
			MaterialNodePin* in_pin = FindNodePin(end);
			if (in_pin)
				in_pin->LinkPin(FindNodePin(start));
		}

		// -- Check for Links & Nodes Destroys --
		// Link dragged to empty space
		int destroyed_id;
		if (ImNodes::IsLinkDestroyed(&destroyed_id))
			DeleteLink((uint)destroyed_id);

		// Link is double right-clicked
		int item_hovered;
		if (ImGui::GetIO().MouseDoubleClicked[2] && ImNodes::IsLinkHovered(&item_hovered))
			DeleteLink((uint)item_hovered);
		
		// Node is double right-clicked
		if (ImGui::GetIO().MouseDoubleClicked[2] && ImNodes::IsNodeHovered(&item_hovered))
			DeleteNode((uint)item_hovered);

		// SUPR/DEL pressed + nodes/link selection
		if (Input::IsKeyDown(KEY::DEL))
		{
			int selected_links = ImNodes::NumSelectedLinks(), selected_nodes = ImNodes::NumSelectedNodes();
			if (selected_links > 0 || selected_nodes > 0)
				DeleteSelection(selected_links, selected_nodes);
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
		if (node_id == m_MainMatNode->GetID())
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

			//m_MainMatNode->m_MaterialToModify->TextureTiling = in_pin->Value = in_pin->DefaultValue;
		}
	}

	void MaterialEditorPanel::DeleteSelection(int selected_links, int selected_nodes)
	{
		if (selected_links > 0)
		{
			int* del_links = new int[selected_links];
			ImNodes::GetSelectedLinks(del_links);
			for (uint i = 0; i < selected_links; ++i)
				DeleteLink((uint)del_links[i]);

			delete[] del_links;
		}
		
		if (selected_nodes > 0)
		{
			int* del_nodes = new int[selected_nodes];
			ImNodes::GetSelectedNodes(del_nodes);
			for (uint i = 0; i < selected_nodes; ++i)
				DeleteNode((uint)del_nodes[i]);

			delete[] del_nodes;
		}
	}

	MaterialNodePin* MaterialEditorPanel::FindNodePin(uint pin_id)
	{
		for (Ref<MaterialNode>& node : m_Nodes)
		{
			if (node->GetID() != m_MainMatNode->GetID() && node->GetOutputPin()->ID == pin_id)
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



	// ----------------------- Public Material Editor Methods ---------------------------------------------
	void MaterialEditorPanel::UnsetMaterialToModify() const
	{
		if (!m_MainMatNode)
			return;

		if (m_MainMatNode->m_MaterialToModify)
			m_MainMatNode->m_MaterialToModify->InMaterialEditor = false;


		m_MainMatNode->m_TextureTilingPin->DefaultValue = m_MainMatNode->m_TextureTilingPin->Value = 1.0f;
		m_MainMatNode->m_MaterialToModify = nullptr;
	}

	void MaterialEditorPanel::SetMaterialToModify(SpriteRendererComponent* sprite_component) const
	{
		if (!m_MainMatNode)
			return;

		if(m_MainMatNode->m_MaterialToModify)
			m_MainMatNode->m_MaterialToModify->InMaterialEditor = false;

		m_MainMatNode->m_MaterialToModify = sprite_component;
		m_MainMatNode->m_MaterialToModify->InMaterialEditor = true;

		m_MainMatNode->m_TextureTilingPin->DefaultValue = m_MainMatNode->m_TextureTilingPin->Value = m_MainMatNode->m_MaterialToModify->TextureTiling;
	}
}
