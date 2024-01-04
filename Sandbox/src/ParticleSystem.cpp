#include "ParticleSystem.h"
#include "Random.h"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

ParticleSystem::ParticleSystem(uint32_t MaxParticles)
	:m_PoolIndex(MaxParticles - 1)
{
	m_ParticlePool.resize(MaxParticles);
}

void ParticleSystem::OnUpdate(Calibur::TimeStep ts)
{
	for (auto& particle : m_ParticlePool)
	{
		if (particle.Active == false)
			continue;

		if (particle.LifeRemaining <= 0.0f)
		{
			particle.Active = false;
			continue;
		}

		particle.Position += particle.Velocity * (float)ts;
		particle.Rotation += 0.01f * ts;
		particle.LifeRemaining -= ts;
	}

}

void ParticleSystem::OnRender(Calibur::OrthographicCamera& camera)
{
	Calibur::Renderer2D::BeginScene(camera);
	for (auto& particle : m_ParticlePool)
	{
		if (particle.Active == false)
			continue;
		
		auto life = particle.LifeRemaining / particle.LifeTime;

		glm::vec4 color = glm::lerp(particle.ColorBegin, particle.ColorEnd, life);
		float size = glm::lerp(particle.SizeBegin, particle.SizeEnd, life);
		
		glm::vec3 position = { particle.Position.x, particle.Position.y, 0.2f };
		Calibur::Renderer2D::DrawRotateQuad(position, { size, size }, particle.Rotation, color);
	}
	Calibur::Renderer2D::EndScene();
}

void ParticleSystem::Emit(const ParticleProps& particleProps)
{
	Particle& particle = m_ParticlePool[m_PoolIndex];
	particle.Active = true;
	particle.Position = particleProps.Position;
	particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();

	// Velocity
	particle.Velocity = particleProps.Velocity;
	particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Float() - 0.5f);
	particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Float() - 0.5f);

	// Color
	particle.ColorBegin = particleProps.ColorBegin;
	particle.ColorEnd = particleProps.ColorEnd;

	particle.LifeTime = particleProps.LifeTime;
	particle.LifeRemaining = particleProps.LifeTime;
	particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);
	particle.SizeEnd = particleProps.SizeEnd;

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}
