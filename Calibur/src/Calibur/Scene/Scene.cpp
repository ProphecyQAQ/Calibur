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
	Scene::Scene()
	{
		#if 0
		entt::entity entity =  m_Registry.create(); // return a uint32_t 

		m_Registry.emplace<TransformComponent>(entity);

		if (m_Registry.try_get<TransformComponent>(entity))
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
		
		auto view = m_Registry.view<TransformComponent>();
		#endif

		m_TransformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 1);
		m_MaterialUniform = UniformBuffer::Create(sizeof(MaterialUniforms), 2);
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

			m_TransformBuffer->SetData(&transform.GetTransform(), sizeof(glm::mat4));
			auto& submeshs = mesh.mesh->GetSubMeshes();
			for (size_t id = 0; id < submeshs.size(); id++)
			{
				auto& material = mesh.mesh->GetMaterials()[submeshs[id].MaterialIndex];

				material->GetShader()->Bind();
				
				//Ref<Shader> shader = Renderer::GetShaderLibrary()->Get("textureToScreen");
				//shader->Bind();
				material->GetAlbedoMap()->Bind(0);
				material->GetNormalMap()->Bind(1);
				//material->GetMetallicMap()->Bind(2);
				material->GetRoughnessMap()->Bind(3);
				float vertices[] = {
					// Î»ÖÃ            // ÎÆÀí×ø±ê
					-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
					 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
					 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
					-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				};
				uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

				Ref<VertexArray> vao = VertexArray::Create();
				BufferLayout layout = {
					{ ShaderDataType::Float3, "a_Position" },
					{ ShaderDataType::Float2, "a_TexCoord" }
				};
				Ref<VertexBuffer> vbo = VertexBuffer::Create(vertices, sizeof(vertices));
				vbo->SetLayout(layout);
				vao->AddVertexBuffer(vbo);
				Ref<IndexBuffer> ibo = IndexBuffer::Create(indices, sizeof(indices));
				vao->SetIndexBuffer(ibo);
				//Renderer::Submit(shader, vao);


				Renderer::RenderMesh(mesh.mesh, id);

				material->GetShader()->Unbind();

				//break;
			}
			//Renderer::Submit(Renderer::GetShaderLibrary()->Get("Texture3D"), mesh.mesh->GetVertexArray());
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

		//Render 2D sprites
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
}
