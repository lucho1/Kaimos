workspace "Kaimos"
    architecture "x64"
    startproject "KaimosEditor"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to Root Folder (or Solution Directory)
IncludeDir = {}
IncludeDir["GLFW"] = "KaimosEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "KaimosEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "KaimosEngine/vendor/imgui"
IncludeDir["glm"] = "KaimosEngine/vendor/glm"

-- How To Add a Library: --
-- Just include it here, on top, as a new IncludeDir, 
-- then add it down there in includedirs (under files)
-- and finally add it in links if needed (aside anything
-- needed, such as dlls)
---------------------------

group "Dependencies"
    include "KaimosEngine/vendor/GLFW" -- Includes GLFW Premake File
    include "KaimosEngine/vendor/Glad" -- Includes Glad Premake File
    include "KaimosEngine/vendor/imgui" -- Includes ImGui Premake File
group ""

-- Kaimos Engine Settings --
project "KaimosEngine"
    location "KaimosEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    
    -- Directories for target (needed build files) and Obj Files (unwanted build files) --
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- Precompiled Header --
    pchheader "kspch.h"
    pchsource "KaimosEngine/src/kspch.cpp"

    -- Files to Add --
    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    -- Definitions for the project (global ones)
    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    -- Include Directories to project (to avoid typing a lot in each #include) --
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}"
    }

    -- Kaimos Engine Project Links --
    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    -- Systems --
    filter "system:windows"
        systemversion "latest"

        defines
        {
            "KS_BUILD_DLL",
            "KS_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE"
        }

        --postbuildcommands
        --{
        --    ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/KaimosEditor/\"")
        --}

    -- Configurations --
    filter "configurations:Debug"
        defines { "KS_DEBUG", "KS_ENABLE_ASSERTS" }
        runtime "Debug"
        symbols "On"
    filter "configurations:Release"
        defines "KS_RELEASE"
        runtime "Release"
        optimize "On"
    filter "configurations:Dist"
        defines "KS_DIST"
        runtime "Release"
        optimize "On"


-- Kaimos Editor Settings --
project "KaimosEditor"
    location "KaimosEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "KaimosEngine/vendor/spdlog/include",
        "KaimosEngine/src",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}"
    }

    links
    {
        "KaimosEngine"
    }

    -- Systems --
    filter "system:windows"
        systemversion "latest"

        defines
        {
            "KS_PLATFORM_WINDOWS"
        }

    -- Configurations --
    filter "configurations:Debug"
        defines "KS_DEBUG"
        runtime "Debug"
        symbols "On"
    filter "configurations:Release"
        defines "KS_RELEASE"
        runtime "Release"
        optimize "On"
    filter "configurations:Dist"
        defines "KS_DIST"
        runtime "Release"
        optimize "On"