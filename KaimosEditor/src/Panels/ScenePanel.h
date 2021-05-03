#ifndef _SCENEPANEL_H_
#define _SCENEPANEL_H_

#include "Kaimos.h"
#include "MaterialEditorPanel.h"

namespace Kaimos {

	class ScenePanel
	{
	public:

		// --- Public Class Methods ---
		ScenePanel() = default;
		ScenePanel(const Ref<Scene>& context, MaterialEditorPanel* material_editor_panel);
		
		void OnUIRender(bool& closing_bool);

		// --- Getters/Setters ---
		void SetContext(const Ref<Scene>& context);

		Entity GetSelectedEntity()						const	{ return m_SelectedEntity; }
		void SetSelectedEntity(Entity entity)					{ m_SelectedEntity = entity; }

	private:

		// --- Private Scene Methods ---
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity& entity);

		template<typename T, typename UIFunction>
		void DrawComponentUI(const std::string& name, Entity entity, UIFunction function);

	private:

		MaterialEditorPanel* m_KMEPanel = nullptr;
		Ref<Scene> m_SceneContext = nullptr;
		Entity m_SelectedEntity = {};
	};
}

#endif //_SCENEPANEL_H_
