#include "kspch.h"
#include "WindowsWindow.h"

#include "Core/Application/Input/Input.h"
#include "Core/Application/Events/ApplicationEvent.h"
#include "Core/Application/Events/MouseEvent.h"
#include "Core/Application/Events/KeyEvent.h"

#include "Renderer/OpenGL/OGLContext.h"
#include "Renderer/Renderer.h"


namespace Kaimos {

	float Window::s_ScreenDPIScaleFactor = 1.0f;
	static uint8_t s_WindowCount = 0;
	static uint s_StoredWinSize[4];

	static void GLFWErrorCallback(int error, const char* desc)
	{
		KS_ERROR("GLFW Initialization Error ({0}): {1}", error, desc);
	}



	// ----------------------- Public Class Methods -------------------------------------------------------
	WindowsWindow::WindowsWindow(const std::string& window_name)
	{
		// -- Initialize GLFW --
		KS_PROFILE_FUNCTION();
		KS_TRACE("Creating Windows Window");
		InitializeGLFW();
		
		// -- Get Primary Monitor Size --
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		uint width = (uint)(mode->width / 1.4f);
		uint height = (uint)(mode->height / 1.4f);

		// -- Set Props & Initialize Window --
		Init(WindowProps(window_name, width, height));

		// -- Set Fullscreen if Dist Build --
		#if KS_DIST
			SetFullscreen(true);
			SetVSync(true);
		#endif
	}

	WindowsWindow::WindowsWindow(const WindowProps& window_props)
	{
		// -- Initialize GLFW & Window --
		KS_PROFILE_FUNCTION();
		KS_TRACE("Creating Windows Window");
		InitializeGLFW();
		Init(window_props);
	}

	WindowsWindow::~WindowsWindow()
	{
		KS_PROFILE_FUNCTION();
		KS_TRACE("Shutting Down Windows Window");
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		KS_PROFILE_FUNCTION();
		glfwPollEvents();
		m_Context->SwapBuffers();
	}



	// ----------------------- Public Window Methods ------------------------------------------------------
	void WindowsWindow::Shutdown()
	{
		KS_PROFILE_FUNCTION();
		KS_TRACE("Destroying GLFW Window '{0}'", m_Data.Title);
		glfwDestroyWindow(m_Window);
		--s_WindowCount;

		if (s_WindowCount == 0)
		{
			KS_TRACE("Terminating GLFW");
			glfwTerminate();
		}
	}



	// ----------------------- Setters --------------------------------------------------------------------
	void WindowsWindow::SetFullscreen(bool fullscreen)
	{
		if (fullscreen == IsFullscreen())
			return;

		if (fullscreen)
		{
			// -- Set Values Before Fullscreen --
			int x, y, w, h;
			glfwGetWindowPos(m_Window, &x, &y);
			glfwGetWindowSize(m_Window, &w, &h);
			
			s_StoredWinSize[0] = x;
			s_StoredWinSize[1] = y;
			s_StoredWinSize[2] = w;
			s_StoredWinSize[3] = h;

			// -- Set Fullscreen --
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, 0);
		}
		else
			glfwSetWindowMonitor(m_Window, nullptr, s_StoredWinSize[0], s_StoredWinSize[1], s_StoredWinSize[2], s_StoredWinSize[3], 0);
	}

	bool WindowsWindow::IsFullscreen() const
	{
		return glfwGetWindowMonitor(m_Window) != nullptr;
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}
	


	// ----------------------- Private Window Methods -----------------------------------------------------
	void WindowsWindow::InitializeGLFW()
	{
		// -- GLFW Initialization --
		KS_PROFILE_FUNCTION();
		if (s_WindowCount == 0)
		{
			KS_PROFILE_SCOPE("GLFW Init");
			KS_TRACE("Initializing GLFW");

			int success = glfwInit();
			KS_ENGINE_ASSERT(success, "Couldn't Initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			m_GLFWInitialized = success;
		}
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		KS_PROFILE_FUNCTION();
		KS_ENGINE_ASSERT(m_GLFWInitialized, "Tried to create a Kaimos Window but GLFW is not Initialized!");

		// -- Set Window Variables --
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title = props.Title;
		KS_TRACE("Created Window '{0}' with resolution {1}x{2}px", m_Data.Title, m_Data.Width, m_Data.Height);

		// -- Window Creation --
		{
			KS_PROFILE_SCOPE("GLFW Create Window");

			float x, y;
			glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &x, &y);

			if (x > 1.0f || y > 1.0f)
			{
				s_ScreenDPIScaleFactor = y;
				glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
			}

			#if KS_DEBUG
				if (Renderer::GetRendererAPI() == RendererAPI::API::OPENGL)
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			#endif

			glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE); // This flag makes the fullscreen window to not lose the focus
			m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
			glfwSetWindowPos(m_Window, 50, 50);
			++s_WindowCount;
		}
		
		// -- Graphics Context Creation --
		KS_TRACE("Initializing Kaimos Context");
		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();
		
		// -- GLFW Window User ptr & VSYNC --
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// -- Set GLFW Callbacks --
		SetGLFWEventCallbacks();
	}

	
	void WindowsWindow::SetGLFWEventCallbacks() const
	{
		// Window/Application Events
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int w, int h)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = w;
				data.Height = h;

				WindowResizeEvent event(w, h);
				if(data.EventCallback)
					data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int drop_count, const char* paths[])
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowDragDropEvent event((uint)drop_count, paths);
				data.EventCallback(event);
			});


		// Key Events
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				key = (int)Input::GetCrossKeyboardKey((KEY_CODE)key);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});


		// Mouse Events
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOff, double yOff)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOff, (float)yOff);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}
}
