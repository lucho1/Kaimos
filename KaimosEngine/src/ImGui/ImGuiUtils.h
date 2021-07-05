#ifndef _IMGUIUTILS_H_
#define _IMGUIUTILS_H_

#include <string>

struct ImVec2;
struct ImVec4;
struct ImFont;

namespace Kaimos::KaimosUI {

	// --- Conversors ---
	ImVec2 ConvertToImVec2(glm::vec2 v);
	ImVec4 ConvertToImVec4(glm::vec4 v);
	glm::vec2 ConvertToVec2(ImVec2 v);
	glm::vec4 ConvertToVec4(ImVec4 v);

	class UIFunctionalities
	{
	public:
		
		// --- Public UI Methods ---
		// Draw a Help Marker with a Text as popup
		static void DrawHelpMarker(const std::string& help_text);

		// - Drag Floats/Sliders -
		// Draw a Drag Float in the same line than 'text' - label is the "widget id"
		static bool DrawInlineDragFloat(const char* text, const char* label, float* value, float speed = 1.0f, float width = 0.0f, float spacing = 2.0f, float min = 0.0f, float max = 0.0f, const char* fmt = "%.2f", float pow = 1.0f);
		
		// Draw a Drag Float of 2 values in the same line than 'text' - label is the "widget id"
		static bool DrawInlineDragFloat2(const char* text, const char* label, glm::vec2& value, float speed = 1.0f, float width = 0.0f, float spacing = 2.0f, float min = 0.0f, float max = 0.0f, const char* fmt = "%.2f", float pow = 1.0f);

		// Draw a Slider in the same line than 'text' - label is the "widget_id"
		static bool DrawInlineSlider(const char* text, const char* label, float* value, float width = 0.0f, float spacing = 2.0f, float max = 10.0f, float min = 0.0f, const char* fmt = "%.2f", float pow = 1.0f);

		// - Buttons -
		// Draw a button with a texture - requires a label (for id) texture id, button size and background color (when it has no texture or it has transparencies)
		// This function requires to use PopButton() after calling it
		static bool DrawTexturedButton(const char* label, uint texture_id, const glm::vec2& size, const glm::vec3& bg_color);

		// Draw a button with a color - requires the label to display, button size, color and a bool to draw the label with a black font
		// This function requires to use PopButton() after calling it
		static bool DrawColoredButton(const char* label, const glm::vec2& size, const glm::vec3& color, bool black_font);

		// Pops Button ImGui values - black_font is for the cases in which black font is used (it needs to be popped too)
		static void PopButton(bool black_font);

		// - Text -
		// Draw Input Text Widget (max string characters: 256)
		static void DrawInputText(const char* label, std::string& value, float widget_width = 0.0f);

		// - Others -
		// Draw Dropdown: options = array of options (needs a size specification), selected_option = name of current selected opt. (needs its index in options[])
		// the width is the width that the dropdown will occupy, if left to 0 will be the half of its imgui-calculated width
		static bool DrawDropDown(const char* label, const std::vector<std::string>& options, uint options_size, std::string& selected_option, uint& selected_index, float width = 0.0f, float spacing = 2.0f);

		// Draw controller of vec3 (ue4-like): name = label ("position"), value = ref to vec3, xyz colors = vec3 axis colors,
		// reset value = reset on pressing axis button, labels = names for axis (xyz, rgb...), column width = width of controller elements
		static void DrawVec3UI(const std::string& name, glm::vec3& value, const glm::vec3& xcolor, const glm::vec3& ycolor, const glm::vec3& zcolor, float reset_value = 0.0f, const std::string& labels = {"XYZ"}, float column_width = 100.0f);
		
	private:

		// --- Private UI Methods ---
		static bool Button(const char* label, const ImVec2& size, const glm::vec3& color, ImFont* font);
		static void PushButtonSettings(const glm::vec3& active_color, const glm::vec3& hover_color, ImFont* font = nullptr);
		
		static void SetItemWidth(float width);
		static void SetTextCursorAndWidth(const char* text, float width, float spacing);
		
		static void InlineDragFloat(const char* label, float* value, float speed);
	};
}

#endif //_IMGUIUTILS_H_
