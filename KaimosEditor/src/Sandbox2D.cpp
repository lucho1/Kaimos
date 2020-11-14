#include "Sandbox2D.h"

#include <imgui.h>

// TODO: TEMP
#include "Platform/OpenGL/OpenGLShader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


template<typename Fn>
class Timer
{
public:

	Timer(const char* name, Fn&& func) : m_Name(name), m_Stopped(false), m_FuncReturn(func)
	{
		m_StartTimePoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped) Stop();
	}

	void Stop()
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
		float duration = (end - start) * 0.001f; // in ms
		m_Stopped = true;

		//std::cout << m_Name  << " Duration: " << duration << "ms (" << duration * 1000.0f << "us)\n";
		m_FuncReturn({ m_Name, duration }); // Setup the return function to ProfileResult (with proper name & duration value)
	}

private:

	Fn m_FuncReturn;
	const char* m_Name;
	bool m_Stopped;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
};

// This next lambda returns (by reference, &) a profile result and it's pusback
#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult resRet) { m_ProfileResults.push_back(resRet); })



Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	m_CheckerTexture = Kaimos::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Kaimos::Timestep dt)
{
	PROFILE_SCOPE("Sandbox2D::OnUpdate"); 

	// --- UPDATE ---
	{
		PROFILE_SCOPE("		Sandbox2D::OnUpdate::CameraController");
		m_CameraController.OnUpdate(dt);
	}

	// --- RENDER ---
	{
		PROFILE_SCOPE("		Sandbox2D::OnUpdate::RenderingPreparation");
		Kaimos::RenderCommand::SetClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
		Kaimos::RenderCommand::Clear();
		Kaimos::Renderer2D::BeginScene(m_CameraController.GetCamera());
	}
	
	// -- Scene --
	{
		PROFILE_SCOPE("		Sandbox2D::OnUpdate::Rendering");
		Kaimos::Renderer2D::DrawQuad(glm::vec2(-1.0f, 0.0f), glm::vec2(0.8f), 45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		Kaimos::Renderer2D::DrawQuad(glm::vec2(0.5f, -0.5f), glm::vec2(0.5f, 0.75f), 20.0f, { 0.2f, 0.3f, 0.8f, 1.0f });
		Kaimos::Renderer2D::DrawQuad(glm::vec3(0.2f, 0.5f, -0.1f), glm::vec2(10.0f), 0.0f, m_CheckerTexture);
		Kaimos::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnUIRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Squares Color", glm::value_ptr(m_Color));

	for (auto& result : m_ProfileResults)
	{
		char label[150];
		strcpy(label, "%.3f ms  ");
		strcat(label, result.name);
		ImGui::Text(label, result.time);
	}

	m_ProfileResults.clear();
	ImGui::End();
}

void Sandbox2D::OnEvent(Kaimos::Event& ev)
{
	m_CameraController.OnEvent(ev);
}