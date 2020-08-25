//https://github.com/awwdev
#pragma once

#include "mini/Box/Array.hpp"
#include "mini/Box/EnumMap.hpp"
#include "mini/Debug/Logger.hpp"
#include "mini/Resources/TextureLoader.hpp"
#include "mini/Resources/TextureArray.hpp"
#include "mini/Resources/ModelLoader.hpp"
#include "mini/Resources/MeshTypes.hpp"
#include "mini/Resources/Terrain/Terrain.hpp"
#include "mini/Rendering/RenderGraph.hpp"
#include "mini/utils/PrimitiveMeshes.hpp"

namespace mini::res {
    
struct Textures
{
    TextureArray<128, 16, 16, 1> monospaceFont;
    TextureArray<1, 512, 512, 4>   default;

    void Load()
    {
        monospaceFont.LoadArray("res/TextureArrays/MonospaceFont"); //RLE would be nice
        default.LoadArray("res/TextureArrays/Default");
    }
};

struct Models
{
    struct MeshVertexView
    {
        const utils::Common_Vertex* begin;
        idx_t count;
    };

    box::EnumMap<res::MeshType::ENUM_END, MeshVertexView> vertexLookup
    {
        //hardcoded primitives
        { res::MeshType::PrimitiveCube,      { utils::MESH_CUBE,     ArrayCount(utils::MESH_CUBE)       } },
        { res::MeshType::PrimitiveQuad,      { utils::MESH_QUAD,     ArrayCount(utils::MESH_QUAD)       } },
        { res::MeshType::PrimitiveTriangle,  { utils::MESH_TRIANGLE, ArrayCount(utils::MESH_TRIANGLE)   } },
        { res::MeshType::PrimitiveRing16,    { utils::MESH_RING_16,  ArrayCount(utils::MESH_RING_16)     } },
    }; 

    box::Array<utils::Common_Vertex, rendering::DEFAULT_VERTEX_MAX_COUNT> allModelVertices;

    void Load()
    {
        box::Array<utils::Common_Vertex, rendering::DEFAULT_VERTEX_MAX_COUNT> tmp;

        LoadModel(tmp, "res/Models/sword.txt");
        vertexLookup.Set(res::MeshType::Sword, MeshVertexView{ &allModelVertices[allModelVertices.count], tmp.count });
        allModelVertices.AppendArray(tmp);
        tmp.Clear();

        LoadModel(tmp, "res/Models/grass.txt");
        vertexLookup.Set(res::MeshType::Grass, MeshVertexView{ &allModelVertices[allModelVertices.count], tmp.count });
        allModelVertices.AppendArray(tmp);
        tmp.Clear();
    }
};

struct HostResources
{   
    Textures textures;
    Models   models;

    res::Terrain<20, 5, 2> terrain;

    //! should all load before any GPU resources are used
    HostResources() 
    {
        textures.Load();
        models.Load();
        terrain.Create();
    }
}; 

}//ns