#pragma once

#include "Calibur/Core/Base.h"
#include "Calibur/Renderer/Texture.h"

#include <filesystem>

namespace Calibur
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
