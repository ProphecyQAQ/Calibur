#include "hzpch.h"
#include "Calibur/Renderer/RenderCommand.h"

namespace Calibur
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
	int RenderCommand::s_DrawCallsCount = 0;
}
