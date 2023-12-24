#include "SceneHierarchyPanel.h"
#include "Hazel/Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Hazel
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		ImGui::SetWindowFontScale(1.5f);

		m_Context->m_Registry.each([this](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			});


		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = {};
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
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)123456, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columuWidth = 100.0f)
	{
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

			if (ImGui::Button(xyz[i], buttonSize))
				values[i] = resetValue;
		
			ImGui::SameLine();
			ImGui::DragFloat(xyz[i+3], &values[i], 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::PopStyleColor(3);
			if (i != 2 ) ImGui::SameLine();
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if (entity.HasComponent<TransformComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				auto& tc =  entity.GetComponent<TransformComponent>();
				
				DrawVec3Control("Translation", tc.Translation);
				glm::vec3 rotation = glm::degrees(tc.Rotation);
				DrawVec3Control("Rotation", rotation);
				tc.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", tc.Scale, 1.0f);
				
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<CameraComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				auto& cameraComponent =  entity.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;

				ImGui::Checkbox("Primary", &cameraComponent.Primary);
				
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

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
				}

				ImGui::TreePop();
			}

		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Render"))
			{
				auto& sc = entity.GetComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(sc.Color));

				ImGui::TreePop();
			}
		}
	}
}