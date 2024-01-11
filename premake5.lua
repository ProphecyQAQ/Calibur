include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Calibur"
	architecture "x86_64"
	startproject "CaliburEditor"

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

group "Dependencies"
	include "vendor/premake"
	include "Calibur/vendor/GLFW"
	include "Calibur/vendor/Glad"
	include "Calibur/vendor/imgui"
	include "Calibur/vendor/yaml-cpp"
	include "Calibur/vendor/shaderc_file"
group ""

include "Calibur"
include "Sandbox"
include "CaliburEditor"
