#pragma once
#include "mini/App/ECS.hpp"
#include "mini/Utils/Types.hpp"
#include "mini/Debug/Logger.hpp"
#include "mini/Box/String.hpp"
#include "mini/App/IRenderer.hpp"
#include "mini/App/UI.hpp"

using namespace mini;

namespace mini::app
{
    struct Scene
    {
        box::String<10> name { "Scene1" };
        ecs::ECS ecs;

        Scene()
        {
            
        }

        inline void Update(IRenderer& renderer, const double dt)
        {
            ui::FPS_Monitor(renderer, dt);

            if (ui::Button(renderer, "Add Entity", {100, 100, 120, 30}) == ui::ButtonState::Released) {
                const auto id = ecs.AddEntity();
                ecs.AddComponent<ecs::C_Transform>(id);
            }

            FOR_ARRAY(ecs.arrays.transforms.dense, i) {
                renderer.Add_DrawText("entity", 0, 30 + i*16, renderer.hostResources.fonts.default_font);
            }     
            
        }

    };

}//ns