#pragma once

#include "Hazel/Core/Base.h"
#include "Hazel/Renderer/Texture.h"

#include <filesystem>

namespace Hazel
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImguiRender();

	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};
}
