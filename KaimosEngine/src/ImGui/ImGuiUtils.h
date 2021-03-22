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

		// To draw an Unreal-like controller of vec3: name is the controller label ("position"), value is the value to modify, xyz
		// colors are the colors that the controller will use for each of the 3 axis, chars are the names for each of the values,
		// reset value is the value to reset the number when pressing on an axis button, and column width is the width of each of the elements of the controller
		static void DrawVec3UI(const std::string& name, glm::vec3& value, const glm::vec3& xcolor, const glm::vec3& ycolor, const glm::vec3& zcolor, float reset_value = 0.0f, const std::string& labels = {"XYZ"}, float column_width = 100.0f);
		
	private:

		// --- Private UI Methods ---
		static void SetButton(const glm::vec3& active_color, const glm::vec3& hover_color, ImFont* font);
	};
}


#endif //_IMGUIUTILS_H_
