#pragma once
enum ConsoleKeyType{
    None = 0,
    ASCII = 1,
    Control = 2,
    ColorChange = 3,
    Cursor = 4,
    Exit = 5
};
enum ConsoleKeyAction{
    KeyDown = 0,
    KeyUp = 1
};

struct ConsoleKeyPress{
    ConsoleKeyType type;
    ConsoleKeyAction action;
    bool isAltPressed = false;
    bool isCtrlPressed = false;
    bool isShiftPressed = false;
    bool isFunctionKey = false;
    bool isCaps = false;
    uint16_t keyCode;
    ConsoleKeyPress(){
        type = None;
        action = KeyDown;
        keyCode = 0;
    }
    ConsoleKeyPress(ConsoleKeyType type, uint16_t keyCode){
        this->type = type;
        this->keyCode = keyCode;
    }
};
