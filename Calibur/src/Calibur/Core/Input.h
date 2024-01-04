#pragma once

#include "Calibur/Core/Base.h"
#include "Calibur/Core/KeyCodes.h"
#include "Calibur/Core/MouseCodes.h"

namespace Calibur
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
