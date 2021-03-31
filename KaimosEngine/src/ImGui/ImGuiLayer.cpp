#include "kspch.h"
#include "ImGuiLayer.h"
#include "Core/Application/Application.h"

#define IMGUI_IMPL_API
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include <GLFW/glfw3.h>


namespace Kaimos {

	// ----------------------- Public Layer Methods -------------------------------------------------------
	void ImGuiLayer::OnAttach()
	{
		KS_PROFILE_FUNCTION();

		// -- Setup ImGui context --
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;			// Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;			// Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;				// Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;				// Enable Multi-Viewport/Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// -- Setup ImGui Style (own style based on other 2) --
		SetEngineUIStyle();
		//ImGui::StyleColorsDark();

		// -- When viewports enabled, tweak WindowRounding/WindowBg so plaform windows can look identical to regular ones --
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		
		// -- Setup Platform/Renderer Bindings --
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}


	void ImGuiLayer::OnDetach()
	{
		KS_PROFILE_FUNCTION();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}


	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& imgui_io = ImGui::GetIO();

			// a |= b is the same than a = a | b
			// a = a | b --> a is true if at least a or b is true, else is false
			bool handled = e.IsHandled() | e.IsInCategory(EVENT_CATEGORY::MOUSE) & imgui_io.WantCaptureMouse;
			handled |= e.IsInCategory(EVENT_CATEGORY::KEYBOARD) & imgui_io.WantCaptureKeyboard;

			e.SetHandled(handled);
		}
	}


	
	// ----------------------- Public ImGui Methods -------------------------------------------------------
	void ImGuiLayer::Begin() const
	{
		KS_PROFILE_FUNCTION();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}


	void ImGuiLayer::End() const
	{
		KS_PROFILE_FUNCTION();
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}		
	}



	// ----------------------- Private ImGui Methods ------------------------------------------------------
	void ImGuiLayer::SetEngineUIStyle() const
	{
		// ImGui Style gotten from a free-to-use style from https://www.unknowncheats.me/forum/c-and-c-/189635-imgui-style-settings.html
		// & From CronosEngine (style by Roger Leon @rleonborras) with some touches by myself
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		
		// -- Fonts Load --
		// Default Font + Secondary one
		io.FontDefault = io.Fonts->AddFontFromFileTTF("../KaimosEngine/res/fonts/Ruda/Ruda-SemiBold.ttf", Window::s_ScreenDPIScaleFactor * 13.0f); // You can also try "Opensans/OpenSans-SemiBold.ttf" (in 14.0f)
		io.Fonts->AddFontFromFileTTF("../KaimosEngine/res/fonts/Ruda/Ruda-Black.ttf", Window::s_ScreenDPIScaleFactor * 13.0f);
		
		// Load different fonts in different positions so ImGui can handle fonts variations
		std::string font_path = "../KaimosEngine/res/fonts/Ruda/Ruda-VariableFont_wght.ttf";
		io.Fonts->AddFontFromFileTTF(font_path.c_str(), Window::s_ScreenDPIScaleFactor * 10.0f);
		io.Fonts->AddFontFromFileTTF(font_path.c_str(), Window::s_ScreenDPIScaleFactor * 12.0f);
		io.Fonts->AddFontFromFileTTF(font_path.c_str(), Window::s_ScreenDPIScaleFactor * 16.0f);
		io.Fonts->AddFontFromFileTTF(font_path.c_str(), Window::s_ScreenDPIScaleFactor * 18.0f);
		io.Fonts->AddFontFromFileTTF(font_path.c_str(), Window::s_ScreenDPIScaleFactor * 20.0f);
		io.Fonts->AddFontFromFileTTF(font_path.c_str(), Window::s_ScreenDPIScaleFactor * 22.0f);
		io.Fonts->AddFontFromFileTTF(font_path.c_str(), Window::s_ScreenDPIScaleFactor * 24.0f);


		// -- UI Style Settings --
		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		style->TabRounding = style->FrameRounding;
		style->ChildRounding = 4.0f;
		//style->FrameBorderSize = 0.01f;
		style->PopupRounding = 2.0f;
		style->TabBorderSize = 1.0f;

		style->ScaleAllSizes(Window::s_ScreenDPIScaleFactor);

		// -- UI Style Colors --
		//colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.0f);
		colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		//colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.0f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.34f, 0.33f, 0.4f, 1.0f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.0f);
		//colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.0f);
		colors[ImGuiCol_Border] = ImVec4(0.8f, 0.8f, 0.83f, 0.88f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.0f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.1f, 0.09f, 0.12f, 1.0f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1f, 0.09f, 0.12f, 1.0f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.8f, 0.8f, 0.83f, 0.31f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.0f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.8f, 0.8f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.8f, 0.8f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.0f);
		//colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.0f);
		//colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.0f);
		//colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.0f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.0f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.0f);
		


		colors[ImGuiCol_PlotLines] = ImVec4(0.56f, 0.57f, 0.38f, 0.61f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.7f, 0.0f, 1.0f);
		colors[ImGuiCol_PlotHistogram] = colors[ImGuiCol_PlotLines];
		colors[ImGuiCol_PlotHistogramHovered] = colors[ImGuiCol_PlotLinesHovered];
		//colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		//colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.0f, 0.0f, 1.0f);
		//colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		//colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.0f, 0.0f, 1.0f);

		//colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.0f, 0.0f, 0.43f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.1f, 0.4f, 0.75f, 0.63f);
		colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.0f, 0.98f, 0.95f, 0.73f);
		
		// -- More Important Stuff (all from web upside) --
		// -- Headers --
		colors[ImGuiCol_Header] = ImVec4(0.14f, 0.13f, 0.17f, 1.0f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.0f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.0f);

		//colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.0f);
		//colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.0f);
		//colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.0f);
		
		// -- Buttons --
		//colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(0.14f, 0.13f, 0.17f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.0f);

		// -- Frames --
		colors[ImGuiCol_FrameBg] = ImVec4(0.1f, 0.09f, 0.12f, 1.0f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.0f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.0f);

		// -- Titles --
		colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.09f, 0.12f, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.09f, 0.12f, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.0f);
		

		// -- From here on, the style comes from Cronos Engine (https://github.com/lucho1/CronosEngine), owned by me (@lucho1) and @rleonborras --
		colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		//colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_Separator] = ImVec4(0.65f, 0.65f, 0.7f, 0.88f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1f, 0.4f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.1f, 0.4f, 0.75f, 1.0f);
		//colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.90f);
		colors[ImGuiCol_DragDropTarget] = colors[ImGuiCol_PlotLinesHovered];
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);

		//colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive]; colors[ImGuiCol_DockingPreview].w *= 0.7f;
		colors[ImGuiCol_DockingPreview] = ImVec4(0.06f, 0.05f, 0.07f, 0.7f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);


		// -- Tabs --
		//colors[ImGuiCol_Tab] = ImVec4(0.392f, 0.369f, 0.376f, 0.5f);
		colors[ImGuiCol_Tab] = ImVec4(0.44f, 0.42f, 0.42f, 0.5f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.392f, 0.369f, 0.376f, 0.5f);
		colors[ImGuiCol_TabActive] = ImVec4(0.392f, 0.369f, 0.376f, 1.0f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.392f, 0.369f, 0.376f, 0.5f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.392f, 0.369f, 0.376f, 0.50f);
	}
}
