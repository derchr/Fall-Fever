#pragma once

#include "definitions/enumHash.h"
#include "definitions/eventActions.h"

class GLFWwindow;

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

    static CameraActionMap s_cameraActionMap;
    static CameraMouseActionMap s_cameraMouseActionMap;
    static WindowActionMap s_windowActionMap;
    static MouseButtonActionMap s_mouseButtonActionMap;

    static float s_mouseSensitivity;
    static bool s_firstMouseInput;

    GLFWwindow *m_window;
};
