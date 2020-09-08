//https://github.com/awwdev

#include "Window/win_Window.hpp"
#include "Window/win_WindowProcedure.hpp"
#include "Debug/Logger.hpp"
#include "Debug/Console.hpp"
#include "Box/Array.hpp"
#include "Utils/Types.hpp"

#include "Memory/Allocator.hpp"
#include "Memory/AllocatorPrint.hpp"

#include "Vulkan/Rendering/Renderer.hpp"

#include "App/Scene.hpp"
#include "Utils/DeltaTime.hpp"
#include "Resources/HostResources.hpp"

using namespace rpg;

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE,
    _In_ PWSTR,
    _In_ int)
{
    dbg::Console console{};
    mem::GlobalAllocate();

    {
        wnd::Window  window { hInstance, 800, 600 };
        auto ptrHostResources = mem::ClaimBlock<res::HostResources>(); //will load immediately for now
        auto ptrRenderer = mem::ClaimBlock<vk::VkRenderer>(
            vk::WindowHandle{window.hInstance, window.hWnd},
            *ptrHostResources);
        auto ptrGameScenes = mem::ClaimBlock<app::GameScene>();
        ptrGameScenes->Create(*ptrHostResources);

        while (wnd::global::events[wnd::Window_Close] == wnd::None && wnd::global::events[wnd::ESC] == wnd::None)
        {
            wnd::UpdateEvents(window.hWnd);
            dt::UpdateFPS();

            if (wnd::global::window_h != 0 && wnd::global::window_w != 0)
            {
                ptrGameScenes->Update(rpg::dt::seconds, *ptrHostResources);
                ptrRenderer->Render(rpg::dt::seconds, *ptrGameScenes, *ptrHostResources);
            }
        }

        vk::VkCheck(vkDeviceWaitIdle(vk::g_contextPtr->device));
        mem::PrintAllocationHTML();
    }
    
    mem::GlobalDeallocate();
    system("pause");
    return 0;
} //main end


//TODO: rework vk image obj
//TODO: profiler
//TODO: normal per triangle on terrain instead of 
//TODO: IBO
//TODO: cast shadow or not (tree vs grass both texture)
//TODO: ECS serialization
//TODO: add static flag to render component, so ubo data is not updated
//TODO: join transform and render component
//TODO: draw stuff on render target for post fx
//TODO: use block ptr for s render function 