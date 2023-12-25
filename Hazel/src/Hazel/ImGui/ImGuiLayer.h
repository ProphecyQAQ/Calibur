#pragma once
#include "Hazel/Core/Layer.h"

#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/ApplicationEvent.h"

namespace Hazel
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();
		 
		void SetBlockEvents(bool block) { m_BlockEvents = block; }
	private:
		void SetDarkThemeColor();
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}
