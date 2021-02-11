-- Kaimos Engine Settings --
project "KaimosEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"
    
    -- Directories for target (needed build files) and Obj Files (unwanted build files) --
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    -- Precompiled Header --
    pchheader "kspch.h"
    pchsource "src/kspch.cpp"

    -- Files to Add into the VS Project --
    files
    {
        "src/**.h",
        "src/**.cpp",
        "vendor/glm/glm/**.hpp",
        "vendor/glm/glm/**.inl",
        "vendor/stb_image/**.cpp",
        "vendor/stb_image/**.h"
    }

    -- Definitions for the project (global ones)
    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }

    -- Include Directories to project (to avoid typing a lot in each #include) --
    includedirs
    {
        "src",
        "vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml}"
    }

    -- Kaimos Engine Project Links --
    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "yaml-cpp",
        "opengl32.lib"
    }

    -- Systems --
    filter "system:windows"
        systemversion "latest"

        defines
        {
            --"KS_BUILD_DLL"
        }

    -- Configurations --
    filter "configurations:Debug"
        defines { "KS_DEBUG", "KS_ENABLE_ASSERTS" }
        runtime "Debug"
        symbols "On"
    filter "configurations:Release"
        defines { "KS_RELEASE", "KS_ENABLE_ASSERTS" }
        runtime "Release"
        optimize "On"
    filter "configurations:Dist"
        defines "KS_DIST"
        runtime "Release"
        optimize "On"