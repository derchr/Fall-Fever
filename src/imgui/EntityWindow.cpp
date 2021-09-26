#include "EntityWindow.h"
#include "../Entity.h"

#include <imgui.h>

Imgui::EntityWindow::EntityWindow(const std::vector<ModelEntity *> entities) : Window("Entities"), m_entites(entities)
{}

void Imgui::EntityWindow::addWidgets()
{
    ImGui::Text("Treelist");

    for (const auto &entity : m_entites) {
        addChildWidget(*entity);
    }
}

void Imgui::EntityWindow::addChildWidget(const ModelEntity &entity)
{
    if (entity.getChildren().empty()) {
        ImGui::Indent();
        ImGui::Text(entity.getUniqueName().c_str());
        ImGui::SameLine();
        ImGui::SmallButton("Edit");
        ImGui::Unindent();
    } else {
        bool expanded = ImGui::TreeNode(entity.getUniqueName().c_str());
        ImGui::SameLine();
        ImGui::SmallButton("Edit");

        if (expanded) {
            for (const auto &child : entity.getChildren()) {
                addChildWidget(*(const ModelEntity *)child);
            }
            ImGui::TreePop();
        }
    }
}