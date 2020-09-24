//https://github.com/awwdev

#pragma once

#include "gpu/Vulkan/Meta/Context.hpp"
#include "gpu/Vulkan/_Old/res/Resources.hpp"
#include "app/Scene.hpp"
#include "res/_Old/CpuResources.hpp"

//vkCmdSetDepthBias(
//    cmdBuffer, 
//    scene.renderGraph.depthBiasConstantFactor,
//    scene.renderGraph.depthBiasClamp,
//    scene.renderGraph.depthBiasSlopeFactor
//);

namespace rpg::gpu::vuk {

inline void ShadowMap(VkCommandBuffer cmdBuffer, const uint32_t, VkResources& resources, const app::GameScene& scene)
{
    /*
    VkDeviceSize offsets {};
    const VkClearValue clears [] { 
        { .depthStencil = { 0, 0 } } //reversed z        
    };

    auto& renderPassShadow = resources.shadow.renderPass;
    auto& pipelineTerrain  = resources.terrain.pipelineShadow;
    auto& pushConsts       = resources.shadow.pushConsts;

    for(uint32_t cascadeIdx = 0; cascadeIdx < renderPassShadow.layerCount; ++cascadeIdx)
    {
        const auto beginInfo = renderPassShadow.GetBeginInfo(cascadeIdx, ArrayCount(clears), clears);
        vkCmdBeginRenderPass    (cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline       (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineTerrain.pipeline);
        pushConsts.cascade = cascadeIdx;
     
        vkCmdSetDepthBias(
            cmdBuffer, 
            scene.renderData.depthBiasConstantFactor[cascadeIdx],
            scene.renderData.depthBiasClamp[cascadeIdx],
            scene.renderData.depthBiasSlopeFactor[cascadeIdx]
        );

        vkCmdPushConstants      (cmdBuffer, pipelineTerrain.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pushConsts), &pushConsts);
        vkCmdBindVertexBuffers  (cmdBuffer, 0, 1, &resources.terrain.vbo.activeBuffer->buffer, &offsets);
        vkCmdDraw               (cmdBuffer, resources.terrain.vbo.count, 1, 0, 0); 
        vkCmdEndRenderPass      (cmdBuffer);
    }
    */

    //vkCmdSetDepthBias(
    //    cmdBuffer, 
    //    scene.renderGraph.depthBiasConstantFactor,
    //    scene.renderGraph.depthBiasClamp,
    //    scene.renderGraph.depthBiasSlopeFactor
    //);

    //DEFAULT
    //vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.common.pipelineShadow.layout, 0, 1, &resources.common.pipelineShadow.sets[cmdBufferIdx], 0, nullptr); 
    //vkCmdBindVertexBuffers  (cmdBuffer, 0, 1, &resources.common.vbo.activeBuffer->buffer, &offsets);
    //vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.common.pipelineShadow.pipeline);
    //FOR_ARRAY(scene.renderGraph.common_ubo.groupsVertexColor.usedIndices, i)
    //{
    //    const auto& ubo = scene.renderGraph.common_ubo.groupsVertexColor;
    //    const auto idx  = ubo.usedIndices[i];
    //    const auto vertOff   = resources.common.vbo.vertexGroups[idx].begin;
    //    const auto vertCount = resources.common.vbo.vertexGroups[idx].count;
    //    const auto instOff   = ubo.Get(idx).begin;
    //    const auto instCount = ubo.Get(idx).count;
    //    vkCmdDraw (cmdBuffer, vertCount, instCount, vertOff, instOff); 
    //}
}

inline void Geometry(VkCommandBuffer cmdBuffer, const uint32_t cmdBufferIdx, VkResources& resources, const app::GameScene& scene)
{
    const VkClearValue clears [] { 
        { .color = { 0.0f, 0.0f, 0.0f, 0.0f } },
        { .depthStencil = { 0, 0 } }, //reversed z
        { .color = { 1.0f, 1.0f, 1.0f, 1.0f } },//ignored
    };

    //const auto beginInfo_default = resources.common.renderPass.GetBeginInfo(cmdBufferIdx, clears);
    //VkDeviceSize offsets {};

    //vkCmdBeginRenderPass(cmdBuffer, &beginInfo_default, VK_SUBPASS_CONTENTS_INLINE);

    //?SKYDOME
    //vkCmdPushConstants      (cmdBuffer, resources.sky.pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(resources.sky.pushConsts), &resources.sky.pushConsts);
    //vkCmdBindPipeline       (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.sky.pipeline.pipeline);
    //vkCmdDraw               (cmdBuffer, 14, 1, 0, 0);

    //?TERRAIN
    //TODO: culling (loops)
    //vkCmdPushConstants      (cmdBuffer, resources.terrain.pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(resources.common_pushConsts2), &resources.common_pushConsts2);
    //vkCmdBindVertexBuffers  (cmdBuffer, 0, 1, &resources.terrain.vbo.activeBuffer->buffer, &offsets);
    //vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.terrain.pipeline.layout, 0, 1, &resources.terrain.pipeline.sets[cmdBufferIdx], 0, nullptr); 
    //vkCmdBindPipeline       (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.terrain.pipeline.pipeline);
    //vkCmdDraw               (cmdBuffer, resources.terrain.vbo.count, 1, 0, 0); 
    //if (scene.editorController.terrainWireMode) {
    //    vkCmdBindPipeline   (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.terrain.pipelineWire.pipeline);
    //    vkCmdDraw           (cmdBuffer, resources.terrain.vbo.count, 1, 0, 0); 
    //}

    //?DEFAULT
    /*vkCmdPushConstants      (cmdBuffer, resources.terrain.pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(resources.common_pushConsts), &resources.common_pushConsts);
    vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.common.pipelineVertexColor.layout, 0, 1, &resources.common.pipelineVertexColor.sets[cmdBufferIdx], 0, nullptr); 
    vkCmdBindVertexBuffers  (cmdBuffer, 0, 1, &resources.common.vbo.activeBuffer->buffer, &offsets);
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.common.pipelineVertexColor.pipeline);
    
    FOR_ARRAY(scene.renderGraph.common_ubo.groupsVertexColor.usedIndices, i)
    {
        const auto& ubo = scene.renderGraph.common_ubo.groupsVertexColor;
        const auto idx  = ubo.usedIndices[i];
        const auto vertOff   = resources.common.vbo.vertexGroups[idx].begin;
        const auto vertCount = resources.common.vbo.vertexGroups[idx].count;
        const auto instOff   = ubo.Get(idx).begin;
        const auto instCount = ubo.Get(idx).count;
        vkCmdDraw (cmdBuffer, vertCount, instCount, vertOff, instOff); 
    }*/

    /*
    vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.common.pipelineTexture.layout, 0, 1, &resources.common.pipelineTexture.sets[cmdBufferIdx], 0, nullptr); 
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.common.pipelineTexture.pipeline);
    vkCmdPushConstants(cmdBuffer,resources.common.pipelineTexture.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(resources.common_pushConsts), &resources.common_pushConsts);
    FOR_ARRAY(scene.renderGraph.common_ubo.groupsTexture.usedIndices, i)
    {
        const auto& ubo = scene.renderGraph.common_ubo.groupsTexture;
        const auto idx  = ubo.usedIndices[i];
        const auto vertOff   = resources.common.vbo.vertexGroups[idx].begin;
        const auto vertCount = resources.common.vbo.vertexGroups[idx].count;
        const auto instOff   = ubo.Get(idx).begin;
        const auto instCount = ubo.Get(idx).count;
        vkCmdDraw (cmdBuffer, vertCount, instCount, vertOff, instOff); 
    }
    */

    //vkCmdEndRenderPass(cmdBuffer);
}

inline void Post(VkCommandBuffer cmdBuffer, const uint32_t cmdBufferIdx, VkResources& resources, const app::GameScene& scene)
{
    VkClearValue clears [] { { .color { 0, 0, 0, 0 } } };
    VkDeviceSize offsets [1] {0};

    const auto beginInfo = resources.post.renderPass.GetBeginInfo(cmdBufferIdx, clears);
    vkCmdBeginRenderPass    (cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline       (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.post.pipeline.pipeline);
    vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.post.pipeline.layout, 0, 1, &resources.post.pipeline.sets[cmdBufferIdx], 0, nullptr); 
    vkCmdBindVertexBuffers  (cmdBuffer, 0, 1, &resources.post.vbo.activeBuffer->buffer, offsets);
    vkCmdDraw               (cmdBuffer, scene.renderData.gui_vbo_blur.count, 1, 0, 0); 
    vkCmdEndRenderPass      (cmdBuffer);
}

inline void UI(VkCommandBuffer cmdBuffer, const uint32_t cmdBufferIdx, VkResources& resources, const app::GameScene&)
{
    const auto beginInfo = resources.ui.renderPass.GetBeginInfo(cmdBufferIdx);

    vkCmdBeginRenderPass    (cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdPushConstants      (cmdBuffer, resources.ui.pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(resources.ui.pushConsts), &resources.ui.pushConsts);
    vkCmdBindPipeline       (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.ui.pipeline.pipeline);
    vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.ui.pipeline.layout, 0, 1, &resources.ui.pipeline.sets[cmdBufferIdx], 0, nullptr); 
    if (resources.ui.ubo.count > 0)
        vkCmdDraw(cmdBuffer, resources.ui.ubo.count * 6, 1, 0, 0); 

    vkCmdEndRenderPass(cmdBuffer);
}

inline void Test(VkCommandBuffer cmdBuffer, const uint32_t cmdBufferIdx, VkResources& resources, const app::GameScene&)
{
    auto& rp = resources.test.renderPass;
    auto& pi = resources.test.pipeline;
    
    VkClearValue clears [] {
        { .color { 0, 0, 0, 0 } },
        { .depthStencil { 0, 0 } },
    };

    const auto beginInfo = rp.GetBeginInfo(clears);
    vkCmdBeginRenderPass(cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    //? TEST TRIANGLE
    vkCmdBindPipeline   (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pi.pipeline);
    vkCmdDraw           (cmdBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass  (cmdBuffer);
}

inline void RecordCommands(
    VkResources& resources,
    Commands& commands, 
    const uint32_t cmdBufferIdx, 
    const double, 
    const app::GameScene& scene)
{
    auto cmdBuffer = commands.cmdBuffers[cmdBufferIdx];
    const auto beginInfo = vuk::CreateCmdBeginInfo();
    VkCheck(vkBeginCommandBuffer(cmdBuffer, &beginInfo));

    //ShadowMap   (cmdBuffer, cmdBufferIdx, resources, scene);
    //Geometry    (cmdBuffer, cmdBufferIdx, resources, scene);

    Test    (cmdBuffer, cmdBufferIdx, resources, scene);
    Post    (cmdBuffer, cmdBufferIdx, resources, scene);
    UI      (cmdBuffer, cmdBufferIdx, resources, scene); 

    VkCheck(vkEndCommandBuffer(cmdBuffer));
}

} //ns