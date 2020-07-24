//https://github.com/awwdev

#pragma once
#include "mini/Vulkan/Context.hpp"
#include "mini/Vulkan/Ctors.hpp"
#include "mini/Vulkan/Resources.hpp"
#include "mini/Resources/HostResources.hpp"
#include "mini/App/Scene.hpp"
#include "mini/Rendering/RenderGraph.hpp"
#include "mini/Utils/Structs.hpp"
#include "mini/Box/String.hpp"
#include "mini/Window/AppEvents.hpp"
#include "mini/Utils/DeltaTime.hpp"

namespace mini::vk
{
    struct VkRenderer
    {
        Context         context;
        VkResources     resources;
        Commands        commands;
        Synchronization sync;
        uint32_t        currentFrame = 0;
        
        VkRenderer(const vk::WindowHandle& wndHandle, res::HostResources& hostResources)
        {
            context.Create(wndHandle); //there is a global ptr to vk context
            sync.Create();
            commands.Create();
            resources.Create(hostResources, commands);
        }

        void RecreateScwapchain()
        {
            vkDeviceWaitIdle(context.device);
            if (!context.RecreateSwapchain())
                return;

            resources.RecreateSwapchain(commands.cmdPool);

            commands.~Commands();
            commands.Create();
        }


        void UpdateVkResources(const app::GameScene& scene, const double dt)
        {
            resources.common_pushConsts.projection = {
                scene.camera.GetProjView()
            };
            resources.common_pushConsts.wnd_w = wnd::window_w;
            resources.common_pushConsts.wnd_h = wnd::window_h;

            resources.ui.ubo.Clear();
            resources.ui.ubo.Store(scene.renderGraph.ui_ubo);

            resources.default.ubo.Clear();
            resources.default.ubo.Store(scene.renderGraph.default_ubo);

            //TODO: selectively update terrain data
            //TODO: make stuff expandable
            resources.terrain.vbo.Clear();
            decltype(scene.terrain.verts)::CREF terrain = scene.terrain.verts;
            resources.terrain.vbo.AppendGroup(terrain);
        }

        void RecordCommands(const uint32_t cmdBufferIdx, const double dt, const app::GameScene& scene)
        {
            auto& cmdBuffer = commands.cmdBuffers[cmdBufferIdx];
            VkDeviceSize vboOffsets { 0 };
            uint32_t     uboOffsets { 0 };

            auto beginInfo = vk::CreateCmdBeginInfo();
            VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &beginInfo));

            const auto beginInfo_ui = CreateRenderPassBeginInfo(
                resources.ui.renderPass.renderPass,
                resources.ui.renderPass.framebuffers[cmdBufferIdx]
            );

            const VkClearValue clears_default [] { 
                { .color = { 0.1f, 0.1f, 0.1f, 1.0f } },
                { .depthStencil = { 0, 0 } } //reversed z
            };
            const auto beginInfo_default = CreateRenderPassBeginInfo(
                resources.default.renderPass.renderPass,
                resources.default.renderPass.framebuffers[cmdBufferIdx],
                ARRAY_COUNT(clears_default),
                clears_default
            );

            VkDeviceSize offsets = 0;

            vkCmdBeginRenderPass    (cmdBuffer, &beginInfo_default, VK_SUBPASS_CONTENTS_INLINE); //used to terrain too now

            //! TERRAIN
            //TODO: culling (loops)
            vkCmdPushConstants      (cmdBuffer, resources.terrain.pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(resources.common_pushConsts), &resources.common_pushConsts);
            vkCmdBindVertexBuffers  (cmdBuffer, 0, 1, &resources.terrain.vbo.activeBuffer->buffer, &offsets);
            vkCmdBindPipeline       (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.terrain.pipeline.pipeline);
            vkCmdDraw               (cmdBuffer, resources.terrain.vbo.count, 1, 0, 0); 
            vkCmdBindPipeline       (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.terrain.pipelineWire.pipeline);
            vkCmdDraw               (cmdBuffer, resources.terrain.vbo.count, 1, 0, 0); 

            //! DEFAULT
            vkCmdPushConstants      (cmdBuffer, resources.default.pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(resources.common_pushConsts), &resources.common_pushConsts);
            vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.default.pipeline.layout, 0, 1, &resources.default.pipeline.sets[cmdBufferIdx], 0, nullptr); 
            vkCmdBindPipeline       (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.default.pipeline.pipeline);
            vkCmdBindVertexBuffers  (cmdBuffer, 0, 1, &resources.default.vbo.activeBuffer->buffer, &offsets);
            FOR_USED_INDICES_MAP_BEGIN(scene.renderGraph.default_ubo.groups, usedIndex)
            {
                const auto vertOff   = resources.default.vbo.vertexGroups[usedIndex].begin;
                const auto vertCount = resources.default.vbo.vertexGroups[usedIndex].count;
                const auto instOff   = scene.renderGraph.default_ubo.groups.Get(usedIndex).begin;
                const auto instCount = scene.renderGraph.default_ubo.groups.Get(usedIndex).count;
                vkCmdDraw (cmdBuffer, vertCount, instCount, vertOff, instOff); 
            }
            FOR_USED_INDICES_MAP_END
            vkCmdEndRenderPass      (cmdBuffer);
           
            //! TEXT
            vkCmdPushConstants      (cmdBuffer, resources.ui.pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(resources.common_pushConsts), &resources.common_pushConsts);
            vkCmdBeginRenderPass    (cmdBuffer, &beginInfo_ui, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline       (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.ui.pipeline.pipeline);
            vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.ui.pipeline.layout, 0, 1, &resources.ui.pipeline.sets[cmdBufferIdx], 0, nullptr); 
            vkCmdDraw               (cmdBuffer, resources.ui.ubo.count * 6, 1, 0, 0); 
            vkCmdEndRenderPass      (cmdBuffer);
            VK_CHECK(vkEndCommandBuffer(cmdBuffer));
        }


        void Render(const double dt, app::GameScene& scene)
        {
            if (wnd::CheckEvent(wnd::EventType::Window_Resize)){
                RecreateScwapchain();
                return;
            } //checking for size==0 is done previously (will also pause other logic)

            if (vkWaitForFences(context.device, 1, &sync.fences[currentFrame], VK_FALSE, 0) != VK_SUCCESS)
                return;

            uint32_t imageIndex = 0;
            const auto acquireRes = vkAcquireNextImageKHR(
                context.device, 
                context.swapchain, 
                0, 
                sync.imageAcquired[currentFrame], 
                VK_NULL_HANDLE, 
                &imageIndex
            ); 

            switch(acquireRes)
            {
                case VK_SUCCESS: break;
                case VK_ERROR_OUT_OF_DATE_KHR: RecreateScwapchain(); return; //when?!
                default: return;
            }

            if (sync.inFlight[imageIndex] != VK_NULL_HANDLE) {
                vkWaitForFences(context.device, 1, &sync.inFlight[imageIndex], VK_FALSE, UINT64_MAX); //important to wait
            }
            sync.inFlight[imageIndex] = sync.fences[currentFrame];
            VK_CHECK(vkResetFences(context.device, 1, &sync.fences[currentFrame]));

            //!UPDATE GPU RESOURCES AND RECORD COMMANDS----------
            UpdateVkResources(scene, dt);
            RecordCommands(imageIndex, dt, scene);
            //!--------------------------------------------------

            const auto submitInfo = SubmitInfo(sync.imageAcquired[currentFrame], sync.imageFinished[currentFrame], commands.cmdBuffers[imageIndex]);
            VK_CHECK(vkQueueSubmit(context.queue, 1, &submitInfo, sync.fences[currentFrame]));

            const auto presentInfo = PresentInfo(sync.imageFinished[currentFrame], context.swapchain, imageIndex);
            VK_CHECK(vkQueuePresentKHR(context.queue, &presentInfo));

            currentFrame = (currentFrame + 1) % (context.swapImages.count - 1);
        }
    };

}//ns
