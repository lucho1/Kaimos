-- Kaimos Editor Settings --
project "KaimosEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/KaimosEngine/vendor/spdlog/include",
        "%{wks.location}/KaimosEngine/src",
        "%{wks.location}/KaimosEngine/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}"
    }

    links
    {
        "KaimosEngine"
    }

    -- Systems --
    filter "system:windows"
        systemversion "latest"

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