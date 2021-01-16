#pragma once

enum cameraActions {
    cameraUp,
    cameraDown,
    cameraForward,
    cameraBackward,
    cameraLeft,
    cameraRight,
    CAMERA_ACTION_NUM_ITEMS
};

enum cameraMouseActions {
    cameraMouseDeltaX,
    cameraMouseDeltaY,
    CAMERA_MOUSE_ACTION_NUM_ITEMS
};

enum windowActions {
    wireFrameToggle,
    mouseCatchToggle,
    WINDOW_ACTION_NUM_ITEMS
};

enum mouseButtonActions {
    leftClicked,
    rightClicked,
    middleClicked,
    MOUSE_BUTTON_ACTION_NUM_ITEMS
};
