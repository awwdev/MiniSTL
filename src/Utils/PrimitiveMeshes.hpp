//https://github.com/awwdev

#pragma once
#include "Utils/Structs.hpp"
#include "Utils/Algorithms.hpp"

namespace rpg::use {
    
constexpr use::Vec4f GREEN = { 0, 1, 0, 1 };
constexpr use::Vec4f BLUE  = { 0, 0, 1, 1 };
constexpr use::Vec4f RED   = { 1, 0, 0, 1 };
constexpr use::Vec4f RED2  = { 1, .3f, .3f, 1 };
constexpr use::Vec4f WHITE = { 1, 1, 1, 1 };


const use::Common_Vertex MESH_CUBE [] {
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

const use::Common_Vertex MESH_QUAD [] {
    { { -1.0f, -1.0f, 0 }, {}, { GREEN }, {} },
    { {  1.0f, -1.0f, 0 }, {}, { GREEN }, {} },
    { {  1.0f,  1.0f, 0 }, {}, { GREEN }, {} },
    { { -1.0f, -1.0f, 0 }, {}, { RED }, {} },
    { {  1.0f,  1.0f, 0 }, {}, { RED }, {} },
    { { -1.0f,  1.0f, 0 }, {}, { RED }, {} },
};

const use::Common_Vertex MESH_TRIANGLE [] {
    { {  0.0f, -1.0f, 0 }, {}, { .8f, .2f, .2f, 1 }, {} },
    { {  1.0f,  1.0f, 0 }, {}, { .8f, .2f, .2f, 1 }, {} },
    { { -1.0f,  1.0f, 0 }, {}, { .8f, .2f, .2f, 1 }, {} },
};

//TODO constexpr but we need sin/cos constexpr to
//TODO unlit flag via UBO
auto GetRingVertex(const u32 current, const u32 max, const f32 radius)
{
    const f32 norm = current / (f32)max;
    const f32 x = sinf(norm * 6.283f) * radius;
    const f32 z = cosf(norm * 6.283f) * radius;
    return use::Common_Vertex {
        { x, 0, z }, { 0, 1, 0 }, { 1, 1, 1, 1 }, {}
    };
}

//ring sizes
constexpr f32 R1 = 1.0f;
constexpr f32 R2 = 0.95f;

#define RING_SEGMENT(n, C)      \
GetRingVertex(n, C, R1),        \
    GetRingVertex(n+1, C, R2),  \
    GetRingVertex(n+1, C, R1),  \
GetRingVertex(n, C, R1),        \
    GetRingVertex(n, C, R2),    \
    GetRingVertex(n+1, C, R2)
     

const use::Common_Vertex MESH_RING_16 [] {
    RING_SEGMENT( 0, 16),
    RING_SEGMENT( 1, 16),
    RING_SEGMENT( 2, 16),
    RING_SEGMENT( 3, 16),
    RING_SEGMENT( 4, 16),
    RING_SEGMENT( 5, 16),
    RING_SEGMENT( 6, 16),
    RING_SEGMENT( 7, 16),
    RING_SEGMENT( 8, 16),
    RING_SEGMENT( 9, 16),
    RING_SEGMENT(10, 16),
    RING_SEGMENT(11, 16),
    RING_SEGMENT(12, 16),
    RING_SEGMENT(13, 16),
    RING_SEGMENT(14, 16),
    RING_SEGMENT(15, 16),
};

}//ns