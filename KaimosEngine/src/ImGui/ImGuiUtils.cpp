#include "kspch.h"
#include "ImGuiUtils.h"
#include "Core/Utils/Maths/Maths.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>


namespace Kaimos::KaimosUI {

	// ----------------------- Conversors -----------------------------------------------------------------
	ImVec2 ConvertToImVec2(glm::vec2 v)	{ return { v.x, v.y }; }
	ImVec4 ConvertToImVec4(glm::vec4 v)	{ return { v.x, v.y, v.z, v.w }; }
	glm::vec2 ConvertToVec2(ImVec2 v)	{ return { v.x, v.y }; }
	glm::vec4 ConvertToVec4(ImVec4 v)	{ return { v.x, v.y, v.z, v.w }; }

	

	// ----------------------- Public UI Methods ----------------------------------------------------------
	// --- Drag Floats/Sliders ---
	bool UIFunctionalities::DrawInlineDragFloat(const char* text, const char* label, float* value, float speed, float width, float spacing, float min, float max, const char* fmt, float pow)
	{
		SetTextCursorAndWidth(text, width, spacing);
		return ImGui::DragFloat(label, value, speed, min, max, fmt, pow);
	}
		
	bool UIFunctionalities::DrawInlineDragFloat2(const char* text, const char* label, glm::vec2& value, float speed, float width, float spacing, float min, float max, const char* fmt, float pow)
	{
		SetTextCursorAndWidth(text, width, spacing);
		return ImGui::DragFloat2(label, glm::value_ptr(value), speed, min, max, fmt, pow);
	}

	bool UIFunctionalities::DrawInlineSlider(const char* text, const char* label, float* value, float width, float spacing, float max, float min, const char* fmt, float pow)
	{
		SetTextCursorAndWidth(text, width, spacing);
		return ImGui::SliderFloat(label, value, min, max, fmt, pow);
	}


	// --- Buttons ---
	bool UIFunctionalities::DrawTexturedButton(uint texture_id, const glm::vec2& size, const glm::vec3& bg_color)
	{
		PushButtonSettings(bg_color, bg_color + glm::vec3(0.1f), nullptr);
		if (texture_id == 0)
			ImGui::Button("###textured_btn", ConvertToImVec2(size));
		else
			return ImGui::ImageButton((ImTextureID)texture_id, ConvertToImVec2(size), { 0.0f, 1.0f }, { 1.0f, 0.0f }, 0, ConvertToImVec4(glm::vec4(bg_color, 1.0f)));
	}

	bool UIFunctionalities::DrawColoredButton(const char* label, const glm::vec2& size, const glm::vec3& color, bool black_font)
	{
		ImFont* font = nullptr;
		if(black_font)
			font = ImGui::GetIO().Fonts->Fonts[1];

		return Button(label, ConvertToImVec2(size), color, font);
	}

	void UIFunctionalities::PopButton(bool black_font)
	{
		ImGui::PopStyleColor(3);
		if (black_font)
			ImGui::PopFont();
	}


	// --- Text ---
	void UIFunctionalities::DrawInputText(const char* label, std::string& value, float widget_width)
	{		
		// - Buffer -
		char buffer[256];										// Make a buffer to store the text input of the widget
		memset(buffer, 0, sizeof(buffer));						// Set it all to 0
		std::strncpy(buffer, value.c_str(), sizeof(buffer));	// Copy value to buffer
		//strcpy_s(buffer, sizeof(buffer), tag.c_str());		// This is the same, but std::strncpy() is more like a C++ standard

		// - Item Width and Input Text -
		SetItemWidth(widget_width);
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank; // | ImGuiInputTextFlags_EnterReturnsTrue;
		if (ImGui::InputText(label, buffer, sizeof(buffer), flags))
			value = std::string(buffer);
	}


	// --- Others ---
	void UIFunctionalities::DrawHelpMarker(const std::string& help_text)
	{
		// - Help Text + Hover -
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			// - Tooltip + Text Wrap -
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);

			ImGui::TextUnformatted(help_text.c_str());

			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}


	void UIFunctionalities::DrawDropDown(const char* label, const char* options[], uint options_size, const char* selected_option, uint& selected_index, float width, float spacing)
	{
		// - Set Combo Settings -
		SetTextCursorAndWidth(label, width, spacing);
		std::string widget_label = "###" + std::string(label);

		if (ImGui::BeginCombo(widget_label.c_str(), selected_option))
		{
			// - Loop options & set selected option -
			for (uint i = 0; i < options_size; ++i)
			{
				bool selected = selected_option == options[i];
				if (ImGui::Selectable(options[i], selected))
				{
					selected_option = options[i];
					selected_index = i;
				}

				// - Set Focus to selected Item -
				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
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
		if(Button(std::string{labels[0]}.c_str(), button_size, xcolor, bold_font))
			value.x = reset_value;

		PopButton(true);
		InlineDragFloat("##X", &value.x, 0.1f);
		ImGui::PopItemWidth();


		// -- Y Button and DragFloat --
		ImGui::SameLine();
		if (Button(std::string{labels[1]}.c_str(), button_size, ycolor, bold_font))
			value.y = reset_value;

		PopButton(true);
		InlineDragFloat("##Y", &value.y, 0.1f);
		ImGui::PopItemWidth();

		// -- Z Button and DragFloat --
		ImGui::SameLine();
		if (Button(&labels[2], button_size, zcolor, bold_font))
			value.z = reset_value;

		PopButton(true);
		InlineDragFloat("##Z", &value.z, 0.1f);
		ImGui::PopItemWidth();

		// -- Pop Initial Pushes --
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}


	
	// ----------------------- Private UI Methods ---------------------------------------------------------
	bool UIFunctionalities::Button(const char* label, const ImVec2& size, const glm::vec3& color, ImFont* font)
	{
		PushButtonSettings(color, color + glm::vec3(0.1f), font);
		return ImGui::Button(label, size);
	}

	void UIFunctionalities::PushButtonSettings(const glm::vec3& active_color, const glm::vec3& hover_color, ImFont* font)
	{
		// - Conversions to ImGui Data Type -
		ImVec4 im_active_color = ConvertToImVec4(glm::vec4(active_color, 1.0f));
		ImVec4 im_hover_color = ConvertToImVec4(glm::vec4(hover_color, 1.0f));

		// - Color Push -
		ImGui::PushStyleColor(ImGuiCol_Button, im_active_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, im_active_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, im_hover_color);

		// - Font Push -
		if(font != nullptr)
			ImGui::PushFont(font);
	}

	void UIFunctionalities::SetItemWidth(float width)
	{
		// - If width != 0, Set Width -
		if (!Maths::CompareFloats(width, 0.0f))
			ImGui::SetNextItemWidth(width);
	}

	void UIFunctionalities::SetTextCursorAndWidth(const char* text, float width, float spacing)
	{
		// - Set Cursor & text -
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
		ImGui::Text(text);

		// - Same Line width + Cursor Offset -
		ImGui::SameLine(ImGui::GetContentRegionAvail().x / spacing - 1.0f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 6.0f);

		// - Set Width -
		SetItemWidth(width);
	}

	void UIFunctionalities::InlineDragFloat(const char* label, float* value, float speed)
	{
		ImGui::SameLine();
		ImGui::DragFloat(label, value, speed, 0.0f, 0.0f, "%.2f");
	}
}
