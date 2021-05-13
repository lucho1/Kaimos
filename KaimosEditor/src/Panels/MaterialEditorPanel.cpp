#include "kspch.h"
#include "MaterialEditorPanel.h"
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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 12.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);

		ImNodes::PushColorStyle(ImNodesCol_NodeOutline, IM_COL32(0.0f, 142.0f, 255.0f, 40.0f));
		ImNodes::PushStyleVar(ImNodesStyleVar_NodeBorderThickness, 2.0f);
		

		// -- Nodes Creation Right-Click Popup --
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Texture Coordinates"))
				m_CurrentGraph->CreateNode(MaterialEditor::VertexParameterNodeType::TEX_COORDS);

			if (ImGui::MenuItem("Vertex Position"))
				m_CurrentGraph->CreateNode(MaterialEditor::VertexParameterNodeType::POSITION);

			if (ImGui::MenuItem("Vertex Normal"))
				m_CurrentGraph->CreateNode(MaterialEditor::VertexParameterNodeType::NORMAL);

			if (ImGui::MenuItem("Delta Time"))
				m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::DELTATIME);

			if (ImGui::MenuItem("Float + Float"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::ADDITION, MaterialEditor::PinDataType::FLOAT);

			if (ImGui::MenuItem("Int + Int"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::ADDITION, MaterialEditor::PinDataType::INT);

			if (ImGui::MenuItem("Vec2 + Vec2"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::ADDITION, MaterialEditor::PinDataType::VEC2);

			if (ImGui::MenuItem("Vec3 + Vec3"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::ADDITION, MaterialEditor::PinDataType::VEC3);

			if (ImGui::MenuItem("Vec4 + Vec4"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::ADDITION, MaterialEditor::PinDataType::VEC4);

			if (ImGui::MenuItem("Float * Float"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::MULTIPLICATION, MaterialEditor::PinDataType::FLOAT);

			if (ImGui::MenuItem("Int * Int"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::MULTIPLICATION, MaterialEditor::PinDataType::INT);

			if (ImGui::MenuItem("Vec2 * Vec2"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::MULTIPLICATION, MaterialEditor::PinDataType::VEC2);

			if (ImGui::MenuItem("Float * Vec2"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::FLOATVEC2_MULTIPLY, MaterialEditor::PinDataType::FLOAT);

			if (ImGui::MenuItem("Vec3 * Vec3"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::MULTIPLICATION, MaterialEditor::PinDataType::VEC3);

			if (ImGui::MenuItem("Vec4 * Vec4"))
				m_CurrentGraph->CreateNode(MaterialEditor::OperationNodeType::MULTIPLICATION, MaterialEditor::PinDataType::VEC4);
		
			ImGui::EndPopup();
		}
		

		// -- Draw Nodes, Pins & Links --
		m_CurrentGraph->DrawNodes();

		// -- End Material Node Editor --
		ImGui::PopStyleVar(7);
		ImNodes::PopColorStyle();
		ImNodes::PopStyleVar();

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
	
	void MaterialEditorPanel::SetGraphToModifyFromMaterial(uint material_id)
	{
		Ref<Material> mat = Renderer::GetMaterial(material_id);
		if (mat && (mat)->m_AttachedGraph)
			m_CurrentGraph = (mat)->m_AttachedGraph.get();
	}

	bool MaterialEditorPanel::IsModifyingMaterialGraph(Material* material)
	{
		if (m_CurrentGraph && m_CurrentGraph->GetID() == material->GetAttachedGraphID())
			return true;

		return false;
	}
}
