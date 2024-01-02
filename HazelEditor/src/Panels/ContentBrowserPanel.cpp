#include "hzpch.h"
#include "ContentBrowserPanel.h"
#include <imgui/imgui.h>


namespace Hazel
{
	static std::filesystem::path s_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(s_AssetPath)
	{
	}

	void ContentBrowserPanel::OnImguiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(s_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		
		auto it = std::filesystem::directory_iterator(m_CurrentDirectory);
		for (auto& directoryEntry : it)
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetPath);
			
			std::string filenameString = relativePath.filename().string();
			if (directoryEntry.is_directory())
			{
				if (ImGui::Button(filenameString.c_str()))
				{
					m_CurrentDirectory /= directoryEntry.path().filename();
				}
			}
			else
			{
				if (ImGui::Button(filenameString.c_str())) 
				{
				}
			}
		}

		ImGui::End();
	}
}
