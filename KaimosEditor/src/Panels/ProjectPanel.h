#ifndef _PROJECTPANEL_H_
#define _PROJECTPANEL_H_

#include "Kaimos.h"

namespace Kaimos {

	class ProjectPanel
	{
	public:

		// --- Public Class Methods ---
		ProjectPanel() = default;
		void OnUIRender(bool& show_project_files, bool& show_console);

	private:

		// --- Private Scene Methods ---
		void DisplayProjectFiles()		{}
		void DisplayConsoleBar();
		void DisplayConsole();

	private:

		struct KaimosConsole
		{
			void ClearLog()					{ ConsoleMessages.clear(); }
			void AddLog(const char* msg)	{ ConsoleMessages.push_back(msg); }

			std::vector<const char*> ConsoleMessages;
			bool AutoScroll = true;
			bool ScrollToBottom = false;
			bool CopyToClipboard = false;
		};

		KaimosConsole m_Console = {};
	};
}

#endif //_PROJECTPANEL_H_
