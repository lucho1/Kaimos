#ifndef _SCENEPANEL_H_
#define _SCENEPANEL_H_

#include "Kaimos.h"

namespace Kaimos {

	class ScenePanel
	{
	public:

		// --- Public Class Methods ---
		ScenePanel() = default;
		ScenePanel(const Ref<Scene>& context);
		
		void OnUIRender();

		// --- Getters/Setters ---
		void SetContext(const Ref<Scene>& context);

		Entity GetSelectedEntity()						const	{ return m_SelectedEntity; }
		void SetSelectedEntity(Entity entity)					{ m_SelectedEntity = entity; }

	private:

		// --- Private Scene Methods ---
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity& entity);

	private:

		Ref<Scene> m_SceneContext = nullptr;
		Entity m_SelectedEntity = {};
	};
}

#endif //_SCENEPANEL_H_
