#include "Scene.h"
#include <iostream>
static void hallo()
{
    std::cout << "Testobj created!\n";
}

Scene::Scene()
{
    struct TestComponent
    {
        int x;
    };

    auto entity = m_registry.create();

    auto sink = m_registry.on_construct<TestComponent>().before();

    m_registry.emplace<TestComponent>(entity, TestComponent{.x = 2});
}