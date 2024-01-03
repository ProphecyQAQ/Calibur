project "HazelEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Hazel/vendor/spdlog/include",
		"%{wks.location}/Hazel/src",
		"%{wks.location}/Hazel/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Assimp}"
	}
	
	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}

	links
	{
		"Hazel",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		symbols "on"

		postbuildcommands
		{
			'{COPY} "%{Binaries.Assimp_Debug}" "%{cfg.targetdir}"',
		}

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "on"

		postbuildcommands
		{
			'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"',
		}


	filter "configurations:Dist"
		defines "HZ_DIST"
		runtime "Release"
		optimize "on"
