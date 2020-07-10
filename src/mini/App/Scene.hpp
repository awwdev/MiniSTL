//https://github.com/awwdev

#pragma once

#include "mini/ECS/ECS.hpp"
#include "mini/ECS/Prefabs.hpp"
#include "mini/Utils/Types.hpp"
#include "mini/Debug/Logger.hpp"
#include "mini/Box/String.hpp"
#include "mini/RenderGraph/RenderGraph.hpp"
#include "mini/App/UI.hpp"

namespace mini::app
{
    struct Scene
    {
        ecs::ECS ecs {};
        rendergraph::RenderGraph renderGraph;

        Scene()
        {
            //TODO: move into some resource manager an load at loading scene
            ecs.prefabs.Parse("res/prefabs.txt"); 
            rendergraph::g_renderGraphPtr = &renderGraph;
        }

        void Update(const double dt)
        {
            //TEST: update ubo data (transform)
            //TODO: ECS provides render data ("render system")
            rendergraph::UniformData_Default cubes [] = {
                math::Identity4x4(),
                math::Identity4x4(),
            };
            //! ORDERING IS CURRENTLY IMPORTANT
            //TODO: SORTING; MESH ID -> ARRAY ID -> UBO VERTICAL ALIGN
            renderGraph.default_uboGroups.Clear();
            renderGraph.default_uboArray.Clear();

            renderGraph.default_uboGroups.Append(rendergraph::UniformGroup{ 
                .begin = renderGraph.default_uboArray.count, 
                .count = (u32)ARRAY_COUNT(cubes) 
            });
            renderGraph.default_uboArray.AppendArray(cubes);

            
            //? UI
            ui::DrawFPS(); 
            ui::DrawConsole();

        }

    };

}//ns