#include "SceneHierarchyPanel.h"
#include "Calibur/Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <cstring>

/* The Microsoft C++ compiler is non-compliant with the C++ standard and needs
 * the following definition to disable a security warning on std::strncpy().
 */
#ifdef _MSVC_LANG
  #define _CRT_SECURE_NO_WARNINGS
#endif

namespace Calibur
{
	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;

	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		ImGui::SetWindowFontScale(1.5f);

		m_Context->m_Registry.each([this](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };

				if (!entity.HasParent()) // start form parent entity
					DrawEntityNode(entity);
			});
	
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = {};
		}

		// Right clink in blank space 
		if (ImGui::BeginPopupContextWindow(0, 1 |  ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("Game object");

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");

		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
			
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}
		
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			auto& children = entity.Children();
			for (auto& child : children)
			{
				DrawEntityNode(m_Context->m_EntityMap[child]);
			}

			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity) m_SelectionContext = {};
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columuWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFons = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columuWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		const char* xyz[] = { "X", "Y", "Z", "##X", "##Y", "##Z" };
		const ImVec4 button[3] = { ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f },ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f },ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f } };
		const ImVec4 buttonHovered[3] = { ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f },ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f },ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f } };
		const ImVec4 buttonActive[3] = { ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f },ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f },ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f } };

		for (int i = 0; i < 3; i++)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, button[i]);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHovered[i]);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActive[i]);
			
			ImGui::PushFont(boldFons);
			if (ImGui::Button(xyz[i], buttonSize))
				values[i] = resetValue;
			ImGui::PopFont();
			ImGui::PopStyleColor(3);
		
			ImGui::SameLine();
			ImGui::DragFloat(xyz[i+3], &values[i], 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			if (i != 2 ) ImGui::SameLine();
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		if (entity.HasComponent<T>())
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight}))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;
				ImGui::EndPopup();
			}
			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				if (!m_SelectionContext.HasComponent<CameraComponent>())
					m_SelectionContext.AddComponent<CameraComponent>();
				else
					HZ_CORE_WARN("This entity already has the Camera Component!");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer"))
			{
				if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
				else
					HZ_CORE_WARN("This entity already has the Sprite Renderer Component!");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Directional Light"))
			{
				if (!m_SelectionContext.HasComponent<DirectionalLightComponent>())
					m_SelectionContext.AddComponent<DirectionalLightComponent>();
				else
					HZ_CORE_WARN("This entity already has the Driectional Light Component!");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Point Light"))
			{
				if (!m_SelectionContext.HasComponent<PointLightComponent>())
					m_SelectionContext.AddComponent<PointLightComponent>();
				else
					HZ_CORE_WARN("This entity already has the Point Light Component!");
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);
				
				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeStrings = projectionTypeStrings[(int)camera.GetProjectionType()];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeStrings)) 
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeStrings == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeStrings = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo(); 
				}
				
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float verticalFov = glm::degrees(camera.GetPerspectiveFOV());
					if (ImGui::DragFloat("Vertical FOV", &verticalFov))
					{
						camera.SetPerspectiveFOV(glm::radians(verticalFov));
					}

					float nearClip = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip))
					{
						camera.SetPerspectiveNearClip(nearClip);
					}

					float FarClip = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far Clip", &FarClip))
					{
						camera.SetPerspectiveFarClip(FarClip);
					}

				}
				else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographics)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
					{
						camera.SetOrthographicSize(orthoSize);
					}

					float nearClip = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip))
					{
						camera.SetOrthographicNearClip(nearClip);
					}

					float FarClip = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far Clip", &FarClip))
					{
						camera.SetOrthographicFarClip(FarClip);
					}

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Render", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::Button("Texture");
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
						TextureSpecification spec;
						Ref<Texture2D> texture = Texture2D::Create(spec, texturePath.string());
						if (texture->IsLoaded())
							component.Texture = texture;
						else
							HZ_WARN("Could not load texture {0}", texturePath.filename().string());
					}
					ImGui::EndDragDropTarget(); 
				}
				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 10.0f);
			});
		
		DrawComponent<MeshComponent>("Mesh", entity, [](auto& component)
			{
				ImGui::Text("Mesh Name: %s", component.mesh->GetFilePath().c_str());

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::string filePath = (std::filesystem::path(g_AssetPath) / path).string();
						Ref<Mesh> newMesh = CreateRef<Mesh>(filePath, component.mesh->GetShaderName(), false);
						if (newMesh->IsLoaded())
							component.mesh = newMesh;
					}

					ImGui::EndDragDropTarget(); 
				}

				const ImGuiTreeNodeFlags treeNodeFlags =  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

				for (auto& submeshID : component.SubmeshIndices)
				{
					ImGui::PushID(submeshID);

					auto &submesh = component.mesh->GetSubMeshes()[submeshID];

					std::string meshName = submesh.MeshName;

					bool open = ImGui::TreeNodeEx((void*)(&meshName), treeNodeFlags, meshName.c_str());
					
					if (open)
					{
						int materialIndex = submesh.MaterialIndex;
						Ref<Material>& material = component.mesh->GetMaterials()[materialIndex];

						ImGui::DragFloat3("Albedo", (float*)& material->GetMaterialUniforms().Albedo, 0.01, 0.01, 1.0);
						ImGui::DragFloat("Roughness", &material->GetMaterialUniforms().Roughness, 0.001f, 0.0f, 1.0f);
						ImGui::DragFloat("Metallic", &material->GetMaterialUniforms().Metallic, 0.001f, 0.0f, 1.0f);
						ImGui::TreePop();
					}

					ImGui::PopID();
				}

			});

		DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component)
			{
				ImGui::ColorEdit3("Radiance", glm::value_ptr(component.Radiance));
				ImGui::DragFloat("Intensity", &component.Intensity, 0.2f, 0.001f, 1000.f);
			});

		DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component)
			{
				ImGui::ColorEdit3("Radiance", glm::value_ptr(component.Radiance));
				ImGui::DragFloat("Intensity", &component.Intensity, 0.2f, 0.001f, 1000.f);
				ImGui::DragFloat("Radius", &component.Radius, 0.2f, 0.001f, 1000.f);
				ImGui::DragFloat("Soruce Size", &component.SourceSize, 0.2f, 0.001f, 1000.f);
				ImGui::Checkbox("Cast Shadow", &component.CastShadow);
			});
	}
}
