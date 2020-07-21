#pragma once
#include "mini/Window/AppEvents.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#undef max

namespace mini::wnd
{
    inline void PollEvents()
    {
        //mouse_dx = mouse_dy = 0;
        events.Clear();
        for (MSG msg; PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);) { //probably define a limit per frame
            TranslateMessage(&msg); //virtual to char?
            DispatchMessage(&msg);
        }

        //outside the window mouse movement
        POINT point;
        GetCursorPos(&point);
        mouse_dx = point.x - mouse_screen_x;
        mouse_dy = point.y - mouse_screen_y;
        mouse_screen_x = point.x;
        mouse_screen_y = point.y;
    }
    
    LRESULT __stdcall WndProc1(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg) {

        #define PRESSED(t, ...)  events.Append(t, EventState::Pressed, __VA_ARGS__);  pressed.Set<t, true>();
        #define RELEASED(t, ...) events.Append(t, EventState::Released, __VA_ARGS__); pressed.Set<t, false>();

        //?keyboard

        case WM_KEYDOWN:
            switch (wParam) 
            {
                case VK_ESCAPE: PRESSED(EventType::Keyboard_Escape); break;
            }
        break;
        case WM_CHAR: //!issue: single key only at a time
            PRESSED(EventType::Keyboard_ASCII);
            events.Last().ascii = (char)wParam;
            asciiPressed = (char)wParam;
        break;

        case WM_KEYUP:
            switch (wParam) 
            {
                case VK_ESCAPE: RELEASED(EventType::Keyboard_Escape); break;
                default:
                {
                    RELEASED(EventType::Keyboard_ASCII);
                    events.Last().ascii = (char)wParam;
                    asciiPressed = 0;
                }
            }
        break;

        //?window

        case WM_CLOSE: events.Append(EventType::Window_Close); break;

        case WM_SIZE: 
        {
            switch (wParam) 
            {
                case SIZE_MAXIMIZED: 
                events.Append(EventType::Window_Resize);
                break;

                case SIZE_MINIMIZED: 
                events.Append(EventType::Window_Resize); 
                break;

                case SIZE_RESTORED: //this means spamming
                if (events.Contains(EventType::Window_Resize) == nullptr)
                    events.Append(EventType::Window_Resize); 
                break;
            }

            window_w = LOWORD(lParam);
            window_h = HIWORD(lParam);
        }
        break;
        
        case WM_SETCURSOR: //prevent cursor redraw
        {
            const auto hit = LOWORD(lParam);
            switch(hit)
            {
                //TODO: more clean way like refresh previous declared cursor?
                case HTCLIENT:  SetCursor(LoadCursor(NULL, IDC_ARROW)); break;
                default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
        }
        break;

        case WM_LBUTTONDOWN: PRESSED (EventType::Mouse_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;
        case WM_LBUTTONUP:   RELEASED(EventType::Mouse_Left, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); break;

        case WM_MOUSEMOVE:
        {
            mouse_client_x = GET_X_LPARAM(lParam);
            mouse_client_y = GET_Y_LPARAM(lParam);
        }
        break;

        case WM_INPUT:
        {
            //TODO: GetRawInputBuffer
            //TODO: multipe keys

            UINT dwSize = 0;
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)) == 0)
            {
                RAWINPUT raw {};
                GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));

                if (raw.header.dwType == RIM_TYPEMOUSE) 
                {
                    
                    //mouse_dx = (s32)raw.data.mouse.lLastX;
                    //mouse_dy = (s32)raw.data.mouse.lLastY;
                    //LOG(mouse_dx, mouse_dy);
                    //SetCursorPos(300 + window_w/2.f, 300 + window_h/2.f);
                }
            }

        }
        break;

        //?default

        default: return DefWindowProc(hWnd, uMsg, wParam, lParam);

        #undef PRESSED
        #undef RELEASED
        }

        return 0;    
    }

}//ns.