#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>

#include "definitions/enumHash.h"
#include "definitions/eventActions.h"

typedef std::unordered_map<CameraAction, bool, EnumClassHash> CameraActionMap;
typedef std::unordered_map<CameraMouseAction, double, EnumClassHash> CameraMouseActionMap;
typedef std::unordered_map<WindowAction, bool, EnumClassHash> WindowActionMap;
typedef std::unordered_map<MouseButtonAction, bool, EnumClassHash> MouseButtonActionMap;

class EventHandler
{
public:
    EventHandler(GLFWwindow *p_window);
    ~EventHandler() = default;

    void handleEvents();

    WindowActionMap &getWindowActionMap() const;
    const CameraActionMap &getCameraActionMap() const;
    const MouseButtonActionMap &getMouseButtonActionMap() const;
    const CameraMouseActionMap &getCameraMouseActionMap() const;
    void setFirstMouseInput(bool val);

private:
    void clearActionRegisters();

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos);

private:
    static CameraActionMap cameraActionMap;
    static CameraMouseActionMap cameraMouseActionMap;
    static WindowActionMap windowActionMap;
    static MouseButtonActionMap mouseButtonActionMap;

    GLFWwindow *window;

    static float mouseSensitivity;
    static bool firstMouseInput;
};
