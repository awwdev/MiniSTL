//https://github.com/awwdev

#pragma once
#include "ecs/ComponentsMeta/ComponentEnum.hpp"
#include "com/Matrix.hpp"
#include "dbg/Logger.hpp"

namespace rpg::ecs {

struct TransformComponent
{
    com::Vec3f scale; 
    com::Vec3f rotation; 
    com::Vec3f translation; 

    //defaults
    TransformComponent( 
        const com::Vec3f pScale       = { 1, 1, 1 },
        const com::Vec3f pRotation    = { 0, 0, 0 },
        const com::Vec3f pTranslation = { 0, 0, 0 })
        : scale        { pScale }
        , rotation     { pRotation }
        , translation  { pTranslation }
    {}

    //parsing
    TransformComponent(const ComponentDataStringPairs& pairs) : TransformComponent()
    {
        FOR_ARRAY(pairs, i) {
            const auto& pair = pairs[i];
            //const auto dataType = GetComponentDataType(pair.key);      
            //switch(dataType)
            {
                //case ComponentDataEnum::scale: 
                //scale = ParseComponentData<com::Vec3f>(pair.val); 
                //break;

                //default: dbg::LogWarning("component data type not defined for this component");
            }
        }
    }

};

}//NS