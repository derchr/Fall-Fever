#pragma once

#include "Window.h"

#include <vector>

class ModelEntity;

namespace Imgui {

class EntityWindow : public Window
{
public:
    EntityWindow(const std::vector<ModelEntity *> &entities);

private:
    void addWidgets() override;

    static void addChildWidget(const ModelEntity &entity);

    const std::vector<ModelEntity *> &m_entites;
};

} // namespace Imgui