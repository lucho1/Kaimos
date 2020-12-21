#include "kspch.h"
#include "WindowsWindow.h"

#include "Core/Input/Input.h"

#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Renderer/Renderer.h"


namespace Kaimos {

	static uint8_t s_WindowCount = 0;

	static void GLFWErrorCallback(int error, const char* desc)
	{
		KS_ENGINE_ERROR("GLFW Initialization Error ({0}): {1}", error, desc);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		KS_PROFILE_FUNCTION();
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		KS_PROFILE_FUNCTION();
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		KS_PROFILE_FUNCTION();
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title = props.Title;
		KS_ENGINE_INFO("Creating Window {0} with measure {1}x{2}px", props.Title, props.Width, props.Height);

		// -- GLFW Initialization --
		if (s_WindowCount == 0)
		{
			KS_PROFILE_SCOPE("GLFW Init");
			KS_ENGINE_INFO("Initializing GLFW");
			int success = glfwInit();
			KS_ENGINE_ASSERT(success, "Couldn't Initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		// -- Window Creation --
		{
			KS_PROFILE_SCOPE("GLFW Create Window");

			#ifdef KS_DEBUG
				if (Renderer::GetRendererAPI() == RendererAPI::API::OPENGL)
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			#endif

			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			++s_WindowCount;
		}
		
		// -- Graphics Context Creation --
		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();
		
		// -- GLFW Window User ptr & VSYNC --
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// -- Set GLFW Callbacks --
		SetGLFWEventCallbacks();
	}

	void WindowsWindow::Shutdown()
	{
		KS_PROFILE_FUNCTION();
		KS_ENGINE_INFO("Destroying GLFW Window '{0}'", m_Data.Title);
		glfwDestroyWindow(m_Window);
		--s_WindowCount;

		if (s_WindowCount == 0)
		{
			KS_ENGINE_INFO("Terminating GLFW");
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		KS_PROFILE_FUNCTION();
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}


	// -- GLFW Event Callbacks --
	void WindowsWindow::SetGLFWEventCallbacks() const
	{
		// Window/Application Events
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int w, int h)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = w;
				data.Height = h;

				WindowResizeEvent event(w, h);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});


		// Key Events
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(static_cast<KEYCODE>(key), 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(static_cast<KEYCODE>(key));
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(static_cast<KEYCODE>(key), 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent event(static_cast<KEYCODE>(keycode));
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
					MouseButtonPressedEvent event(static_cast<MOUSECODE>(button));
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(static_cast<MOUSECODE>(button));
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