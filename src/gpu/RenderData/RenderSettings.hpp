//https://github.com/awwdev

#pragma once

namespace rpg::gpu {

union RenderSettings
{
    struct FoliageSettings
    {
    };

    struct CommonSettings
    {
    };

    struct MetallicSettings
    {
    };

    struct LineSettings
    {
    };

    FoliageSettings foliage;
    CommonSettings common;
    MetallicSettings metallic;
    LineSettings lines;
};

}//ns