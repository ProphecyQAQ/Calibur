#pragma once

#include "Hazel/Core/Base.h"

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
	};
}
