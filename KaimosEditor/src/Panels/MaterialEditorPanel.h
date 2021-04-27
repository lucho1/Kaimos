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
		~MaterialEditorPanel()
		{
			for (Ref<MaterialNode>& matnode_ref : m_Nodes)
				matnode_ref.reset();

			m_Nodes.clear();
		}

		void OnUIRender();

		void LoadIniEditorSettings() const;
		void SaveIniEditorSettings() const;

	public:

		// --- Public Material Methods ---
		void UnsetMaterialToModify() const;
		void SetMaterialToModify(SpriteRendererComponent* sprite_component) const;

	private:

		// --- Private Material Editor Methods ---
		void CreateNode();
		void DeleteNode(uint node_id);
		void DeleteLink(uint pin_id);
		MaterialNodePin* FindNodePin(uint pin_id);
		MaterialNode* FindNode(uint node_id);

	private:

		// --- Variables ---
		std::vector<Ref<MaterialNode>> m_Nodes;
		mutable SpriteRendererComponent* m_MaterialToModify = nullptr;
	};
}

#endif //_MATERIALEDITORPANEL_H_
