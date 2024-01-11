project "Shaderc_file"
	kind "StaticLib"
    language "C++"
    cppdialect "C++17" 
    staticruntime "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "libshaderc_util/include/libshaderc_util/**.h",
        "libshaderc_util/src/**.cc",
        "libshaderc_util/src/**.h",

        "glslc/**.cc",
        "glslc/**.h"
    }

	includedirs
	{
		"libshaderc_util/include",
        "libshaderc_util/include/libshaderc_util",
        "%{IncludeDir.VulkanSDK}"
	}

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"