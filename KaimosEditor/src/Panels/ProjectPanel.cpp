#include "kspch.h"
#include "ProjectPanel.h"

#include <ImGui/imgui.h>

namespace Kaimos {

	static ImGuiTextFilter filter;

	// ----------------------- Public Class Methods -------------------------------------------------------
	void Kaimos::ProjectPanel::OnUIRender(bool& show_project_files, bool& show_console)
	{
		if (show_project_files)
		{
			ImGui::Begin("Project", &show_project_files);
			DisplayProjectFiles();
			ImGui::End();
		}

		if (show_console)
		{
			ImGui::Begin("Console", &show_console);
			DisplayConsoleBar();

			ImGui::Separator();
			DisplayConsole();
			ImGui::End();
		}
	}


	
	// ----------------------- Private Class Methods ------------------------------------------------------
	void ProjectPanel::DisplayConsoleBar()
	{
		ImVec2 buttons_size = { 40.0f, 23.5f };

		// - Clear Button -
		if (ImGui::Button("Clear", buttons_size))
			Log::ClearLogs();

		// - Copy Button -
		ImGui::SameLine();
		buttons_size.x += 62.0f;
		m_Console.CopyToClipboard = ImGui::Button("Copy to Clipboard", buttons_size);

		// - Filter -
		ImGui::SameLine(200.0f);
		filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180.0f);

		// - Auto Scroll Box -
		ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - 110.0f);
		ImGui::Text("Auto Scroll"); ImGui::SameLine();
		ImGui::Checkbox("###console_autoscroll", &m_Console.AutoScroll);

		// - Scroll Button -
		ImGui::SameLine();
		if (ImGui::Button(" v ", { 23.0f, 23.0f }))
			m_Console.ScrollToBottom = true;
	}


	void Kaimos::ProjectPanel::DisplayConsole()
	{
		// - Console Region -
		ImGui::BeginChild("ConsoleRegion", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
		
		// - Spacing & Copy to Clipboard -
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 1.5f)); // Tighten spacing
		if (m_Console.CopyToClipboard)
			ImGui::LogToClipboard();

		std::vector<Kaimos::Log::KaimosLog> vec = Log::GetLogs();

		// - Message Filter & Color -
		for (uint i = 0; i < vec.size(); ++i)
		{
			std::string message = vec[i].Log;
			if (!filter.PassFilter(message.c_str()))
				continue;

			switch (vec[i].LogType)
			{
				case Log::LOG_TYPES::INFO_LOG:	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.9f, 0.4f, 1.0f)); break;
				case Log::LOG_TYPES::WARN_LOG:	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.4f, 1.0f)); break;
				case Log::LOG_TYPES::ERROR_LOG:	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f)); break;
				default: ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f)); break;
			}
			
			// - Message Print -
			ImGui::TextUnformatted(message.c_str());
			ImGui::PopStyleColor();
		}

		// - Copy to Clipboard -
		if (m_Console.CopyToClipboard)
		{
			ImGui::LogFinish();
			m_Console.CopyToClipboard = false;
		}

		// - Scroll -
		if (m_Console.ScrollToBottom || (m_Console.AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
		{
			ImGui::SetScrollHereY(1.0f);
			m_Console.ScrollToBottom = false;
		}
		
		// - Pop -
		ImGui::PopStyleVar();
		ImGui::EndChild();
	}
}
