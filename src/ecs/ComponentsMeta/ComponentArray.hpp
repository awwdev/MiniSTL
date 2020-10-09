//https://github.com/awwdev

#pragma once
#include "com/box/Array.hpp"
#include "com/box/Bitset.hpp"

#include "ecs/EntityID.hpp"
#include "ecs/ComponentsMeta/ComponentEnum.hpp"

namespace rpg::ecs {
    
template<class COMPONENT_T, auto MAX_COMPONENT_COUNT = MAX_ENTITY_COUNT>
struct ComponentArray
{
    using COMPONENT = COMPONENT_T;

    com::Array<COMPONENT_T, MAX_COMPONENT_COUNT> dense;
    ID componentLookup [(idx_t) MAX_COMPONENT_COUNT];
    ID entityLookup    [(idx_t) MAX_COMPONENT_COUNT];

    auto*       GetPtr (const ID entityID)       { return componentLookup[entityID] == ECS_NULL ? nullptr : &dense[componentLookup[entityID]]; }
    const auto* GetPtr (const ID entityID) const { return componentLookup[entityID] == ECS_NULL ? nullptr : &dense[componentLookup[entityID]]; }
    auto&       Get    (const ID entityID)       { return dense[componentLookup[entityID]]; }
    const auto& Get    (const ID entityID) const { return dense[componentLookup[entityID]]; }

    ComponentArray() 
    {
        Clear(); //used for initialization          
    }

    template<class... CtorArgs>
    COMPONENT_T& SetComponent(const ID entityID, CtorArgs&&... args)
    {
        //overwrite
        if (componentLookup[entityID] != ECS_NULL) {
            dbg::LogWarning("add component that alread exists");
            dense[componentLookup[entityID]] = { std::forward<CtorArgs>(args)... };
            return dense[componentLookup[entityID]];
        }
        //new
        dense.Append(std::forward<CtorArgs>(args)...);
        componentLookup[entityID] = dense.Count() - 1;
        entityLookup[dense.Count() - 1] = entityID;
        return dense.Last();
    }

    void Clear()
    {
        dense.Clear();
        for(idx_t i = 0; i < (idx_t) MAX_COMPONENT_COUNT; ++i) {
            componentLookup[i] = ECS_NULL;
            entityLookup[i]    = ECS_NULL;
        }  
    }

};

}//NS