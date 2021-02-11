#include "kspch.h"
#include "Core/Utils/PlatformUtils.h"
#include "Core/Application.h"

// --- To get usage of windows file dialogs ---
#include <commdlg.h>

// --- To attach file dialogs to the engine's window ---
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32 // If defined, we can get Win32 functionalities we need
#include <GLFW/glfw3native.h>


namespace Kaimos {

	std::string FileDialogs::OpenFile(const char* filter)
	{
		// --- Initialize OPENFILENAME to 0 (Common Dialog Box Structure) ---
		OPENFILENAMEA open_file_name;
		ZeroMemory(&open_file_name, sizeof(OPENFILENAME));	

		// --- Set OPENFILENAME ---
		// Set Size
		open_file_name.lStructSize = sizeof(OPENFILENAME);

		// Set the owner of the dialog window to the engine's window
		open_file_name.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		
		// Buffer the file
		CHAR file[260] = { 0 };
		open_file_name.lpstrFile = file;
		open_file_name.nMaxFile = sizeof(file);
		open_file_name.lpstrFilter = filter;
		open_file_name.nFilterIndex = 1;
		open_file_name.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; // If this is not here, it will change the working directory

		// If file (Ascii) is open (exists), return it
		if (GetOpenFileNameA(&open_file_name) == TRUE)
			return open_file_name.lpstrFile;
		
		return std::string();
	}


	std::string FileDialogs::SaveFile(const char* filter)
	{
		// --- Initialize OPENFILENAME to 0 (Common Dialog Box Structure) ---
		OPENFILENAMEA open_file_name;
		ZeroMemory(&open_file_name, sizeof(OPENFILENAME));

		// --- Set OPENFILENAME ---
		// Set Size
		open_file_name.lStructSize = sizeof(OPENFILENAME);

		// Set the owner of the dialog window to the engine's window
		open_file_name.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());

		// Buffer the file
		CHAR file[260] = { 0 };
		open_file_name.lpstrFile = file;
		open_file_name.nMaxFile = sizeof(file);
		open_file_name.lpstrFilter = filter;
		open_file_name.nFilterIndex = 1;
		open_file_name.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; // If this is not here, it will change the working directory

		// If file (Ascii) is open (exists), return it
		if (GetSaveFileNameA(&open_file_name) == TRUE)
			return open_file_name.lpstrFile;

		return std::string();
	}
}