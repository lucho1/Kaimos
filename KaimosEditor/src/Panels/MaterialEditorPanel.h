#ifndef _MATERIALEDITORPANEL_H_
#define _MATERIALEDITORPANEL_H_

#include "Kaimos.h"
#include "Renderer/MaterialEditor/MaterialGraph.h"

struct ImVec2;

namespace Kaimos {

	class MaterialEditorPanel
	{
	public:

		// --- Public Class Methods ---
		MaterialEditorPanel() = default;
		MaterialEditorPanel(const Ref<Scene>& scene_context) : m_SceneContext(scene_context) {}
		~MaterialEditorPanel() = default;

		void OnUIRender();

	public:

		// --- Public Material Editor Panel Methods ---
		void UnsetGraphToModify();
		void SetGraphToModifyFromMaterial(uint material_id);

		bool IsModifyingMaterialGraph(Material* material);

		void LoadCurrentGraphSettings() const { if (m_CurrentGraph) m_CurrentGraph->LoadEditorSettings(); }
		void SaveCurrentGraphSettings() const { if (m_CurrentGraph) m_CurrentGraph->SaveEditorSettings(); }

		void SerializeGraphs();

	private:

		// --- Private Editor Panel UI Methods ---
		void DeleteSelection(int selected_links, int selected_nodes);
		uint PushImGuiStyleVars();
		void DrawRightClickPopup(ImVec2 popup_pos);

		void DrawSameTypesOperationNodesMenu(MaterialEditor::OperationNodeType op_type, const std::string& operator_str, ImVec2 popup_pos);
		void DrawFloatVecOperationNodesMenu(MaterialEditor::OperationNodeType op_type, const std::string& operator_str, ImVec2 popup_pos);
		void DrawSpecialOperationNodesMenu(MaterialEditor::SpecialOperationNodeType op_type, ImVec2 popup_pos, bool only_vec_types, bool add_vec2 = true);
		void DrawBreakVectorSubmenu(ImVec2 popup_pos);

	public:

		// --- Public Variables ---
		bool ShowPanel = true;

	private:

		// --- Private Variables ---
		MaterialEditor::MaterialGraph* m_CurrentGraph = nullptr;
		Ref<Scene> m_SceneContext = nullptr;

		bool m_EditorHovered = false;
		bool m_Resize = false;
		bool m_Maximized = false;
	};
}

#endif //_MATERIALEDITORPANEL_H_
