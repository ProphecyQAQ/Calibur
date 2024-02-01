#include "hzpch.h"
#include "Calibur/Scene/Scene.h"
#include "Calibur/Scene/ScriptableEntity.h"
#include "Calibur/Scene/Entity.h"
#include "Calibur/Scene/Components.h"
#include "Calibur/Renderer/SceneRenderer.h"
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
		Ref<Shader> shader;
		Ref<TextureCube> texture;
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
		s_Skybox.shader = Renderer::GetShaderLibrary()->Get("Skybox");
		s_Skybox.vao = VertexArray::Create();
		Ref<VertexBuffer> vbo = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices));
		vbo->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
			});
		s_Skybox.vao->AddVertexBuffer(vbo);
		Ref<IndexBuffer> ibo = IndexBuffer::Create(skyboxIndices, sizeof(skyboxIndices));
		s_Skybox.vao->SetIndexBuffer(ibo);
		//s_Skybox.texture = TextureCube::Create("assets/sIbl/hallstatt4_hd.hdr");

		m_MaterialUniform = UniformBuffer::Create(sizeof(MaterialUniforms), 2);

		m_SceneEnv = CreateRef<SceneEnvironment>("assets/sIbl/hallstatt4_hd.hdr", true);
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateChildEntity({}, name);
	}

	Entity Scene::CreateChildEntity(Entity parent, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<IDComponent>(UUID());

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		auto& relationship = entity.AddComponent<RelationshipComponent>();

		if (parent)
			entity.SetParent(parent);

		return entity;
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<IDComponent>(uuid);
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateEditor(Ref<SceneRenderer> renderer, TimeStep ts, EditorCamera& camera)
	{
		// Record SceneRenderer
		m_Renderer = renderer;

		// Light
		SceneLightData lightData;
		{
			lightData.DirectionalLightCount = 0;
			lightData.PointLights.clear();

			// Directional Light 
			// Now only one dir light
			auto view = m_Registry.view<DirectionalLightComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto& light = view.get<DirectionalLightComponent>(entity);
				auto& transform = view.get<TransformComponent>(entity);
				glm::vec3 direction = -glm::normalize(glm::mat3(transform.GetTransform()) * glm::vec3(1.0f));
				lightData.DirectionalLights[lightData.DirectionalLightCount++] =
				{
					light.Radiance,
					light.Intensity,
					direction,
				};
			}

			// Point Light
			auto view1 = m_Registry.view<PointLightComponent, TransformComponent>();
			lightData.PointLights.resize(view1.size_hint());
			uint32_t pointLightIndex = 0;
			for (auto entity : view1)
			{
				auto& light = view1.get<PointLightComponent>(entity);
				auto& transform = view1.get<TransformComponent>(entity);
				lightData.PointLights[pointLightIndex++] = 
				{
					light.Radiance,
					light.Intensity,
					transform.Translation,
					light.Radius,
					light.SourceSize,
					light.CastShadow == true ? 1u : 0u
				};
			}
		}

		// Render Scene Init
		renderer->SetScene(shared_from_this());
		renderer->BeginScene(
			{
				camera.GetProjection(),
				camera.GetViewMatrix(),
				camera.GetPosition(),
				camera.GetFov(), camera.GetNearClip(), camera.GetFarClip(), camera.GetAspectRatio()
			});
		renderer->SubmitLight(lightData);
		// Skybox
		Renderer::BeginScene();
		{
			RenderCommand::SetDepthTest(false);
			s_Skybox.shader->Bind();
			//s_Skybox.texture->Bind(1);
			m_SceneEnv->GetSkybox()->Bind(1);
			//m_SceneEnv->GetIrradianceMap()->Bind(1);
			//m_SceneEnv->GetPreFilterMap()->Bind(1);
			RenderCommand::DrawIndexed(s_Skybox.vao);
			s_Skybox.shader->Unbind();
			RenderCommand::SetDepthTest(true);
		}
		Renderer::EndScene();

		// Set Environment map
		m_SceneEnv->GetIrradianceMap()->Bind(6);
		m_SceneEnv->GetPreFilterMap()->Bind(5);

		RenderCommand::SetFaceCulling(false);

		// Render 2D
		RenderScene2D();
		
		// Render 3D
		RenderScene3D();

		renderer->EndScene();
	}

	void Scene::OnUpdateRuntime(Ref<SceneRenderer> renderer, TimeStep ts)
	{
		// 
		m_Renderer = renderer;

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
				lightData.DirectionalLights[lightData.DirectionalLightCount++] =
				{
					light.Radiance,
					light.Intensity,
					direction,
				};
			}
		}
		
		// Find camera data
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		glm::vec3 cameraPosition;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view) 
			{
				auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					cameraPosition = transform.Translation;
					break;
				}
			}
		}

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
		
		// Begin to render
		if (mainCamera)
		{		
			renderer->SetScene(shared_from_this());
			renderer->BeginScene({mainCamera->GetProjection(), glm::inverse(cameraTransform), cameraPosition});

			renderer->SubmitLight(lightData);

			// Skybox
			Renderer::BeginScene();
			{
				RenderCommand::SetDepthTest(false);
				RenderCommand::SetFaceCulling(false);
				s_Skybox.shader->Bind();
				//s_Skybox.texture->Bind(1);
				m_SceneEnv->GetSkybox()->Bind(1);
				//m_SceneEnv->GetIrradianceMap()->Bind(1);
				//m_SceneEnv->GetPreFilterMap()->Bind(1);
				RenderCommand::DrawIndexed(s_Skybox.vao);
				s_Skybox.shader->Unbind();
				RenderCommand::SetDepthTest(true);
				//RenderCommand::SetFaceCulling(true);
			}
			Renderer::EndScene();
			
			//Render 2D
			RenderScene2D();

			// Render 3D	
			RenderScene3D();

			renderer->EndScene();
		}
	}

	void Scene::RenderScene2D()
	{
		//Render 2D
		Renderer2D::BeginScene();
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
		}
		Renderer2D::EndScene();
	}

	void Scene::RenderScene3D(Ref<Shader> shader)
	{
		Renderer::BeginScene();
			
		auto view = m_Registry.view<TransformComponent, MeshComponent>();
		for (auto entity : view)
		{
			auto& transform = view.get<TransformComponent>(entity);

			auto& mesh = view.get<MeshComponent>(entity);
			if (!mesh.mesh->IsLoaded()) continue;
			auto& submeshs = mesh.mesh->GetSubMeshes();

			m_Renderer->GetTransformUB()->SetData(&transform.GetTransform(), sizeof(glm::mat4));
			for (size_t id = 0; id < submeshs.size(); id++)
			{
				auto& material = mesh.mesh->GetMaterials()[submeshs[id].MaterialIndex];
				m_MaterialUniform->SetData((void*) & material->GetMaterialUniforms(), sizeof(MaterialUniforms));

				if (shader == nullptr)
					material->GetShader()->Bind();
				else
					shader->Bind();

				material->GetDiffuseMap()->Bind(0);
				if (material->GetMaterialUniforms().useNormalMap == 1)
					material->GetNormalMap()->Bind(1);
				material->GetSpecMap()->Bind(2);
				material->GetRoughnessMap()->Bind(3);

				Renderer::RenderMesh(mesh.mesh, id);

				if (shader == nullptr)
					material->GetShader()->Unbind();
				else
					shader->Unbind();
			}
		}

		Renderer::EndScene();
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
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component)
	{
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

	template<>
	void Scene::OnComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component)
	{
	}
}
