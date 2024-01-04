#pragma once
#include "Calibur/Core/Layer.h"

#include "Calibur/Events/KeyEvent.h"
#include "Calibur/Events/MouseEvent.h"
#include "Calibur/Events/ApplicationEvent.h"

namespace Calibur
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
