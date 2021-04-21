#ifndef _MATERIALEDITORPANEL_H_
#define _MATERIALEDITORPANEL_H_

#include "Kaimos.h"
#include "Scene/ECS/Components.h"

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

		// --- Private Scene Methods ---
		void UnsetMaterialToModify() const;
		void SetMaterialToModify(SpriteRendererComponent* sprite_component) const;


	private:

		// --- Variables ---
		mutable SpriteRendererComponent* m_MaterialToModify = nullptr;
	};
}

#endif //_MATERIALEDITORPANEL_H_
