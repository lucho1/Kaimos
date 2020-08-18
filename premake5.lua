workspace "Kaimos"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "KaimosEngine"
    location "KaimosEngine"
    kind "SharedLib"
    language "C++"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "kspch.h"
    pchsource "KaimosEngine/src/kspch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include"
    }

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

    filter "configurations:Debug"
        defines "KS_DEBUG"
        symbols "On"
    filter "configurations:Release"
        defines "KS_RELEASE"
        optimize "On"
    filter "configurations:Dist"
        defines "KS_DIST"
        optimize "On"


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

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "KS_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "KS_DEBUG"
        symbols "On"
    filter "configurations:Release"
        defines "KS_RELEASE"
        optimize "On"
    filter "configurations:Dist"
        defines "KS_DIST"
        optimize "On"