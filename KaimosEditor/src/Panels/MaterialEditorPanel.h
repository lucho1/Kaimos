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
		void OnUIRender();

		void LoadIniEditorSettings() const;
		void SaveIniEditorSettings() const;

	public:

		// --- Public Material Methods ---
		void UnsetMaterialToModify() const;
		void SetMaterialToModify(SpriteRendererComponent* sprite_component) const;
		void CreateNode();

		MaterialNodePin* FindNodePin(uint pin_id);

	private:

		// --- Variables ---
		std::vector<Ref<MaterialNode>> m_Nodes;
		mutable SpriteRendererComponent* m_MaterialToModify = nullptr;
	};
}

#endif //_MATERIALEDITORPANEL_H_
