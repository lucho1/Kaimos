#include "kspch.h"
#include "ImGuiUtils.h"

#include <imgui.h>
#include <imgui_internal.h>


namespace Kaimos::UI {

	// ----------------------- Conversors -----------------------------------------------------------------
	ImVec2 ConvertToImVec2(glm::vec2 v)	{ return { v.x, v.y }; }
	ImVec4 ConvertToImVec4(glm::vec4 v)	{ return { v.x, v.y, v.z, v.w }; }
	glm::vec2 ConvertToVec2(ImVec2 v)	{ return { v.x, v.y }; }
	glm::vec4 ConvertToVec4(ImVec4 v)	{ return { v.x, v.y, v.z, v.w }; }

	

	// ----------------------- Public UI Methods ----------------------------------------------------------
	void UIFunctionalities::DrawHelpMarker(const std::string& help_text)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(help_text.c_str());
			ImGui::PopTextWrapPos();

			ImGui::EndTooltip();
		}
	}


	void UIFunctionalities::DrawVec3UI(const std::string& name, glm::vec3& value, const glm::vec3& xcolor, const glm::vec3& ycolor, const glm::vec3& zcolor, float reset_value, const std::string& labels, float column_width)
	{
		ImFont* bold_font = ImGui::GetIO().Fonts->Fonts[1];
		glm::vec3 hover_sum = glm::vec3(0.1f);

		// -- New ID --
		// To notify ImGui kind of new "namespace" or new ID, so we don't have problems of values modifying other values
		ImGui::PushID(name.c_str());

		// -- Name Column --
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, column_width);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		// -- Vector Columns --
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 4 });

		// -- Line Height --
		// Internal Imgui code, on how ImGui calculates line heights
		float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 button_size = { line_height + 3.0f, line_height };

		// -- X Button and DragFloat --
		SetButton(xcolor, xcolor + hover_sum, bold_font);
		if (ImGui::Button(std::string{labels[0]}.c_str(), button_size))
			value.x = reset_value;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();


		// -- Y Button and DragFloat --
		ImGui::SameLine();
		SetButton(ycolor, ycolor + hover_sum, bold_font);
		if (ImGui::Button(std::string{labels[1]}.c_str(), button_size))
			value.y = reset_value;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		// -- Z Button and DragFloat --
		ImGui::SameLine();
		SetButton(zcolor, zcolor + hover_sum, bold_font);
		if (ImGui::Button(&labels[2], button_size))
			value.z = reset_value;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		// -- Pop Initial Pushes --
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}


	
	// ----------------------- Private UI Methods ---------------------------------------------------------
	void UIFunctionalities::SetButton(const glm::vec3& active_color, const glm::vec3& hover_color, ImFont* font)
	{
		ImVec4 im_active_color = ConvertToImVec4(glm::vec4(active_color, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_Button, im_active_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, im_active_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ConvertToImVec4(glm::vec4(hover_color, 1.0f)));
		ImGui::PushFont(font);
	}
}
