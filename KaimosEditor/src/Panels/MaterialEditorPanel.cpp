#include "kspch.h"
#include "MaterialEditorPanel.h"

#include "Core/Utils/Maths/RandomGenerator.h"
#include "Renderer/Renderer.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>


namespace Kaimos {
	
	
	// ----------------------- Public Class Methods -------------------------------------------------------
	void MaterialEditorPanel::OnUIRender()
	{
		ImGui::Begin("Kaimos Material Editor");
		if (!m_CurrentGraph)
		{
			ImGui::End();
			return;
		}

		// -- Begin Editor --
		ImNodes::BeginNodeEditor();
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 250.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
		//ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, ImVec2(20.0f, 20.0f));
		//ImGuiStyleVar_FrameBorderSize;
		//ImGuiStyleVar_FramePadding;
		//ImGuiStyleVar_PopupBorderSize;
		//ImGuiStyleVar_PopupRounding;
		
		
		
		// ----------- CENTRAL HARDCODED NODE -------------------------------------------------
		//bool set_draggable = true;
		//float indent = 5.0f, width = 30.0f;

		// -- Central Node Beginning --
		//
		//// -- Color Node --
		//ImGui::Text("Color");
		//ImGuiColorEditFlags color_flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs;
		//ImGui::SameLine();
		//ImGui::ColorEdit4("##spcompcolor", glm::value_ptr(m_MainMatNode->m_MaterialToModify->Color), color_flags);
		//
		//if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
		//	set_draggable = false;
		//
		//// -- Texture Node --
		//if (m_MainMatNode->m_MaterialToModify->SpriteTexture)
		//{
		//	std::string tex_path = m_MainMatNode->m_MaterialToModify->TextureFilepath;
		//	std::string tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1));
		//
		//	ImGui::Text("Texture");
		//	ImGui::Indent(indent);
		//	ImGui::Text("Texture %i: '%s'", m_MainMatNode->m_MaterialToModify->SpriteTexture->GetTextureID(), tex_name.c_str());
		//	ImGui::SameLine();
		//	ImGui::Text("(%ix%i)", m_MainMatNode->m_MaterialToModify->SpriteTexture->GetWidth(), m_MainMatNode->m_MaterialToModify->SpriteTexture->GetHeight());
		//}
		//
		//// -- Texture Tiling Node --
		//ImGui::Text("Texture Tiling");
		//ImGui::SameLine(); ImGui::SetNextItemWidth(width);
		//ImGui::DragFloat("##spcomptextiling", &m_MainMatNode->m_MaterialToModify->TextureTiling, 0.2f);
		//
		//if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
		//	set_draggable = false;
		//
		//
		//// -- Texture UV Offset Node --
		//ImGui::Text("Texture Offset");
		//ImGui::SameLine(); ImGui::SetNextItemWidth(width * 2.0f);
		//ImGui::DragFloat2("##spcomptexoffx", glm::value_ptr(m_MainMatNode->m_MaterialToModify->TextureUVOffset), 0.2f);
		//
		//if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
		//	set_draggable = false;
		//
		//// -- Central Node Dragging & Ending --
		//ImNodes::SetNodeDraggable(1, set_draggable);
		//ImNodes::EndNode();


		// ----------- NODES ------------------------------------------------------------------
		// -- Right-Click Popup --
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Texture Coordinates"))
				m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::TCOORDS);

			if (ImGui::MenuItem("Delta Time"))
				m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::DELTATIME);

			if (ImGui::MenuItem("Float + Float"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::ADDITION, MaterialEditor::PinDataType::FLOAT);

			if (ImGui::MenuItem("Int + Int"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::ADDITION, MaterialEditor::PinDataType::INT);

			if (ImGui::MenuItem("Vec2 + Vec2"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::ADDITION, MaterialEditor::PinDataType::VEC2);

			if (ImGui::MenuItem("Vec4 + Vec4"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::ADDITION, MaterialEditor::PinDataType::VEC4);

			if (ImGui::MenuItem("Float * Float"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::MULTIPLICATION, MaterialEditor::PinDataType::FLOAT);

			if (ImGui::MenuItem("Int * Int"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::MULTIPLICATION, MaterialEditor::PinDataType::INT);

			if (ImGui::MenuItem("Vec2 * Vec2"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::MULTIPLICATION, MaterialEditor::PinDataType::VEC2);

			if (ImGui::MenuItem("Vec4 * Vec4"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::MULTIPLICATION, MaterialEditor::PinDataType::VEC4);
		
			ImGui::EndPopup();
		}
		

		// -- Draw Nodes, Pins & Links --
		m_CurrentGraph->DrawNodes();

		// -- End Material Node Editor --
		//ImGui::PopStyleVar(8);
		ImNodes::EndNodeEditor();
		
		// -- Create Links between Node Pins --
		int start, end;
		if (ImNodes::IsLinkCreated(&start, &end))
			m_CurrentGraph->CreateLink(start, end); // Input pin is always end while Output pin is always start

		// -- Check for Links & Nodes Destroys --
		// Link dragged to empty space
		int destroyed_id;
		if (ImNodes::IsLinkDestroyed(&destroyed_id))
			m_CurrentGraph->DeleteLink((uint)destroyed_id);

		// Link is double right-clicked
		int item_hovered;
		if (ImGui::GetIO().MouseDoubleClicked[2] && ImNodes::IsLinkHovered(&item_hovered))
			m_CurrentGraph->DeleteLink((uint)item_hovered);
		
		// Node is double right-clicked
		if (ImGui::GetIO().MouseDoubleClicked[2] && ImNodes::IsNodeHovered(&item_hovered))
			m_CurrentGraph->DeleteNode((uint)item_hovered);

		// SUPR/DEL pressed + nodes/link selection
		if (Input::IsKeyDown(KEY::DEL) || ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Delete)))
		{
			int selected_links = ImNodes::NumSelectedLinks(), selected_nodes = ImNodes::NumSelectedNodes();
			if (selected_links > 0 || selected_nodes > 0)
				DeleteSelection(selected_links, selected_nodes);
		}

		// -- End Editor --
		ImGui::End();
	}


	
	// ----------------------- Private Material Editor Panel Methods --------------------------------------
	void MaterialEditorPanel::DeleteSelection(int selected_links, int selected_nodes)
	{
		if (selected_links > 0)
		{
			int* del_links = new int[selected_links];
			ImNodes::GetSelectedLinks(del_links);
			for (uint i = 0; i < selected_links; ++i)
				m_CurrentGraph->DeleteLink((uint)del_links[i]);

			delete[] del_links;
		}
		
		if (selected_nodes > 0)
		{
			int* del_nodes = new int[selected_nodes];
			ImNodes::GetSelectedNodes(del_nodes);
			for (uint i = 0; i < selected_nodes; ++i)
				m_CurrentGraph->DeleteNode((uint)del_nodes[i]);

			delete[] del_nodes;
		}
	}



	// ----------------------- Public Material Editor Panel Methods ---------------------------------------
	void MaterialEditorPanel::UnsetGraphToModify()
	{
		m_CurrentGraph = nullptr;
	}
	
	void MaterialEditorPanel::SetMaterialToModify(uint material_id)
	{
		Ref<Material> mat = Renderer::GetMaterial(material_id);
		if (mat)
			m_CurrentGraph = (mat)->m_AttachedGraph.get();
	}

	bool MaterialEditorPanel::IsModifyingMaterialGraph(Material* material)
	{
		if (m_CurrentGraph && m_CurrentGraph->GetID() == material->GetGraphID())
			return true;

		return false;
	}
}
