
-- Calibur Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/Calibur/vendor/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/Calibur/vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/Calibur/vendor/Box2D/include"
IncludeDir["filewatch"] = "%{wks.location}/Calibur/vendor/filewatch"
IncludeDir["GLFW"] = "%{wks.location}/Calibur/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Calibur/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Calibur/vendor/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Calibur/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/Calibur/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/Calibur/vendor/entt/include"
IncludeDir["mono"] = "%{wks.location}/Calibur/vendor/mono/include"
IncludeDir["shaderc"] = "%{wks.location}/Calibur/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Calibur/vendor/SPIRV-Cross"
IncludeDir["Assimp"] = "%{wks.location}/Calibur/vendor/assimp/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}

Library["Assimp_Debug"] = "%{wks.location}/Calibur/vendor/assimp/bin/Debug/assimp-vc142-mtd.lib"
Library["Assimp_Release"] = "%{wks.location}/Calibur/vendor/assimp/bin/Release/assimp-vc142-mt.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

Binaries = {}

Binaries['Assimp_Debug'] = "%{wks.location}/Calibur/vendor/assimp/bin/Debug/assimp-vc142-mtd.dll"
Binaries['Assimp_Release'] = "%{wks.location}/Calibur/vendor/assimp/bin/Release/assimp-vc142-mt.dll"
