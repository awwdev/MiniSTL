//https://github.com/awwdev

#pragma once
#include "mini/Window/WindowEvents.hpp"
#include "mini/Box/String.hpp"
#include "mini/App/InputMode.hpp"
#include "mini/Rendering/RenderGraph.hpp"
#include "mini/Resources/Terrain/Terrain.hpp"
#include "mini/UI/UI.hpp"
#include "mini/UI/UI_Terrain.hpp"
#include "mini/UI/UI_Stats.hpp"
#include "mini/Rendering/Sun.hpp"
#include "mini/ECS/Prefabs/PrefabTypes.hpp"

#include <charconv>

namespace mini::ui {

struct TerrainUI
{
    Window wnd {
        .title = "Terrain",
        .rect = { wnd::global::window_w - 200.f, 0.f, 200.f, 300.f },
    };

    Slider<f32> sunRotSlider {
        .name = "sun rot",
        .min  = 0,
        .max  = 6.28f,
    };

    Slider<f32> brushSlider {
        .name  = "brush size",
        .min   = 1.f,
        .max   = 20.f,
    };

    Slider<f32> rSlider {
        .name  = "R",
        .min   = 0.f,
        .max   = 1.f,
    };
    Slider<f32> gSlider {
        .name  = "G",
        .min   = 0.f,
        .max   = 1.f,
    };
    Slider<f32> bSlider {
        .name  = "B",
        .min   = 0.f,
        .max   = 1.f,
    };

    Slider<f32> depthBiasConstantFactor {
        .name  = "fac",
        .knobPos = 0.5,
        .min   =-5000.f,
        .max   = 0.f,
    };
    Slider<f32> depthBiasClamp {
        .name  = "clamp",
        .knobPos = 0.5,
        .min   =-100.f,
        .max   = 100.f,
    };
    Slider<f32> depthBiasSlopeFactor {
        .name  = "slope",
        .knobPos = 0.5f,
        .min   =-10.f,
        .max   =  2,
    };

    Slider<f32> cascadeZoom0 {
        .name    = "cascadeZoom0",
        .min     = 0.0001f,
        .max     = 0.1f,
    };
    Slider<f32> cascadeZoom1 {
        .name    = "cascadeZoom1",
        .min     = 0.0001f,
        .max     = 0.1f,
    };
    Slider<f32> cascadeZoom2 {
        .name    = "cascadeZoom2",
        .min     = 0.0001f,
        .max     = 0.1f,
    };
    Slider<f32> maxShadowDist0 {
        .name    = "maxShadowDist0",
        .min     = 0,
        .max     = 50,
    };
    Slider<f32> maxShadowDist1 {
        .name    = "maxShadowDist1",
        .min     = 30,
        .max     = 300,
    };
    Slider<f32> maxShadowDist2 {
        .name    = "maxShadowDist2",
        .min     = 300,
        .max     = 1000,
    };

    List<box::String<20>, 10> prefabList {
        .name  = "Prefabs",
        .rect  = { 0, 0, 0, 100 },
    };


    TerrainUI()
    {
        FOR_STRING_MAP_BEGIN_CONST(ecs::PREFAB_STR_TO_ENUM, item)
            prefabList.items.Append(item.key);
        FOR_STRING_MAP_END
    }
};

template<
    auto QUAD_COUNT, 
    auto QUAD_LEN, 
    auto QUADRANT_COUNT>
inline void DrawUI_Terrain(
    res::Terrain<QUAD_COUNT, QUAD_LEN, QUADRANT_COUNT>& terrain,
    rendering::Sun& sun)
{
    using TERRAIN_T = res::Terrain<QUAD_COUNT, QUAD_LEN, QUADRANT_COUNT>;

    static TerrainUI ui {};

    //WINDOW
    DrawWindow(ui.wnd);
    ui.wnd.UpdateInputMode();

    //SUN
    sun.t = DrawSlider(ui.sunRotSlider, ui.wnd);

    //BRUSH
    terrain.settings.brushSize = DrawSlider(ui.brushSlider, ui.wnd);

    //VERTEX MODE
    box::String<50> vertModeStr = "Vertex Mode: ";
    if (terrain.settings.mode == res::EditMode::VertexGrab)
        vertModeStr.Append("VertexGrab");
    if (terrain.settings.mode == res::EditMode::VertexPaint)
        vertModeStr.Append("VertexPaint");
    DrawText(vertModeStr, ui.wnd);

    //VERTEX COLOR
    using namespace utils;
    terrain.settings.vertexColor[X] = DrawSlider(ui.rSlider, ui.wnd);
    terrain.settings.vertexColor[Y] = DrawSlider(ui.gSlider, ui.wnd);
    terrain.settings.vertexColor[Z] = DrawSlider(ui.bSlider, ui.wnd);

    //SHADOW ARTIFACTS
    //g_aciveRenderGraph->depthBiasConstantFactor = DrawSlider(ui.depthBiasConstantFactor, ui.wnd);
    //g_aciveRenderGraph->depthBiasClamp          = DrawSlider(ui.depthBiasClamp, ui.wnd);
    //g_aciveRenderGraph->depthBiasSlopeFactor    = DrawSlider(ui.depthBiasSlopeFactor, ui.wnd);

    //CACADES
    //g_aciveRenderGraph->cascadeZoom0   = DrawSlider(ui.cascadeZoom0, ui.wnd);
    //g_aciveRenderGraph->cascadeZoom1   = DrawSlider(ui.cascadeZoom1, ui.wnd);
    //g_aciveRenderGraph->cascadeZoom2   = DrawSlider(ui.cascadeZoom2, ui.wnd);
    //g_aciveRenderGraph->maxShadowDist0 = DrawSlider(ui.maxShadowDist0, ui.wnd);
    //g_aciveRenderGraph->maxShadowDist1 = DrawSlider(ui.maxShadowDist1, ui.wnd);
    //g_aciveRenderGraph->maxShadowDist2 = DrawSlider(ui.maxShadowDist2, ui.wnd);

    //LIST
    if (DrawList(ui.prefabList, ui.wnd)){
        const auto& str  = ui.prefabList.items[ui.prefabList.activeIndex];
        const auto* type = ecs::PREFAB_STR_TO_ENUM.GetOptional(str.data);
        terrain.settings.prefabType = *type;
    }

}

}//ns