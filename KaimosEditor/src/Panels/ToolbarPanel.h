#ifndef _TOOLBARPANEL_H_
#define _TOOLBARPANEL_H_

#include "Kaimos.h"

namespace Kaimos {

	class ToolbarPanel
	{
		friend class EditorLayer;
	public:

		// --- Public Class Methods ---
		ToolbarPanel() = default;
		void OnUIRender();

	private:

		// --- Private Scene Methods ---
		bool DrawToolbarButton(const char* label, uint texture_id, const glm::vec4& color);

	private:

		int m_SelectedOperation = -1;	//Operations: Nothing = -1, Transform = 0, Rotation = 1, Scale = 2
		bool m_WorldMode = false;		// Local/World mode
		bool m_Snap = false;			// Grid-Snap

		bool m_ChangeSnap = false;		// Bool for the Guizmo to know if change the snap mode if CTRL key was released
	};
}

#endif //_TOOLBARPANEL_H_
