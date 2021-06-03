#ifndef _MATERIALEDITORPANEL_H_
#define _MATERIALEDITORPANEL_H_

#include "Kaimos.h"
#include "Renderer/MaterialEditor/MaterialGraph.h"


namespace Kaimos {

	class MaterialEditorPanel
	{
	public:

		// --- Public Class Methods ---
		MaterialEditorPanel() = default;
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

		// --- Private Material Editor Panel Methods ---
		void DeleteSelection(int selected_links, int selected_nodes);

	public:

		// --- Public Variables ---
		bool ShowPanel = true;

	private:

		// --- Private Variables ---
		MaterialEditor::MaterialGraph* m_CurrentGraph = nullptr;
	};
}

#endif //_MATERIALEDITORPANEL_H_
