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
            auto& mainComponent = ecs.arrays.mainComponents.Get(prefabID);
            mainComponent.translation = position;
            mainComponent.baseColor = baseColor;

            //randomness test
            mainComponent.translation.x += (rand() % 10) / 10.f - 0.5f;
            mainComponent.translation.z += (rand() % 10) / 10.f - 0.5f;
            mainComponent.scale.x += (rand() % 10) / 10.f;
            mainComponent.scale.z += (rand() % 10) / 10.f;
            mainComponent.scale.y += (rand() % 10) / 10.f;
        }
    }

};

} //ns