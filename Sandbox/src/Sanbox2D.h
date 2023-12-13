#pragma once

#include "Hazel.h"

class Sanbox2D : public Hazel::Layer
{
public:
	Sanbox2D();
	virtual ~Sanbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Hazel::TimeStep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Hazel::Event& e) override;
private:
	Hazel::OrthographicCameraController m_CameraController;


	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;

	Hazel::Ref<Hazel::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f};
};
