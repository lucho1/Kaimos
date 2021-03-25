#ifndef _IMGUIUTILS_H_
#define _IMGUIUTILS_H_

#include <string>

struct ImVec2;
struct ImVec4;
struct ImFont;

namespace Kaimos::UI {

	// --- Conversors ---
	static ImVec2 ConvertToImVec2(glm::vec2 v);
	static ImVec4 ConvertToImVec4(glm::vec4 v);
	static glm::vec2 ConvertToVec2(ImVec2 v);
	static glm::vec4 ConvertToVec4(ImVec4 v);

	class UIFunctionalities
	{
	public:
		
		// --- Public UI Methods ---
		static void DrawHelpMarker(const std::string& help_text);

		// Draw controller of vec3 (ue4-like): name = label ("position"), value = ref to vec3, xyz colors = vec3 axis colors,
		// reset value = reset on pressing axis button, labels = names for axis (xyz, rgb...), column width = width of controller elements
		static void DrawVec3UI(const std::string& name, glm::vec3& value, const glm::vec3& xcolor, const glm::vec3& ycolor, const glm::vec3& zcolor, float reset_value = 0.0f, const std::string& labels = {"XYZ"}, float column_width = 100.0f);
		
	private:

		// --- Private UI Methods ---
		static void SetButton(const glm::vec3& active_color, const glm::vec3& hover_color, ImFont* font);
	};
}

#endif //_IMGUIUTILS_H_
