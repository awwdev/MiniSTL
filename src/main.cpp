#include "MiniSTL/Window/win_Window.hpp"
#include "MiniSTL/Window/win_WindowCallback.hpp"
#include "MiniSTL/Debug/Logger.hpp"
#include "MiniSTL/Debug/Console.hpp"
#include "MiniSTL/Debug/Profiler.hpp"
#include "MiniSTL/Box/String.hpp"
#include "MiniSTL/Box/Array.hpp"
#include "MiniSTL/Memory/Allocator.hpp"

using namespace mini;

int WINAPI wWinMain(
    _In_        HINSTANCE hInstance,
    _In_opt_    HINSTANCE hPrevInstance,
    _In_        PWSTR pCmdLine,
    _In_        int nCmdShow)
{
    const auto con = dbg::CreateConsole();
    const auto wnd = wnd::mini_CreateWindow(hInstance, 800, 600);
    
    mem::Allocate();
    auto o1 = mem::ClaimBlock<box::String<100>>("Hello ");
    auto o2 = mem::ClaimBlock<box::Array<float, 100>>(1, 2, 3);

    o1->Append("World!");
    o2->Append(42.f);

    LOG(*o2, *o2);


    while (!app::CheckEvent(EventType::Window_Close) && !app::IsPressed(EventType::Keyboard_Escape))
    {
        wnd::PollEvents();
        
    
        if (app::CheckEvent(EventType::Keyboard_W, EventState::Released)) {
            mini::dbg::dlog("released w");
        }

        //update current scene
        //draw current scene
    }
}

//variant
//print input?
//scene queue