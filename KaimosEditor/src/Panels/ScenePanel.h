#ifndef _SCENEPANEL_H_
#define _SCENEPANEL_H_

#include "Core/Core.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

namespace Kaimos {

	class ScenePanel
	{
		//friend class Scene;
	public:

		ScenePanel() = default;
		ScenePanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);
		void OnUIRender();

	private:

		void DrawEntityNode(Entity entity);

	private:

		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
}

#endif