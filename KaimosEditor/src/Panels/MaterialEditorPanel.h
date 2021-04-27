#ifndef _MATERIALEDITORPANEL_H_
#define _MATERIALEDITORPANEL_H_

#include "Kaimos.h"
#include "Scene/ECS/Components.h"
#include "../MaterialEditor/MaterialNode.h"

namespace Kaimos {

	class MaterialEditorPanel
	{
	public:

		// --- Public Class Methods ---
		MaterialEditorPanel() = default;
		~MaterialEditorPanel() { CleanUp(); }

		void Start();
		void CleanUp();
		void OnUIRender();

		void LoadIniEditorSettings() const;
		void SaveIniEditorSettings() const;

	public:

		// --- Public Material Editor Methods ---
		void UnsetMaterialToModify() const;
		void SetMaterialToModify(SpriteRendererComponent* sprite_component) const;

	private:

		// --- Private Material Editor Methods ---
		void CreateNode();
		void DeleteNode(uint node_id);
		void DeleteLink(uint pin_id);
		void DeleteSelection(int selected_links, int selected_nodes);

		MaterialNodePin* FindNodePin(uint pin_id);
		MaterialNode* FindNode(uint node_id);

	private:

		// --- Variables ---
		Ref<MainMaterialNode> m_MainMatNode = nullptr;

		std::vector<Ref<MaterialNode>> m_Nodes;
		//mutable SpriteRendererComponent* m_MaterialToModify = nullptr;
	};
}

#endif //_MATERIALEDITORPANEL_H_
