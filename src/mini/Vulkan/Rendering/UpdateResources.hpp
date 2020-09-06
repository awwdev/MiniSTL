//https://github.com/awwdev

#pragma once
#include "mini/Vulkan/Core.hpp"
#include "mini/Vulkan/Context.hpp"
#include "mini/Vulkan/Resources/Resources.hpp"
#include "mini/Vulkan/Commands.hpp"
#include "mini/App/Scene.hpp"
#include "mini/Resources/HostResources.hpp"

namespace mini::vk {

const utils::Mat4f BIAS { 
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0 
};

inline void CreateCascades2(utils::Mat4f (&cascades)[3], const app::GameScene& scene, rendering::Terrain_UniformData& uboMeta)
{
    using namespace utils;

    auto shadowDist0 = scene.renderGraph.maxShadowDist0;
    auto shadowDist1 = scene.renderGraph.maxShadowDist1;
    auto shadowDist2 = scene.renderGraph.maxShadowDist2;
    //shadowDist = 30;

    uboMeta = {
        .camProj = (app::global::inputMode == app::global::PlayMode ? scene.playerController.camera.perspective : scene.editorController.camera.perspective),
        .camView = (app::global::inputMode == app::global::PlayMode ? scene.playerController.camera.view        : scene.editorController.camera.view),
        .sunView = scene.sun.GetView(),
        .sunDir  = utils::Normalize(scene.sun.pos),
        .cascadeFadeDist0 = shadowDist0,
        .cascadeFadeDist1 = shadowDist1,
        .cascadeFadeDist2 = shadowDist2,
    };

    auto& camera = scene.editorController.camera;

    //auto farPlane = shadowDist;
    auto sunDir   = utils::Normalize(scene.sun.pos);
    auto sunDist  = 10;

    Vec3f invCameraPos { -camera.position[X], camera.position[Y], -camera.position[Z] };
    Vec3f cameraDir    { Sin(camera.rotation[Y] * 3.14f/180.f), 0, Cos(camera.rotation[Y] * 3.14f/180.f) };

    auto cascadePos0 = invCameraPos;
    auto cascadePos1 = invCameraPos;// + (Normalize(cameraDir) * -farPlane);
    auto cascadePos2 = invCameraPos;// + (Normalize(cameraDir) * -farPlane);

    auto sunPos0 = cascadePos0 + (sunDir * sunDist);
    auto sunPos1 = cascadePos1 + (sunDir * sunDist);
    auto sunPos2 = cascadePos2 + (sunDir * sunDist);

    auto view0 = utils::LookAt(sunPos0, cascadePos0);
    auto view1 = utils::LookAt(sunPos1, cascadePos1);
    auto view2 = utils::LookAt(sunPos2, cascadePos2);

    float S;
    const float D = 0.00001f; 
    const float Z = 0.02f;

    

    S = scene.renderGraph.cascadeZoom0;
    //S = 0.07;
    utils::Mat4f ortho0 ={
        S, 0, 0, 0,
        0, S, 0, 0,
        0, 0, D, 0,
        0, 0, Z, 1,
    };

    S = scene.renderGraph.cascadeZoom1;
    //S = 0.010;
    utils::Mat4f ortho1 ={
        S, 0, 0, 0,
        0, S, 0, 0,
        0, 0, D, 0,
        0, 0, Z, 1,
    };

    S = scene.renderGraph.cascadeZoom2;
    //S = 0.002;
    utils::Mat4f ortho2 ={
        S, 0, 0, 0,
        0, S, 0, 0,
        0, 0, D, 0,
        0, 0, Z, 1,
    };

    cascades[0] = ortho0 * view0;
    cascades[1] = ortho1 * view1;
    cascades[2] = ortho2 * view2;
}

inline void UpdateVkResources_GameScene(VkResources& resources, const app::GameScene& scene, res::HostResources& hostRes, double dt, Commands& commands)
{
    

    //resources.common_pushConsts.camera = scene.camera.GetOrthographic() * scene.sun.GetView();
    //resources.common_pushConsts.camera = scene.camera.GetPerspective()  * scene.playerController.GetView(scene.ecs);
    //resources.common_pushConsts.camera = scene.camera.GetPerspective()  * scene.camera.GetView();

    //TODO: this should be handled somewhere else which is active
    if (app::global::inputMode == app::global::PlayMode){
        resources.common_pushConsts.camera = resources.sky.pushConsts.camera = scene.playerController.camera.perspective * scene.playerController.camera.view;

        resources.common_pushConsts2.projection = scene.playerController.camera.perspective;
        resources.common_pushConsts2.view       = scene.playerController.camera.view;
    }
        
    else{
        resources.common_pushConsts.camera = resources.sky.pushConsts.camera = scene.editorController.camera.perspective * scene.editorController.camera.view;
        //resources.common_pushConsts.camera = scene.sun.GetOrthographic() * scene.sun.GetView();

        resources.common_pushConsts2.projection = scene.editorController.camera.perspective;
        resources.common_pushConsts2.view       = scene.editorController.camera.view;
    }

    //TODO: THIS IS THE ISSUE !!! we would need the right sun persp
    resources.common_pushConsts2.sunView = scene.sun.GetView();
    resources.common_pushConsts2.sunDir  = utils::Normalize(scene.sun.pos * 1);
    

    //? UBO META TEST
    static utils::Mat4f cascades [3]{};
    rendering::Terrain_UniformData uboMeta {};
    CreateCascades2(cascades, scene, uboMeta);

    uboMeta.sunProjCasc[0] = BIAS * cascades[0];
    uboMeta.sunProjCasc[1] = BIAS * cascades[1];
    uboMeta.sunProjCasc[2] = BIAS * cascades[2];
    resources.terrain.uboMeta.Store(uboMeta);

    resources.shadow.pushConsts.sunCasc[0] = cascades[0];
    resources.shadow.pushConsts.sunCasc[1] = cascades[1];
    resources.shadow.pushConsts.sunCasc[2] = cascades[2];




    static float t = 0;
    t+=(f32)dt;
    resources.common_pushConsts.time = t;
    resources.common_pushConsts.sun    = {};


    resources.common_pushConsts.sunBias= BIAS * scene.sun.GetOrthographic(0) * scene.sun.GetView();
    resources.common_pushConsts.sunDir = utils::Normalize(scene.sun.pos * 1);
    resources.ui.pushConsts.wnd_w = wnd::global::window_w;
    resources.ui.pushConsts.wnd_h = wnd::global::window_h;
    resources.sky.pushConsts.topColor = { 0.1f, 0.1f, 1.0f, 1 };
    resources.sky.pushConsts.botColor = { 1.0f, 1.0f, 1.0f, 1 };

    resources.ui.ubo.Clear();
    resources.ui.ubo.Store(scene.renderGraph.ui_ubo);

    resources.default.ubo.Clear();
    resources.default.ubo.Store(scene.renderGraph.default_ubo);

    //update terrain quadrant that is edited
    if (hostRes.terrain.settings.baked){
        if (!resources.terrain.vbo.IsBaked())
            resources.terrain.vbo.Bake(commands.cmdPool);
    }
    else
    {
        resources.terrain.vbo.activeBuffer = &resources.terrain.vbo.cpuBuffer;
        FOR_ARRAY(hostRes.terrain.settings.dirtyQuadrants, i){
            const auto quadrantIdx = hostRes.terrain.settings.dirtyQuadrants[i];
            resources.terrain.vbo.UpdateGroup(quadrantIdx, hostRes.terrain.GetQuadrant(quadrantIdx).verts);
        }
    }
    

}

} //ns