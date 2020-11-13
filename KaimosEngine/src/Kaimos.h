#ifndef _KAIMOS_H_
#define _KAIMOS_H_

// For Kaimos Applications use (clients)
#include "Core/Core.h"

// --- Engine Basic Stuff ---
#include "Core/Application.h"
#include "Core/Layers/Layer.h"
#include "Core/Log/Log.h"
#include "Core/Time/Timestep.h"

#include "Core/Input/Input.h"
#include "Core/Input/KaimosInputCodes.h"

#include "ImGui/ImGuiLayer.h"


// -- Engine Renderer --
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/RenderCommand.h"

#include "Renderer/Buffer.h"
#include "Renderer/Resources/Shader.h"
#include "Renderer/Resources/Texture.h"

#include "Renderer/Cameras/OrthographicCamera.h"
#include "Renderer/Cameras/OrtographicCameraController.h"


// --- Entry Point ---
// Entry point must be inicluded only where the EntryPoint (main) is actually
// needed, otherwise, it would be included everywhere (if included here)

#endif