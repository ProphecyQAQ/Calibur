#include <Calibur.h>
#include "Calibur/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Calibur
{

	class CaliburEditor : public Calibur::Application
	{
	public:
		CaliburEditor()
			: Application("Calibur Editor")
		{
			//PushLayer(new ExampleLayer());
			PushLayer(new EditorLayer());
		}

		~CaliburEditor() {


		}

	};

	Application* CreateApplication(ApplicationCommandLineArgs args) {
		return new CaliburEditor();
	}
}
