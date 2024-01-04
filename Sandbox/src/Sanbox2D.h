#pragma once

#include "Calibur.h"
#include "ParticleSystem.h"

class Sanbox2D : public Calibur::Layer
{
public:
	Sanbox2D();
	virtual ~Sanbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Calibur::TimeStep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Calibur::Event& e) override;
private:
	Calibur::OrthographicCameraController m_CameraController;


	Calibur::Ref<Calibur::Shader> m_FlatColorShader;
	Calibur::Ref<Calibur::VertexArray> m_SquareVA;

	Calibur::Ref<Calibur::Texture2D> m_CheckerboardTexture;
	Calibur::Ref<Calibur::Texture2D> m_SpriteSheet;
	Calibur::Ref<Calibur::SubTexture2D> m_TextureStairs;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f};

	ParticleProps m_Particle;
	ParticleSystem m_ParticleSystem;
	
	uint32_t m_MapWidth, m_MapHeight;
	std::unordered_map<char, Calibur::Ref<Calibur::SubTexture2D>> m_TextureMap;
};
