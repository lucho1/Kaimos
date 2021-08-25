#include "kspch.h"
#include "MaterialEditorPanel.h"
#include "Core/Utils/Maths/Maths.h"
#include "Renderer/Renderer.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>


namespace Kaimos {
	
	
	// ----------------------- Public Class Methods -------------------------------------------------------
	void MaterialEditorPanel::OnUIRender()
	{
		// -- Set Window Always on Top --
		ImGuiWindowClass wnd_class;
		wnd_class.ViewportFlagsOverrideSet = ImGuiViewportFlags_TopMost;
		ImGui::SetNextWindowClass(&wnd_class);
		
		// -- Set size if needed --
		static ImVec2 original_size = ImVec2(720.0f, 406.0f);
		if (m_Resize)
		{
			if (m_Maximized)
				ImGui::SetNextWindowSize(original_size);
			else
			{
				ImGui::SetNextWindowSize(ImGui::GetWindowViewport()->Size);
				ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
			}

			m_Maximized = !m_Maximized;
			m_Resize = false;
		}
		
		// -- Begin Window --
		ImGui::Begin("Kaimos Material Editor", &ShowPanel);
		if (Maths::CompareFloats(ImGui::GetWindowViewport()->Size.x, ImGui::GetWindowWidth()) && Maths::CompareFloats(ImGui::GetWindowViewport()->Size.y, ImGui::GetWindowHeight()))
			m_Maximized = true;
		else
			m_Maximized = false;

		// -- Right Click Options --
		if (!m_EditorHovered)
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (m_Maximized)
				{
					if (ImGui::MenuItem("Restore"))
						m_Resize = true;
				}
				else
				{
					if (ImGui::MenuItem("Maximize"))
					{
						float x = ImGui::GetWindowPos().x + ImGui::GetWindowSize().x;
						float y = ImGui::GetWindowPos().y + ImGui::GetWindowSize().y;
						original_size = ImVec2(x, y);
						m_Resize = true;
					}
				}

				ImGui::EndPopup();
			}
		}

		// -- Early Exit if !Graph --
		if (!m_CurrentGraph)
		{
			m_EditorHovered = false;
			ImGui::End();
			return;
		}

		// -- Compile Material Button --
		if (ImGui::Button("Compile"))
			m_SceneContext->UpdateMeshAndSpriteComponentsVertices(m_CurrentGraph->GetMaterialAttachedID());

		// -- Begin Editor --
		ImNodes::BeginNodeEditor();
		m_EditorHovered = ImNodes::IsEditorHovered();

		uint pushvars_num = PushImGuiStyleVars();
		ImNodes::PushColorStyle(ImNodesCol_NodeOutline, IM_COL32(0.0f, 142.0f, 255.0f, 40.0f));
		ImNodes::PushStyleVar(ImNodesStyleVar_NodeBorderThickness, 2.0f);
		ImNodes::PushStyleVar(ImNodesStyleVar_PinCircleRadius, 4.5f);
		ImNodes::PushStyleVar(ImNodesStyleVar_PinHoverRadius, 4.5f);

		// -- Right-Click Popup (for Nodes Creation) --
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			ImVec2 popup_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
			DrawRightClickPopup(popup_pos);
			ImGui::EndPopup();
		}

		// -- Draw Nodes, Pins & Links --
		m_CurrentGraph->DrawNodes();

		// -- End Material Node Editor --
		ImGui::PopStyleVar(static_cast<int>(pushvars_num));
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
		ImNodes::PopStyleVar();
		ImNodes::PopStyleVar();
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


	uint MaterialEditorPanel::PushImGuiStyleVars()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 4.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 12.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);

		// Num of PushStyleVar() calls to pop them later
		return 7;
	}


	void MaterialEditorPanel::DrawRightClickPopup(ImVec2 popup_pos)
	{
		// Global Nodes
		if (ImGui::BeginMenu("Vertex Attributes"))
		{
			if (ImGui::MenuItem("TCoords"))			m_CurrentGraph->CreateNode(MaterialEditor::VertexParameterNodeType::TEX_COORDS, popup_pos);
			if (ImGui::MenuItem("Vertex Position"))	m_CurrentGraph->CreateNode(MaterialEditor::VertexParameterNodeType::POSITION, popup_pos);
			if (ImGui::MenuItem("Vertex Normal"))	m_CurrentGraph->CreateNode(MaterialEditor::VertexParameterNodeType::NORMAL, popup_pos);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Constants"))
		{
			if (ImGui::MenuItem("Time"))		m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::DELTATIME, popup_pos);
			if (ImGui::MenuItem("PI"))			m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::PI, popup_pos);
			if (ImGui::MenuItem("Scene Color"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::SCENE_COLOR, popup_pos);
			if (ImGui::MenuItem("Screen Resolution")) m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::SCREEN_RES, popup_pos);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::MenuItem("FOV"))			m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::CAMERA_FOV, popup_pos);
			if (ImGui::MenuItem("Aspect Ratio"))m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::CAMERA_AR, popup_pos);
			if (ImGui::MenuItem("Clip Planes"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::CAMERA_PLANES, popup_pos);
			if (ImGui::MenuItem("Ortho Size"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::CAMERA_ORTHOSIZE, popup_pos);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Variables"))
		{
			if (ImGui::MenuItem("Int"))		m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::INT, popup_pos);
			if (ImGui::MenuItem("Float"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::FLOAT, popup_pos);
			if (ImGui::MenuItem("Vec2"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::VEC2, popup_pos);
			if (ImGui::MenuItem("Vec3"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::VEC3, popup_pos);
			if (ImGui::MenuItem("Vec4"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::VEC4, popup_pos);
			ImGui::EndMenu();
		}

		// Maths Nodes
		ImGui::Separator();
		if (ImGui::BeginMenu("Maths"))
		{
			if (ImGui::BeginMenu("Random"))
			{
				if (ImGui::MenuItem("Int"))		m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::INT_RANDOM, popup_pos);
				if (ImGui::MenuItem("Float"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::FLOAT_RANDOM, popup_pos);
				if (ImGui::MenuItem("Vec2"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::VEC2_RANDOM, popup_pos);
				if (ImGui::MenuItem("Vec3"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::VEC3_RANDOM, popup_pos);
				if (ImGui::MenuItem("Vec4"))	m_CurrentGraph->CreateNode(MaterialEditor::ConstantNodeType::VEC4_RANDOM, popup_pos);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Basics"))
			{
				if (ImGui::BeginMenu("Sum"))
					DrawSameTypesOperationNodesMenu(MaterialEditor::OperationNodeType::ADDITION, "+", popup_pos);

				if (ImGui::BeginMenu("Subtract"))
					DrawSameTypesOperationNodesMenu(MaterialEditor::OperationNodeType::SUBTRACTION, "-", popup_pos);

				if (ImGui::BeginMenu("Multiply"))
					DrawSameTypesOperationNodesMenu(MaterialEditor::OperationNodeType::MULTIPLICATION, "*", popup_pos);

				if (ImGui::BeginMenu("Divide"))
					DrawSameTypesOperationNodesMenu(MaterialEditor::OperationNodeType::DIVISION, "/", popup_pos);

				if (ImGui::BeginMenu("Num * Vec"))
					DrawFloatVecOperationNodesMenu(MaterialEditor::OperationNodeType::FLOATVEC_MULTIPLY, "*", popup_pos);

				if (ImGui::BeginMenu("Num / Vec"))
					DrawFloatVecOperationNodesMenu(MaterialEditor::OperationNodeType::FLOATVEC_DIVIDE, "/", popup_pos);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Advanced"))
			{
				if (ImGui::BeginMenu("Absolute"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::ABS, popup_pos, false);

				if (ImGui::BeginMenu("Min"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::MIN, popup_pos, false);

				if (ImGui::BeginMenu("Max"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::MAX, popup_pos, false);

				if (ImGui::BeginMenu("Negate"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::NEGATE, popup_pos, false);

				if (ImGui::BeginMenu("Power"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::POWER, popup_pos, false);

				if (ImGui::BeginMenu("Sqrt"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::SQUARE_ROOT, popup_pos, false);

				if (ImGui::BeginMenu("Inv. Sqrt"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::INVERSE_SQUARE_ROOT, popup_pos, false);

				if (ImGui::BeginMenu("Log"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::LOG, popup_pos, false);

				if (ImGui::BeginMenu("Log2"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::LOG2, popup_pos, false);

				if (ImGui::BeginMenu("Exp"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::EXP, popup_pos, false);

				if (ImGui::BeginMenu("Exp2"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::EXP2, popup_pos, false);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Shaders Functions"))
			{
				if (ImGui::BeginMenu("Step by Float"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::FLOAT_STEP, popup_pos, false);

				if (ImGui::BeginMenu("Step by Vec"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_STEP, popup_pos, true);

				if (ImGui::BeginMenu("Smoothstep by Float"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::FLOAT_SMOOTHSTEP, popup_pos, false);

				if (ImGui::BeginMenu("Smoothstep by Vec"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_SMOOTHSTEP, popup_pos, true);

				if (ImGui::BeginMenu("Ceil"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::CEIL, popup_pos, false);

				if (ImGui::BeginMenu("Floor"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::FLOOR, popup_pos, false);

				if (ImGui::BeginMenu("Clamp"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::CLAMP, popup_pos, false);

				if (ImGui::BeginMenu("Round"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::ROUND, popup_pos, false);

				if (ImGui::BeginMenu("Sign"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::SIGN, popup_pos, false);

				if (ImGui::BeginMenu("Fractal"))
					DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::FRACTAL, popup_pos, false);

				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("Vectors"))
			{
				if (ImGui::BeginMenu("Basics"))
				{
					if (ImGui::BeginMenu("Vec Normalize"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_NORMALIZE, popup_pos, true);

					if (ImGui::BeginMenu("Vec Magnitude"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_MAGNITUDE, popup_pos, true);

					if (ImGui::BeginMenu("Vec Distance"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_DIST, popup_pos, true);

					if (ImGui::BeginMenu("Vec Dot"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_DOT, popup_pos, true);

					if (ImGui::BeginMenu("Vec Cross"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_CROSS, popup_pos, true);

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Advanced"))
				{
					if (ImGui::BeginMenu("Vec Reflect"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_REFLECT, popup_pos, true);

					if (ImGui::BeginMenu("Vec Refract"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_REFRACT, popup_pos, true);

					if (ImGui::BeginMenu("Lerp by Float"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::FLOAT_LERP, popup_pos, false);

					if (ImGui::BeginMenu("Lerp by Vec"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_LERP, popup_pos, true);

					if (ImGui::BeginMenu("Mod by Float"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::FLOAT_MOD, popup_pos, false);

					if (ImGui::BeginMenu("Mod by Vec"))
						DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType::VEC_MOD, popup_pos, true);

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}


	void MaterialEditorPanel::DrawSameTypesOperationNodesMenu(MaterialEditor::OperationNodeType op_type, const std::string& operator_str, ImVec2 popup_pos)
	{
		if (ImGui::MenuItem(std::string("Float " + operator_str + " Float").c_str()))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::FLOAT, popup_pos);

		if (ImGui::MenuItem(std::string("Int " + operator_str + " Int").c_str()))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::INT, popup_pos);

		if (ImGui::MenuItem(std::string("Vec2 " + operator_str + " Vec2").c_str()))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::VEC2, popup_pos);

		if (ImGui::MenuItem(std::string("Vec3 " + operator_str + " Vec3").c_str()))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::VEC3, popup_pos);

		if (ImGui::MenuItem(std::string("Vec4 " + operator_str + " Vec4").c_str()))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::VEC4, popup_pos);

		ImGui::EndMenu();
	}

	void MaterialEditorPanel::DrawFloatVecOperationNodesMenu(MaterialEditor::OperationNodeType op_type, const std::string& operator_str, ImVec2 popup_pos)
	{
		if (ImGui::MenuItem(std::string("Num " + operator_str + " Vec2").c_str()))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::VEC2, popup_pos);
		
		if (ImGui::MenuItem(std::string("Num " + operator_str + " Vec3").c_str()))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::VEC3, popup_pos);
		
		if (ImGui::MenuItem(std::string("Num " + operator_str + " Vec4").c_str()))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::VEC4, popup_pos);

		ImGui::EndMenu();
	}

	void MaterialEditorPanel::DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType op_type, ImVec2 popup_pos, bool only_vec_types)
	{
		if (!only_vec_types)
		{
			if (ImGui::MenuItem("Int"))
				m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::INT, popup_pos);

			if (ImGui::MenuItem("Float"))
				m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::FLOAT, popup_pos);
		}

		if (ImGui::MenuItem("Vec2"))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::VEC2, popup_pos);

		if (ImGui::MenuItem("Vec3"))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::VEC3, popup_pos);

		if (ImGui::MenuItem("Vec4"))
			m_CurrentGraph->CreateNode(op_type, MaterialEditor::PinDataType::VEC4, popup_pos);

		ImGui::EndMenu();
	}



	// ----------------------- Public Material Editor Panel Methods ---------------------------------------
	void MaterialEditorPanel::UnsetGraphToModify()
	{
		SaveCurrentGraphSettings();
		m_CurrentGraph = nullptr;
	}
	
	void MaterialEditorPanel::SetGraphToModifyFromMaterial(uint material_id)
	{
		SaveCurrentGraphSettings();
		Ref<Material> mat = Renderer::GetMaterial(material_id);
		if (mat && (mat)->m_AttachedGraph)
		{
			m_CurrentGraph = (mat)->m_AttachedGraph.get();
			LoadCurrentGraphSettings();
			ShowPanel = true;
		}
	}

	bool MaterialEditorPanel::IsModifyingMaterialGraph(Material* material)
	{
		if (material && m_CurrentGraph && m_CurrentGraph->GetID() == material->GetAttachedGraphID())
			return true;

		return false;
	}

	void MaterialEditorPanel::SerializeGraphs()
	{
		uint material_modifying = 0;
		if (m_CurrentGraph)
			material_modifying = m_CurrentGraph->GetMaterialAttachedID();

		uint materials_size = Renderer::GetMaterialsQuantity();
		for (uint i = 0; i < materials_size; ++i)
		{
			Ref<Material> mat = Renderer::GetMaterialFromIndex(i);
			if (mat)
			{
				SetGraphToModifyFromMaterial(mat->GetID());
				m_CurrentGraph->SaveEditorSettings();
			}
		}

		if (material_modifying != 0)
			SetGraphToModifyFromMaterial(material_modifying);
		else
			UnsetGraphToModify();
	}

}
