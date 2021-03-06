//https://github.com/awwdev

#pragma once
#include "com/Types.hpp"
#include "com/Matrix.hpp"
#include "gpu/RenderData/RenderData_Main.hpp"

namespace rpg::res {

static constexpr uint32_t MESHES_TOTAL = 100;
static constexpr uint32_t MESHES_VERTICES_TOTAL = 1'000'000;
static constexpr uint32_t MESHES_INDICES_TOTAL  = 1'000'000;

struct MeshVertexRange 
{ 
    uint32_t index;
    uint32_t count; 
};

struct MeshIndexRange
{ 
    uint32_t index;
    uint32_t count; 
};

using MeshVertex = gpu::RenderData_Main::Vertex;
using MeshIndex  = uint32_t;

using VertexArray  = com::Array<MeshVertex, MESHES_VERTICES_TOTAL>;
using IndexArray   = com::Array<MeshIndex, MESHES_INDICES_TOTAL>;
using VertexRanges = MeshVertexRange [MESHES_TOTAL];
using IndexRanges  = MeshIndexRange  [MESHES_TOTAL];

}//ns