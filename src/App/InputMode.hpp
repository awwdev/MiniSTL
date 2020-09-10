//https://github.com/awwdev

#pragma once
#include "Window/WindowEvents.hpp"

namespace rpg::app {
    
namespace global  
{
    enum InputMode { PlayMode, Edit_Mode, UI_Mode, FlyMode } inputMode = Edit_Mode;
    InputMode prev = Edit_Mode;
}

inline void ResetUpdateInputMode()
{
    //reset and set on UI update
    if (global::inputMode == global::UI_Mode)
        global::inputMode = global::Edit_Mode;

    if (wnd::HasEvent<wnd::EventType::F1, wnd::EventState::Pressed>()){
        if (global::inputMode == global::PlayMode)
            global::prev = global::inputMode = global::Edit_Mode;
        else    
            global::prev = global::inputMode = global::PlayMode;
    }

    if (wnd::HasEvent<wnd::EventType::Mouse_ButtonRight, wnd::EventState::PressedOrHeld>()) 
        global::inputMode = global::FlyMode;
    if (wnd::HasEvent<wnd::EventType::Mouse_ButtonRight, wnd::EventState::Released>()) 
        global::inputMode = global::prev;
}

}//ns