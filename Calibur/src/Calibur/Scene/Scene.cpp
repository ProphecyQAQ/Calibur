#include "hzpch.h"
#include "Calibur/Scene/Scene.h"
#include "Calibur/Scene/Entity.h"
#include "Calibur/Scene/Components.h"
#include "Calibur/Renderer/Renderer2D.h"
#include "Calibur/Renderer/Material.h"
#include "Calibur/Renderer/Renderer.h"

#include <glm/glm.hpp>

namespace Calibur
{
	static struct SkyboxData
	{
		Ref<VertexArray> vao;
		Ref<VertexBuffer> vbo;
		Ref<IndexBuffer> ibo;
		Ref<TextureCube> texture;
		Ref<Shader> shader;
	}s_Skybox;

	static float skyboxVertices[24] =
	{
		+1, +1, +1,
		-1, +1, +1,
		-1, -1, +1,
		+1, -1, +1,
		+1, +1, -1,
		-1, +1, -1,
		-1, -1, -1,
		+1, -1, -1,
	};

	static uint32_t skyboxIndices[6 * 6] =  // in fact, uint8_t is enough
	{
		1,5,2, 2,5,6,  // left
		3,7,0, 0,7,4,  // right
		2,6,3, 3,6,7,  // back
		0,4,1, 1,4,5,  // front
		6,5,7, 7,5,4,  // bottom
		1,2,0, 0,2,3,  // top
	};

	Scene::Scene()
	{
		#if 0
		entt::entity entity = m_Registry.create(); // return a uint32_t 

		m_Registry.emplace<TransformComponent>(entity);

		if (m_Registry.try_get<TransformComponent>(entity))
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		auto view = m_Registry.view<TransformComponent>();
		#endif

		m_TransformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 1);
		m_MaterialUniform = UniformBuffer::Create(sizeof(MaterialUniforms), 2);

		s_Skybox.texture = TextureCube::Create("Resources/CubeMap/skybox");
		s_Skybox.vbo = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices));
		s_Skybox.ibo = IndexBuffer::Create(skyboxIndices, sizeof(skyboxIndices));
		BufferLayout layout = {{ ShaderDataType::Float3, "a_Position" }};
		s_Skybox.vbo->SetLayout(layout);

		s_Skybox.vao = VertexArray::Create();
		s_Skybox.vao->SetIndexBuffer(s_Skybox.ibo);
		s_Skybox.vao->AddVertexBuffer(s_Skybox.vbo);
		s_Skybox.shader = Renderer::GetShaderLibrary()->Get("Skybox");

		m_SceneEnv = CreateRef<SceneEnvironment>("Resources/sIbl/hallstatt4_hd.hdr", true);
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateEditor(TimeStep ts, EditorCamera& camera)
	{
		// Skybox
		{
			RenderCommand::SetDepthTest(false);
			s_Skybox.shader->Bind();
			//s_Skybox.texture->Bind(1);
			m_SceneEnv->GetSkybox()->Bind(1);
			RenderCommand::DrawIndexed(s_Skybox.vao);
			s_Skybox.shader->Unbind();
			RenderCommand::SetDepthTest(true);
		}

		// Light
		SceneLightData lightData;
		{
			//Directional Light 
			// Now only one dir light
			auto view = m_Registry.view<DirectionalLightComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto& light = view.get<DirectionalLightComponent>(entity);
				auto& transform = view.get<TransformComponent>(entity);
				glm::vec3 direction = -glm::normalize(glm::mat3(transform.GetTransform()) * glm::vec3(1.0f));
				lightData.DirectionalLights =
				{
					light.Radiance,
					direction,
					light.Intensity
				};
			}
		}
		Renderer::SubmitLight(lightData);

		// Render 2D
		Renderer2D::BeginScene(camera);
		
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}
		
		Renderer2D::EndScene();
		
		// Render 3D
		Renderer::BeginScene(camera);
		
		auto view = m_Registry.view<TransformComponent, MeshComponent>();
		for (auto entity : view)
		{
			auto& transform = view.get<TransformComponent>(entity);

			auto& mesh = view.get<MeshComponent>(entity);
			auto& submeshs = mesh.mesh->GetSubMeshes();

			m_TransformBuffer->SetData(&transform.GetTransform(), sizeof(glm::mat4));
			for (size_t id = 0; id < submeshs.size(); id++)
			{
				auto& material = mesh.mesh->GetMaterials()[submeshs[id].MaterialIndex];
				m_MaterialUniform->SetData((void*) & material->GetMaterialUniforms(), sizeof(MaterialUniforms));

				material->GetShader()->Bind();
				material->GetDiffuseMap()->Bind(0);
				material->GetNormalMap()->Bind(1);
				material->GetSpecMap()->Bind(2);
				material->GetRoughnessMap()->Bind(3);

				Renderer::RenderMesh(mesh.mesh, id);

				material->GetShader()->Unbind();
			}
		}

		Renderer::EndScene();
	}

	void Scene::OnUpdateRuntime(TimeStep ts)
	{
		// Update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (nsc.Instance == nullptr)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(ts); 
			});

		}

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view) 
			{
				auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}
		
		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			}

			Renderer2D::EndScene();


			Renderer::BeginScene(*mainCamera, cameraTransform);
			
			auto view = m_Registry.view<TransformComponent, MeshComponent>();
			for (auto entity : view)
			{
				auto& transform = view.get<TransformComponent>(entity);

				auto& mesh = view.get<MeshComponent>(entity);
				auto& submeshs = mesh.mesh->GetSubMeshes();

				m_TransformBuffer->SetData(&transform.GetTransform(), sizeof(glm::mat4));
				for (size_t id = 0; id < submeshs.size(); id++)
				{
					auto& material = mesh.mesh->GetMaterials()[submeshs[id].MaterialIndex];

					material->GetShader()->Bind();
					material->GetDiffuseMap()->Bind(0);
					material->GetNormalMap()->Bind(1);
					material->GetSpecMap()->Bind(2);
					material->GetRoughnessMap()->Bind(3);

					Renderer::RenderMesh(mesh.mesh, id);

					material->GetShader()->Unbind();
				}
			}

			Renderer::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (cameraComponent.FixedAspectRatio == false)
			{
				cameraComponent.Camera.SetViewportSize(width, height);  
			}
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary) return Entity(entity, this);
		}
		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component)
	{
	}
}
