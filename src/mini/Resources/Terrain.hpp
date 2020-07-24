//https://github.com/awwdev

#pragma once
#include "mini/Utils/PrimitiveMeshes.hpp"
#include "mini/Debug/Assert.hpp"
#include "mini/Resources/Mesh.hpp"
#include "mini/Utils/Structs.hpp"
#include "mini/ECS/ECS.hpp"
#include "mini/Rendering/Camera.hpp"
#include "mini/Window/AppEvents.hpp"
#include "mini/Box/Array.hpp"

//TODO: move vertical
//TODO: AABB to check which quadrant
//TODO: exapandable
//TODO: serialize

namespace mini::res
{
    //! mind the terrain vertex max count!
    struct Quadrant
    {
        static constexpr auto QUAD_COUNT = 4;
        static constexpr auto QUAD_COUNT_TOTAL = QUAD_COUNT * QUAD_COUNT;
        static constexpr auto VERT_COUNT_TOTAL = QUAD_COUNT_TOTAL * 6;
        static constexpr auto SIZE  = 20;

        const f32 quadrantX;
        const f32 quadrantY;
        utils::Common_Vertex verts [VERT_COUNT_TOTAL];

        box::Array<u32, 6> qCorners [QUAD_COUNT+1][QUAD_COUNT+1];
        u32 GetCornerByVertex(const u32 vIdx)
        {
            const auto normIdx = (u32)(vIdx % 6);
            const auto quadIdx = (u32)(vIdx / 6);
            const auto quadRow = (u32)(quadIdx / QUAD_COUNT);

            if (normIdx == 0 || normIdx == 3)   return quadIdx + quadRow + 0;
            if (normIdx == 1)                   return quadIdx + quadRow + 1;
            if (normIdx == 2 || normIdx == 4)   return quadIdx + quadRow + 1 + QUAD_COUNT + 1;
            if (normIdx == 5)                   return quadIdx + quadRow + 0 + QUAD_COUNT + 1;
        }

        void Create(const math::Vec4f& col = { 0.1f, 0.7f, 0.1f, 1 })
        {
            const auto quadSize = SIZE  / QUAD_COUNT;

            for(u8 z = 0; z < QUAD_COUNT; ++z) {
            for(u8 x = 0; x < QUAD_COUNT; ++x) { 
                const auto idx = (z * QUAD_COUNT + x) * 6;
                verts[idx + 0] = { {  quadrantX + 0.0f * quadSize + x * quadSize, 0,  quadrantY + 0.0f * quadSize + z * quadSize, 1 }, {}, col, {} };
                verts[idx + 1] = { {  quadrantX + 1.0f * quadSize + x * quadSize, 0,  quadrantY + 0.0f * quadSize + z * quadSize, 1 }, {}, col, {} };
                verts[idx + 2] = { {  quadrantX + 1.0f * quadSize + x * quadSize, 0,  quadrantY + 1.0f * quadSize + z * quadSize, 1 }, {}, col, {} };

                verts[idx + 3] = { {  quadrantX + 0.0f * quadSize + x * quadSize, 0,  quadrantY + 0.0f * quadSize + z * quadSize, 1 }, {}, col, {} };
                verts[idx + 4] = { {  quadrantX + 1.0f * quadSize + x * quadSize, 0,  quadrantY + 1.0f * quadSize + z * quadSize, 1 }, {}, col, {} };
                verts[idx + 5] = { {  quadrantX + 0.0f * quadSize + x * quadSize, 0,  quadrantY + 1.0f * quadSize + z * quadSize, 1 }, {}, col, {} };
            }}

            const auto CORNER_COUNT = QUAD_COUNT + 1;
            for(u8 z = 0; z < CORNER_COUNT; ++z) {
            for(u8 x = 0; x < CORNER_COUNT; ++x) { 

                if (x < CORNER_COUNT && z < CORNER_COUNT){
                    qCorners[z][x].Append(x*6 + z*(CORNER_COUNT*6));
                }

            }}
        }
    };

    struct Terrain
    {
        Quadrant quadrants [4] = {
            { 0,              0 },
            { Quadrant::SIZE, 0 },
            { 0,              Quadrant::SIZE },
            { Quadrant::SIZE, Quadrant::SIZE },
        };

        ecs::ID gizmoID = 0;
        const float S = .05f; //gizmo cube scale

        f32 yDragPoint = 0;
        s32 draggedVertex = -1;

        void Create(ecs::ECS& ecs)
        {
            quadrants[0].Create({ 0.1f, 0.7f, 0.1f, 1 });
            quadrants[1].Create({ 0.7f, 0.1f, 0.1f, 1 });
            quadrants[2].Create({ 0.1f, 0.1f, 0.7f, 1 });
            quadrants[3].Create({ 0.7f, 0.7f, 0.7f, 1 });

            //GIZMO CUBE
            {
                constexpr float X =  0; 
                constexpr float Y =  0;
                constexpr float Z =  0;
                gizmoID = ecs.AddEntity();
                const math::Mat4f pos = math::Mat4f{
                    S, 0, 0, 0,
                    0, S, 0, 0,
                    0, 0, S, 0,
                    X, Y, Z, 1,
                };
                ecs.arrays.AddComponent<ecs::ComponentType::Transform> (gizmoID, pos);
                ecs.arrays.AddComponent<ecs::ComponentType::RenderData>(gizmoID, res::MeshType::PrimitiveCube);
            }
        }

        void Update(const double dt, const rendering::Camera& camera, ecs::ECS& ecs)
        {
            auto& quadrant = quadrants[0];

            //INTERSECTION
            using namespace utils;
            const auto ray = camera.ScreenRay();
            for(auto i = 0; i < quadrant.VERT_COUNT_TOTAL; i+=3)
            {
                auto& v0 = quadrant.verts[i+0].pos;
                auto& v1 = quadrant.verts[i+1].pos;
                auto& v2 = quadrant.verts[i+2].pos;
                const auto intersection = utils::RayTriangleIntersection(
                    camera.pos,
                    ray,
                    { v0[Vx], v0[Vy], v0[Vz] },
                    { v1[Vx], v1[Vy], v1[Vz] },
                    { v2[Vx], v2[Vy], v2[Vz] }
                );

                if (intersection)
                {
                    auto X = intersection->pos[Vx];
                    auto Y = intersection->pos[Vy];
                    auto Z = intersection->pos[Vz];

                    //TODO: could be written better:
                    enum CloseVertex { V0, V1, V2 } closeVertex = V0;
                    if      (intersection->u > 0.5) closeVertex = V1;
                    else if (intersection->v > 0.5) closeVertex = V2;
                    else if (intersection->u > 0.25 && intersection->v > 0.25)
                    {
                        if  (intersection->u > intersection->v) closeVertex = V1;
                        else closeVertex = V2;
                    }

                    if (closeVertex == V0)
                    {
                        X = v0[Vx];
                        Y = v0[Vy];
                        Z = v0[Vz];
                    }
                    if (closeVertex == V1)
                    {
                        X = v1[Vx];
                        Y = v1[Vy];
                        Z = v1[Vz];
                    }
                    if (closeVertex == V2)
                    {
                        X = v2[Vx];
                        Y = v2[Vy];
                        Z = v2[Vz];
                    }

                    auto& cubeTrans = ecs.arrays.transforms.Get(gizmoID);
                    cubeTrans.transform = {
                        S, 0, 0, 0,
                        0, S, 0, 0,
                        0, 0, S, 0,
                        X, Y, Z, 1,
                    };
                   
                    if (wnd::CheckEvent(wnd::EventType::Mouse_Left, wnd::EventState::Pressed))
                    {
                        yDragPoint = (f32)wnd::mouse_client_y;
                        if (closeVertex == V0) draggedVertex = i+0;
                        if (closeVertex == V1) draggedVertex = i+1;
                        if (closeVertex == V2) draggedVertex = i+2;

                        LOG(
                            quadrant.GetCornerByVertex(draggedVertex)//;
                        );
                    }
                    if (wnd::CheckEvent(wnd::EventType::Mouse_Left, wnd::EventState::Released))
                    {
                        draggedVertex = -1;
                    }

                    if (draggedVertex)
                    {
                        const auto yDragDelta = (f32)wnd::mouse_client_y - yDragPoint;
                        constexpr f32 dragScale = 0.01f;
                        auto& v = quadrant.verts[draggedVertex].pos;
                        v[Vy] += yDragDelta * dragScale;
                        yDragPoint = (f32)wnd::mouse_client_y;
                    }
                    
                    break;
                }

            }//for end
        }

    };
    
}//ns