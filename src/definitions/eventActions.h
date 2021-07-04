#pragma once

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
