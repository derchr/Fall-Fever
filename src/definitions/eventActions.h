#pragma once

#include "enumHash.h"
#include <unordered_map>

enum class CameraAction
{
    Up,
    Down,
    Forward,
    Backward,
    Left,
    Right,
    CAMERA_ACTION_NUM_ITEMS
};

enum class CameraMouseAction
{
    DeltaX,
    DeltaY,
    CAMERA_MOUSE_ACTION_NUM_ITEMS
};

enum class WindowAction
{
    WireFrameToggle,
    MouseCatchToggle,
    WindowShouldClose,
    WINDOW_ACTION_NUM_ITEMS
};

enum class MouseButtonAction
{
    LeftClicked,
    RightClicked,
    MiddleClicked,
    MOUSE_BUTTON_ACTION_NUM_ITEMS
};

typedef std::unordered_map<CameraAction, bool, EnumClassHash> CameraActionMap;
typedef std::unordered_map<CameraMouseAction, double, EnumClassHash> CameraMouseActionMap;
typedef std::unordered_map<WindowAction, bool, EnumClassHash> WindowActionMap;
typedef std::unordered_map<MouseButtonAction, bool, EnumClassHash> MouseButtonActionMap;
