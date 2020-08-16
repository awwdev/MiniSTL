//https://github.com/awwdev

#pragma once
#include "mini/Utils/Structs.hpp"
#include "mini/Utils/Algorithms.hpp"

namespace mini::utils {
    
constexpr utils::Vec4f GREEN = { 0, 1, 0, 1 };
constexpr utils::Vec4f BLUE  = { 0, 0, 1, 1 };
constexpr utils::Vec4f RED   = { 1, 0, 0, 1 };
constexpr utils::Vec4f RED2  = { 1, .3f, .3f, 1 };
constexpr utils::Vec4f WHITE = { 1, 1, 1, 1 };


const utils::Common_Vertex MESH_CUBE [] {
    { { -1, 1, 1 }, {}, RED, {} },
    { {  1, 1, 1 }, {}, RED, {} },
    { {  1, 1,-1 }, {}, RED, {} },
    { {  1, 1, 1 }, {}, RED, {} },
    { {  1,-1, 1 }, {}, RED, {} },
    { {  1,-1,-1 }, {}, RED, {} },
    { {  1, 1,-1 }, {}, RED, {} },
    { {  1,-1,-1 }, {}, RED, {} },
    { { -1,-1,-1 }, {}, RED, {} },
    { { -1,-1, 1 }, {}, RED, {} },
    { { -1, 1, 1 }, {}, RED, {} },
    { { -1, 1,-1 }, {}, RED, {} },
    { {  1,-1, 1 }, {}, RED, {} },
    { { -1,-1, 1 }, {}, RED, {} },
    { { -1,-1,-1 }, {}, RED, {} },
    { { -1, 1, 1 }, {}, RED, {} },
    { { -1,-1, 1 }, {}, RED, {} },
    { {  1,-1, 1 }, {}, RED, {} },
    { { -1, 1, 1 }, {}, RED, {} },
    { {  1,-1, 1 }, {}, RED, {} },
    { {  1, 1, 1 }, {}, RED, {} },
    { {  1,-1, 1 }, {}, RED, {} },
    { { -1,-1,-1 }, {}, RED, {} },
    { {  1,-1,-1 }, {}, RED, {} },
    { { -1,-1, 1 }, {}, RED, {} },
    { { -1, 1,-1 }, {}, RED, {} },
    { { -1,-1,-1 }, {}, RED, {} },
    { {  1, 1,-1 }, {}, RED, {} },
    { { -1,-1,-1 }, {}, RED, {} },
    { { -1, 1,-1 }, {}, RED, {} },
    { {  1, 1, 1 }, {}, RED, {} },
    { {  1,-1,-1 }, {}, RED, {} },
    { {  1, 1,-1 }, {}, RED, {} },
    { { -1, 1, 1 }, {}, RED, {} },
    { {  1, 1,-1 }, {}, RED, {} },
    { { -1, 1,-1 }, {}, RED, {} },
};

const utils::Common_Vertex MESH_QUAD [] {
    { { -1.0f, -1.0f, 0 }, {}, { GREEN }, {} },
    { {  1.0f, -1.0f, 0 }, {}, { GREEN }, {} },
    { {  1.0f,  1.0f, 0 }, {}, { GREEN }, {} },
    { { -1.0f, -1.0f, 0 }, {}, { RED }, {} },
    { {  1.0f,  1.0f, 0 }, {}, { RED }, {} },
    { { -1.0f,  1.0f, 0 }, {}, { RED }, {} },
};

const utils::Common_Vertex MESH_TRIANGLE [] {
    { {  0.0f, -1.0f, 0 }, {}, { .8f, .2f, .2f, 1 }, {} },
    { {  1.0f,  1.0f, 0 }, {}, { .8f, .2f, .2f, 1 }, {} },
    { { -1.0f,  1.0f, 0 }, {}, { .8f, .2f, .2f, 1 }, {} },
};

//TODO constexpr but we need sin/cos constexpr to
//TODO unlit flag via UBO
auto GetRingVertex(const u32 current, const u32 max, const f32 radius)
{
    const f32 norm = current / (f32)max;
    const f32 x = std::sinf(norm * 6.283f) * radius;
    const f32 z = std::cosf(norm * 6.283f) * radius;
    return utils::Common_Vertex {
        { x, 0, z }, { 0, 1, 0 }, { 1, 1, 1, 1 }, {}
    };
}

//ring sizes
constexpr f32 R1 = 1.0f;
constexpr f32 R2 = 0.99f;

#define RING_SEGMENT(n, C)      \
GetRingVertex(n, C, R1),        \
    GetRingVertex(n+1, C, R2),  \
    GetRingVertex(n+1, C, R1),  \
GetRingVertex(n, C, R1),        \
    GetRingVertex(n, C, R2),    \
    GetRingVertex(n+1, C, R2)
     

const utils::Common_Vertex MESH_RING_8 [] {
    RING_SEGMENT(0, 8),
    RING_SEGMENT(1, 8),
    RING_SEGMENT(2, 8),
    RING_SEGMENT(3, 8),
    RING_SEGMENT(4, 8),
    RING_SEGMENT(5, 8),
    RING_SEGMENT(6, 8),
    RING_SEGMENT(7, 8),
};

}//ns