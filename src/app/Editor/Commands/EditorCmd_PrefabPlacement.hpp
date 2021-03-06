//https://github.com/awwdev

#pragma once
#include "ecs/ECS.hpp"
#include "res/Resources.hpp"
#include "res/Prefab/PrefabEnum.hpp"
#include "app/Editor/EditorCommandDirection.hpp"

namespace rpg::app {

struct EditorCmd_PrefabPlacement
{
    res::PrefabEnum prefabEnum;
    com::Vec3f position;
    ecs::ID prefabID;
    com::Vec4f baseColor; //should not really be here

    template<EditorCommandDirection DIR>
    void Execute(res::Resources& res, ecs::ECS& ecs)
    {
        if constexpr(DIR == EditorCommandDirection::Backwards)
        {
            ecs.RemoveEntity(prefabID);
        }
        if constexpr(DIR == EditorCommandDirection::Forwards)
        {
            prefabID = ecs.AddEntity(prefabEnum);
            auto& transform_component = ecs.arrays.transform_components.Get(prefabID);
            auto& render_component = ecs.arrays.render_components.Get(prefabID);
          
            transform_component.translation = position;

            if (render_component.materialEnum == res::MeshMaterialEnum::Foliage)
                 render_component.materialSettings.foliage.baseColor = baseColor;           
            
            //randomness test
            transform_component.translation.x += (rand() % 10) / 10.f - 0.5f;
            transform_component.translation.z += (rand() % 10) / 10.f - 0.5f;
            transform_component.scale.x += (rand() % 10) / 10.f;
            transform_component.scale.z += (rand() % 10) / 10.f;
            transform_component.scale.y += (rand() % 10) / 10.f;
        }
    }

};

} //ns