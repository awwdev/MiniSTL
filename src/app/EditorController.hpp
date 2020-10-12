//https://github.com/awwdev

#pragma once
#include "ecs/ECS.hpp"
#include "com/Matrix.hpp"
#include "wnd/WindowEvents.hpp"
#include "gpu/Meta/Cameras.hpp"
#include "app/InputMode.hpp"
#include "gpu/RenderData/RenderData.hpp"
#include "res/Terrain/TerrainSerialization.hpp"

namespace rpg::app {

struct EditorController
{
    gpu::EgoCamera camera;

    EditorController()
    {
        camera.Update(0);
    }

    void Update(
    const double dt, 
    ecs::ECS& ecs, 
    res::Resources& res,
    gpu::RenderData& renderData)
    {
        if (app::glo::inputMode == app::glo::FlyMode)
            camera.Update(dt);
        if (wnd::glo::resizeState == wnd::glo::ResizeState::End)
            camera.UpdatePerspective();

        renderData.general.meta.view = camera.view;
        renderData.general.meta.proj = camera.perspective;
        renderData.general.meta.viewDir = com::Normalize(camera.rotation);

        //? serialization 

        if (wnd::HasEvent<wnd::EventType::F5, wnd::EventState::Pressed>())
        {
            ecs.Save();
            res::SaveTerrain(res.terrain.terrain.quadrants);
        }

        if (wnd::HasEvent<wnd::EventType::F6, wnd::EventState::Pressed>())
        {
            ecs.Load();
            res::LoadTerrain(res.terrain.terrain.quadrants);
            res.terrain.terrain.MarkAllDirty();
        }

    }
};

}//ns