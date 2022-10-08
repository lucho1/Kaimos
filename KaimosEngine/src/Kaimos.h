#ifndef _KAIMOS_H_
#define _KAIMOS_H_

// For Kaimos Applications use (clients)
#include "Core/Core.h"

// --- Engine Basic Stuff ---
#include "Core/Application/Application.h"
#include "Core/Application/Layers/Layer.h"
#include "Core/Application/Input/Input.h"
#include "Core/Application/Input/KaimosInputCodes.h"

#include "Core/Utils/Log/Log.h"
#include "Core/Utils/Time/Timestep.h"

#include "Core/Resources/Resource.h"

#include "ImGui/ImGuiLayer.h"

#include "Scene/Scene.h"
#include "Scene/ECS/Entity.h"
#include "Scene/ECS/ScriptableEntity.h"
#include "Scene/ECS/Components.h"

// -- Engine Renderer --
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/Foundations/RenderCommand.h"

#include "Renderer/Resources/Buffer.h"
#include "Renderer/Resources/Shader.h"
#include "Renderer/Resources/Framebuffer.h"
#include "Renderer/Resources/Texture.h"

#include "Renderer/Cameras/Camera.h"
#include "Renderer/Cameras/CameraController.h"

// --- Entry Point ---
// Entry point must be inicluded only where the EntryPoint (main) is actually
// needed, otherwise, it would be included everywhere (if included here)

#endif //_KAIMOS_H_
