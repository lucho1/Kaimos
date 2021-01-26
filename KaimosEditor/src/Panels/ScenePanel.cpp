#include "ScenePanel.h"

#include "Scene/Components.h"

#include <imgui.h>

namespace Kaimos {

	ScenePanel::ScenePanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void ScenePanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void ScenePanel::OnUIRender()
	{
		ImGui::Begin("Scene");

		m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID , m_Context.get() };
				DrawEntityNode(entity);				
			});

		ImGui::End();
	}

	void ScenePanel::DrawEntityNode(Entity entity)
	{
		std::string& tag = entity.GetComponent<TagComponent>().Tag;		

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		bool opened = ImGui::TreeNodeEx((void*)(uint)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		if (opened)
			ImGui::TreePop();


	}
}