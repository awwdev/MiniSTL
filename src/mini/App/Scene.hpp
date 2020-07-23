//https://github.com/awwdev

#pragma once

#include "mini/ECS/ECS.hpp"
#include "mini/ECS/Prefabs.hpp"
#include "mini/ECS/Systems/S_Render.hpp"

#include "mini/Utils/Types.hpp"
#include "mini/Debug/Logger.hpp"
#include "mini/Box/String.hpp"
#include "mini/Rendering/RenderGraph.hpp"
#include "mini/Rendering/Camera.hpp"
#include "mini/Rendering/UI.hpp"
#include "mini/Resources/Terrain.hpp"

namespace mini::app
{
    struct Scene
    {
        //TODO: not every Scene will have those members
        ecs::ECS ecs {};
        rendering::RenderGraph renderGraph;

        ecs::ID trisID = 0;

        Scene()
        {
            //TODO: move into some resource manager an load at loading scene
            //ecs.prefabs.Parse("res/prefabs.txt"); 

            {
                constexpr float S = 1; //sword is one 1x1 blender cube
                constexpr float X = 0; 
                constexpr float Y = 0;
                constexpr float Z =-4;
                trisID = ecs.AddEntity();
                const math::Mat4f pos = math::Mat4f{
                    S, 0, 0, 0,
                    0, S, 0, 0,
                    0, 0, S, 0,
                    X, Y, Z, 1,
                };
                ecs.arrays.AddComponent<ecs::ComponentType::Transform> (trisID, pos);
                ecs.arrays.AddComponent<ecs::ComponentType::RenderData>(trisID, res::MeshType::PrimitiveTriangle);
            }

            
            /*{
                constexpr float S =  0.1f; //ths cube is -1 to 1 and half matches sword
                constexpr float X = -1; 
                constexpr float Y =  0;
                constexpr float Z =  0;
                const auto id = ecs.AddEntity();
                const math::Mat4f pos = math::Mat4f{
                    S, 0, 0, 0,
                    0, S, 0, 0,
                    0, 0, S, 0,
                    X, Y, Z, 1,
                };
                ecs.arrays.AddComponent<ecs::ComponentType::Transform> (id, pos);
                ecs.arrays.AddComponent<ecs::ComponentType::RenderData>(id, res::MeshType::PrimitiveCube);
            }*/
            
            
        }

        void Update(const double dt)
        {
            renderGraph.camera.Update(dt);

            //INTERSECTION TEST
            //TODO: move camera outside of rendergraph (same with terrain) later on ..
            using namespace utils;
            const auto& trisTransform = ecs.arrays.transforms.Get(trisID).transform;
            const auto v0  = MESH_TRIANGLE[0].pos * trisTransform;
            const auto v1  = MESH_TRIANGLE[1].pos * trisTransform;
            const auto v2  = MESH_TRIANGLE[2].pos * trisTransform;

            const auto ray = renderGraph.camera.ScreenRay();
            const auto intersection = utils::RayTriangleIntersection(
                renderGraph.camera.pos,
                ray,
                { v0[Vx], v0[Vy], v0[Vz] },
                { v1[Vx], v1[Vy], v1[Vz] },
                { v2[Vx], v2[Vy], v2[Vz] }
            );
            if (intersection) LOG(intersection.t);

            //? ECS
            ecs::S_Render(ecs.arrays, dt, renderGraph);

            //? UI
            ui::DrawFPS(renderGraph); 
            ui::DrawConsole(renderGraph);
            ui::DrawRenderStats(renderGraph);
            ui::DrawCameraPos(renderGraph, renderGraph.camera);

        }

    };

}//ns