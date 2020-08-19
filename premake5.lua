workspace "Kaimos"
    architecture "x64"

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

-- How To Add a Library: --
-- Just include it here, on top, as a new IncludeDir, 
-- then add it down there in includedirs (under files)
-- and finally add it in links if needed (aside anything
-- needed, such as dlls)
---------------------------

include "KaimosEngine/vendor/GLFW" -- Includes GLFW Premake File

-- Kaimos Engine Settings --
project "KaimosEngine"
    location "KaimosEngine"
    kind "SharedLib"
    language "C++"
    
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
        "%{prj.name}/src/**.cpp"
    }

    -- Include Directories to project (to avoid typing a lot in each #include) --
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}"
    }

    -- Kaimos Engine Project Links --
    links
    {
        "GLFW",
        "opengl32.lib"
    }

    -- Systems --
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "KS_BUILD_DLL",
            "KS_PLATFORM_WINDOWS"
        }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/KaimosEditor")
        }

    -- Configurations --
    filter "configurations:Debug"
        defines { "KS_DEBUG", "KS_ENABLE_ASSERTS" }
        symbols "On"
    filter "configurations:Release"
        defines "KS_RELEASE"
        optimize "On"
    filter "configurations:Dist"
        defines "KS_DIST"
        optimize "On"


-- Kaimos Editor Settings --
project "KaimosEditor"
    location "KaimosEditor"
    kind "ConsoleApp"
    language "C++"

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
        "KaimosEngine/src"
    }

    links
    {
        "KaimosEngine"
    }

    -- Systems --
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "KS_PLATFORM_WINDOWS"
        }

    -- Configurations --
    filter "configurations:Debug"
        defines "KS_DEBUG"
        symbols "On"
    filter "configurations:Release"
        defines "KS_RELEASE"
        optimize "On"
    filter "configurations:Dist"
        defines "KS_DIST"
        optimize "On"