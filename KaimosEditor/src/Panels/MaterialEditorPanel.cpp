#include "kspch.h"
#include "MaterialEditorPanel.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>
#include <glm/gtc/type_ptr.hpp>

namespace Kaimos {

	void MaterialEditorPanel::OnUIRender()
	{
		ImGui::Begin("Kaimos Material Editor");
		if (!m_MaterialToModify)
		{
			ImGui::End();
			return;
		}

		// -- Begin Editor --
		ImNodes::BeginNodeEditor();

		bool set_draggable = true;
		float indent = 5.0f, width = 30.0f;

		// -- Central Node Beginning --
		ImNodes::BeginNode(1);
		ImNodes::BeginNodeTitleBar();
		ImGui::Text("Central");
		ImNodes::EndNodeTitleBar();

		// -- Color Node --
		ImGui::Text("Color");
		ImGuiColorEditFlags color_flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs;
		ImGui::SameLine();
		ImGui::ColorEdit4("##spcompcolor", glm::value_ptr(m_MaterialToModify->Color), color_flags);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
			set_draggable = false;

		// -- Texture Node --
		if (m_MaterialToModify->SpriteTexture)
		{
			std::string tex_path = m_MaterialToModify->TextureFilepath;
			std::string tex_name = tex_path.substr(tex_path.find_last_of("/\\" + 1, tex_path.size() - 1));

			ImGui::Text("Texture");
			ImGui::Indent(indent);
			ImGui::Text("Texture %i: '%s'", m_MaterialToModify->SpriteTexture->GetTextureID(), tex_name.c_str());
			ImGui::SameLine();
			ImGui::Text("(%ix%i)", m_MaterialToModify->SpriteTexture->GetWidth(), m_MaterialToModify->SpriteTexture->GetHeight());
		}


		// -- Texture Tiling Node --
		ImGui::Text("Texture Tiling");
		ImGui::SameLine(); ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##spcomptextiling", &m_MaterialToModify->TextureTiling, 0.2f);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
			set_draggable = false;


		// -- Texture UV Offset Node --
		ImGui::Text("Texture Offset");
		ImGui::SameLine(); ImGui::SetNextItemWidth(width * 2.0f);
		ImGui::DragFloat2("##spcomptexoffx", glm::value_ptr(m_MaterialToModify->TextureUVOffset), 0.2f);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused() || ImGui::IsItemActive() || ImGui::IsItemEdited() || ImGui::IsItemClicked())
			set_draggable = false;

		// -- Central Node Dragging & Ending --
		ImNodes::SetNodeDraggable(1, set_draggable);
		ImNodes::EndNode();


		// -------- EXAMPLE OF LINKING --------
		//ImNodes::BeginNode();
		//ImNodes::BeginInputAttribute(3);
		//ImGui::Text("Input Pin (%.1f, %.1f)", 5.0f, 4.0f);
		//ImNodes::EndInputAttribute();
		//
		//
		//ImNodes::BeginOutputAttribute(4);
		//ImGui::Indent(40.0f);
		//ImGui::Text("Output Pin");
		//ImNodes::EndOutputAttribute();
		//
		//ImNodes::EndNode();
		//
		//ImNodes::BeginNode(2);
		//
		//ImNodes::BeginInputAttribute(1);
		//ImGui::Text("Input Pin");
		//ImNodes::EndInputAttribute();
		//
		//ImNodes::BeginOutputAttribute(2);
		//ImGui::Text("Output Pin");
		//ImNodes::EndOutputAttribute();
		//
		//ImNodes::EndNode();
		//
		//static std::vector<std::pair<int, int>> links;
		//for (int i = 0; i < links.size(); ++i)
		//	ImNodes::Link(i, links[i].first, links[i].second);

		ImNodes::EndNodeEditor();

		//int start = 4, end = 1;
		//if (ImNodes::IsLinkCreated(&start, &end))
		//{
		//	links.push_back({ 4, 1 });
		//}


		// -- End Editor --
		ImGui::End();
	}

	void MaterialEditorPanel::LoadIniEditorSettings() const
	{
		UnsetMaterialToModify();
		ImNodes::LoadCurrentEditorStateFromIniFile("imnode.ini");
	}

	void MaterialEditorPanel::SaveIniEditorSettings() const
	{
		ImNodes::SaveCurrentEditorStateToIniFile("imnode.ini");
	}

	void MaterialEditorPanel::UnsetMaterialToModify() const
	{
		if (m_MaterialToModify)
			m_MaterialToModify->InMaterialEditor = false;

		m_MaterialToModify = nullptr;
	}

	void MaterialEditorPanel::SetMaterialToModify(SpriteRendererComponent* sprite_component) const
	{
		if(m_MaterialToModify)
			m_MaterialToModify->InMaterialEditor = false;

		m_MaterialToModify = sprite_component;
		m_MaterialToModify->InMaterialEditor = true;
	}
}
