//https://github.com/awwdev

#include "Window/win_Window.hpp"
#include "Window/win_WindowProcedure.hpp"
#include "Debug/Logger.hpp"
#include "Debug/Console.hpp"
#include "Common/Container/Array.hpp"
#include "Common/Types.hpp"

#include "Common/Memory/Allocator.hpp"
#include "Common/Memory/AllocatorPrint.hpp"

#include "Vulkan/Rendering/Renderer.hpp"

#include "App/Scene.hpp"
#include "Common/DeltaTime.hpp"
#include "Resources/HostResources.hpp"

using namespace rpg;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int)
{
    dbg::Console console{};
    com::mem::GlobalAllocate();

    {
        wnd::Window  window { hInstance, 800, 600 };
        auto ptrHostResources  = com::mem::ClaimBlock<res::HostResources>();
        auto ptrRenderer       = com::mem::ClaimBlock<vk::VkRenderer>(vk::WindowHandle{window.hInstance, window.hWnd}, *ptrHostResources);
        auto ptrGameScenes     = com::mem::ClaimBlock<app::GameScene>();
        ptrGameScenes->Create(*ptrHostResources);

        while (wnd::glo::events[(idx_t)wnd::EventType::Window_Close] == wnd::EventState::None && 
               wnd::glo::events[(idx_t)wnd::EventType::ESC] == wnd::EventState::None)
        {
            wnd::UpdateEvents(window.hWnd);
            dt::UpdateFPS();

            if (wnd::glo::window_h != 0 && wnd::glo::window_w != 0)
            {
                ptrGameScenes->Update(rpg::dt::seconds, *ptrHostResources);
                ptrRenderer->Render(rpg::dt::seconds, *ptrGameScenes, *ptrHostResources);
            }
        }

        vk::VkCheck(vkDeviceWaitIdle(vk::g_contextPtr->device));
        com::mem::PrintAllocationHTML();
    }
    
    com::mem::GlobalDeallocate();
    system("pause");

    return 0;
}


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