#include "kspch.h"
#include "MaterialEditorPanel.h"

#include <ImGui/imgui.h>
#include <ImNodes/imnodes.h>

namespace Kaimos {

	void MaterialEditorPanel::OnUIRender()
	{
		ImGui::Begin("Kaimos Material Editor");


		//ImNodesContext* sadas = ImNodes::GetCurrentContext();
		//ImNodes::GetEditorContext();
		//ImNodes::LoadEditorStateFromIniFile(ImNodes::GetEditorContext(), "imnode.ini");

		ImNodes::BeginNodeEditor();

		// ---
		ImNodes::BeginNode(1);
		ImNodes::BeginNodeTitleBar();
		ImGui::Text("VIVA EL VINO");
		ImNodes::EndNodeTitleBar();

		static float num = 0.0f;
		ImGui::PushItemWidth(40.0f);
		ImGui::DragFloat("##dasd", &num);

		ImNodes::BeginInputAttribute(3);
		ImGui::Text("Input Pin (%.1f, %.1f)", 5.0f, 4.0f);
		ImNodes::EndInputAttribute();

		
		ImNodes::BeginOutputAttribute(4);
		ImGui::Indent(40.0f);
		ImGui::Text("Output Pin");
		ImNodes::EndOutputAttribute();
		
		ImNodes::EndNode();



		ImNodes::BeginNode(2);
		
		ImNodes::BeginInputAttribute(1);
		ImGui::Text("Input Pin");
		ImNodes::EndInputAttribute();

		ImNodes::BeginOutputAttribute(2);
		ImGui::Text("Output Pin");
		ImNodes::EndOutputAttribute();
		
		ImNodes::EndNode();

		static std::vector<std::pair<int, int>> links;
		for (int i = 0; i < links.size(); ++i)
			ImNodes::Link(i, links[i].first, links[i].second);

		// ---
		//ImNodes::SaveCurrentEditorStateToIniFile("imnode.ini");
		ImNodes::EndNodeEditor();

		int start = 4, end = 1;
		if (ImNodes::IsLinkCreated(&start, &end))
		{
			links.push_back({ 4, 1 });
		}


		ImGui::End();
	}
}
