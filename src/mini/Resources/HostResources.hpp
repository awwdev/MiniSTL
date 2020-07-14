//https://github.com/awwdev
#pragma once

#include "mini/Memory/Allocator.hpp"
#include "mini/Resources/TextureLoader.hpp"
#include "mini/Box/Map.hpp"
#include "mini/Box/Array.hpp"
#include "mini/Debug/Logger.hpp"
#include "mini/Resources/TextureArray.hpp"
#include "mini/Resources/ModelLoader.hpp"
#include "mini/Resources/Mesh.hpp"

namespace mini::hostRes
{
    struct HostResources
    {
        TextureArray<96, 16, 16, 1> fontTextures; // added one texture that is all FF which can be used for blank quad () 

        box::Array<utils::Vertex, 250> sword;

        HostResources() 
        {
            //currently load is instant inside ctor
            fontTextures.LoadArray("res/TextureArray"); //RLE would be nice
            
            LoadModel(sword, "res/Models/sword.txt");
            resources::MESH_VERTEX_MAP.Set(resources::MeshType::Sword, resources::MeshVertexView{sword.dataPtr, sword.Count()});
        }
    };















/*
    struct TextureInfo
    {
        chars_t path;
        u32 w, h;
    };

    //! ADD RESOURCES HERE---------------------------------------------------
    //always add both name and map entry
    enum TextureName 
    {
        Texture1, 
        Texture2,
        Font,
        ENUM_END
    };

    using TextureInfos = box::Map<TextureInfo, TextureName::ENUM_END>;
    using P = TextureInfos::Pair_t;
    const TextureInfos TEXTURE_INFOS //size is used to figure out which memory to claim (texture array)
    { //order does not matter
        P{ TextureName::Texture1,   { "res/Textures/Texture1.bmp",  32, 32 } },
        P{ TextureName::Texture2,   { "res/Textures/Texture2.bmp",  48, 48 } },
        P{ TextureName::Font,       { "res/Textures/Font.bmp",      256, 128 } },
    };
    //! ---------------------------------------------------------------------

    struct Textures
    {
        //used to auto create gpu resources! (vk::Image)
        ITexture* iTextures[TextureName::ENUM_END] { nullptr }; //size agnostic table

        //? ARRAY OF SIZES 
        //! add to the Claim/Load method!
        mem::BlockPtr<box::Array<Texture<  32,   32>, 2>> ptrTextures1 { mem::AutoClaim::Yes };
        mem::BlockPtr<box::Array<Texture<1024, 1024>, 2>> ptrTextures2 { mem::AutoClaim::Yes };

        inline void Load()
        {
            FOR_MAP_BEGIN(TEXTURE_INFOS, i)
                AssignTextureToArray(i, 
                    //! ADD HERE ALL TEXTURE ARRAYS
                    ptrTextures1, 
                    ptrTextures2
                ); 
                iTextures[i]->LoadFrom_BMP(TEXTURE_INFOS.GetValue(i).path);
            FOR_MAP_END
        }

        template<class... T>
        void AssignTextureToArray(const u32 idx, T&... arrays)
        {
            u32 arrayIdx = 0;

            const auto fn = [&](auto& arr)
            {
                using Texture_T = typename std::decay_t<decltype(arr)>::DATA_T::DATA_T;

                if (iTextures[idx] != nullptr) return;

                const auto& textureInfo = TEXTURE_INFOS.GetValue(idx);
                if (textureInfo.w <= Texture_T::MAX_WIDTH && textureInfo.h <= Texture_T::MAX_HEIGHT) 
                {
                    iTextures[idx] = &arr.Get().AppendReturn(); //"assignment"
                    LOG("assign texture to ", arrayIdx);
                }
                ++arrayIdx;    
            };

            ((fn(arrays)), ...); //unfold allows to have "iteration" over heterogeneous types

            if (iTextures[idx] == nullptr)
                ERR("not texture array found");
        }
    };
*/
   

}//ns