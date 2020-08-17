workspace "KaimosEngine"
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

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    include
    {
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
        define "KS_DEBUG"
        symbols "On"
    filter "configurations:Release"
        define "KS_RELEASE"
        optimize "On"
    filter "configurations:Dist"
        define "KS_DIST"
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

    include
    {
        "KaimosEngine/vendor/spdlog/include"
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
        define "KS_DEBUG"
        symbols "On"
    filter "configurations:Release"
        define "KS_RELEASE"
        optimize "On"
    filter "configurations:Dist"
        define "KS_DIST"
        optimize "On"