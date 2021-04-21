include "./vendor/premake/premake_customization/solution_items.lua"

workspace "Kaimos"
    architecture "x86_64"
    startproject "KaimosEditor"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    solution_items
    {
        ".editorconfig"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    -- Include directories relative to Root Folder (or Solution Directory)
    IncludeDir = {}
    IncludeDir["GLFW"]      = "%{wks.location}/KaimosEngine/vendor/GLFW/include"
    IncludeDir["Glad"]      = "%{wks.location}/KaimosEngine/vendor/Glad/include"
    IncludeDir["ImGui"]     = "%{wks.location}/KaimosEngine/vendor/imgui"
    IncludeDir["glm"]       = "%{wks.location}/KaimosEngine/vendor/glm"
    IncludeDir["stb_image"] = "%{wks.location}/KaimosEngine/vendor/stb_image"
    IncludeDir["entt"]      = "%{wks.location}/KaimosEngine/vendor/entt/include"
    IncludeDir["yaml"]      = "%{wks.location}/KaimosEngine/vendor/yaml/include"
    IncludeDir["ImGuizmo"]  = "%{wks.location}/KaimosEngine/vendor/ImGuizmo"
    IncludeDir["ImNodes"]   = "%{wks.location}/KaimosEngine/vendor/ImNodes"

    -- How To Add a Library: --
    -- Just include it here, on top, as a new IncludeDir, 
    -- then add it down there (or in its vendor file) in includedirs (under defines{})
    -- and finally add it in links if needed (aside anything needed, such as dlls)
    ---------------------------

    group "Dependencies"
        include "vendor/premake"
        include "KaimosEngine/vendor/GLFW"  -- Includes GLFW Premake File
        include "KaimosEngine/vendor/Glad"  -- Includes Glad Premake File
        include "KaimosEngine/vendor/imgui" -- Includes ImGui Premake File
        include "KaimosEngine/vendor/yaml"  -- Includes yaml Premake File
    group ""

    include "KaimosEngine"
    include "KaimosEditor"