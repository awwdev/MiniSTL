//https://github.com/awwdev

#pragma once
#include "res/Terrain/TerrainMesh.hpp"
#include "gpu/RenderData/RenderData_General.hpp"
#include "com/box/Optional.hpp"
#include "com/box/Array.hpp"
#include "com/utils/Utils.hpp"

namespace rpg::res2 {

template<auto QUAD_COUNT, auto QUAD_SIZE>
struct Quadrant
{   
    //? meta
    static constexpr float QUADRANT_SIZE = QUAD_SIZE * QUAD_COUNT;
    using Vertex = gpu::RenderData_General::Vertex;

    //? data
    TerrainMeshIndexed<Vertex, QUAD_COUNT> mesh;

    void Create(float const quadrantIdx_z, float const quadrantIdx_x, idx_t const quadrantIdx, 
        com::Vec4f const& color = { 0.1f, 0.7f, 0.1f, 1 })
    {
        float const z = quadrantIdx_z * QUADRANT_SIZE;
        float const x = quadrantIdx_x * QUADRANT_SIZE;
        idx_t const i = quadrantIdx * (QUAD_COUNT+1)*(QUAD_COUNT+1); //indices count
        mesh.Create(QUAD_SIZE, QUAD_SIZE, z, x, i, color);
    }

    struct RayQuadrant_Intersection
    {
        com::Vec3f point;
        uint32_t closestVertexIndex;
    };

    com::Optional<RayQuadrant_Intersection> RayIntersection(com::Ray const& ray) const
    {
        if (auto const aabbIntersection = RayAABB_Intersection(ray, mesh.aabb))
        {
            for(idx_t i = 0; i < mesh.INDEX_COUNT; i += 3) 
            {
                //using indices to get triangles
                auto const  i0 = mesh.GetRelativeVertexIndex(i+0);
                auto const  i1 = mesh.GetRelativeVertexIndex(i+1);
                auto const  i2 = mesh.GetRelativeVertexIndex(i+2);
                auto const& v0 = mesh.vertices[i0].pos;
                auto const& v1 = mesh.vertices[i1].pos;
                auto const& v2 = mesh.vertices[i2].pos;
                
                if (auto const triangleIntersection = RayTriangle_Intersection(ray, v0, v1, v2))
                {
                    auto const d0 = com::Distance(v0, triangleIntersection.point);
                    auto const d1 = com::Distance(v1, triangleIntersection.point);
                    auto const d2 = com::Distance(v2, triangleIntersection.point);
                    auto const closestVertexIndex = d2 < com::Min(d0, d1) ? i2 : (d0 < d1 ? i0 : i1);
                    return { triangleIntersection.point, closestVertexIndex + mesh.indicesOffset };
                } 
            }
        }
        return {};
    }

};

}//ns