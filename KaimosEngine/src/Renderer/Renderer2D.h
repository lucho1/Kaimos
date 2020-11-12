#ifndef _RENDERER_2D_
#define _RENDERER_2D_

#include "Cameras/OrthographicCamera.h"

namespace Kaimos {

	class Renderer2D // Won't deal with Storage (will have 0 storage), no static stuff, just render commands
	{
	public:

		static void Init();
		static void Shutdown();
		
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		
		static void DrawQuad(const glm::vec2& position, const glm::vec2 size, float rotation, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2 size, float rotation, const glm::vec4& color);
	};

}

#endif