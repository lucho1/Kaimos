#ifndef _SCENEPANEL_H_
#define _SCENEPANEL_H_

#include "Kaimos.h"
#include "Core/Core.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include <glm/glm.hpp>

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
		void DrawComponents(Entity entity);

	private:

		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
}

#endif
