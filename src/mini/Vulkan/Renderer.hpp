//https://github.com/awwdev

#pragma once

#include "mini/Vulkan/Context.hpp"
#include "mini/Vulkan/Ctors.hpp"
#include "mini/Vulkan/Resources.hpp"
#include "mini/Resources/HostResources.hpp"
#include "mini/Scene/Scene.hpp"
#include "mini/Utils/Vertex.hpp"
#include "mini/Box/String.hpp"
#include "mini/Window/AppEvents.hpp"
#include "mini/Scene/IRenderer.hpp"

namespace mini::vk
{
    struct Renderer : IRenderer
    {
        Context         context;
        VkResources     resources;
        Commands        commands;
        Synchronization sync;

        uint32_t currentFrame = 0;
        
        //TODO: ref to current window size
        Renderer(const vk::WindowHandle& wndHandle, hostRes::HostResources& hostResources)
            : context   { wndHandle }
            , resources { context.device }
        {
            //context.Create(wndHandle);
            sync.Create(context);
            commands.Create(context);
            resources.Create(context, hostResources, commands);
        }


        inline void RecreateScwapchain()
        {
            vkDeviceWaitIdle(context.device);

            if (!context.RecreateSwapchain())
                return;

            resources.default_renderPass.~Default_RenderPass();
            resources.default_renderPass.Create(context);

            resources.default_pipeline.~Default_Pipeline();
            resources.default_pipeline.Recreate(context);

            commands.~Commands();
            commands.Create(context);
        }


        inline void UpdateVkResources(const scenes::Scene& scene, const double dt)
        {
            //? frame begin (clear stuff) is done before all vk calls
            //? collecting data in scene update 
            //? and pushing data to gpu here (sync)
            resources.pushConstants.wnd_w = wnd::window_w;
            resources.pushConstants.wnd_h = wnd::window_h;

            resources.default_vb.Store(vertices.Data(), vertices.Count());
            resources.default_vb.indexBuffer.Store(indices.Data(), indices.Count() * sizeof(uint32_t));

            //TODO: store all at once, but then we needed aligned host storage ?
            //TODO: do not hardcode 256 but gpu ubo min size
            resources.default_ub.buffer.Store(&ubo_TextureUsage[0], sizeof(decltype(ubo_TextureUsage)::DATA_T), 0);
            resources.default_ub.buffer.Store(&ubo_TextureUsage[1], sizeof(decltype(ubo_TextureUsage)::DATA_T), 256);
        }


        inline void RecordCommands(const uint32_t cmdBufferIdx, const double dt, const scenes::Scene& scene)
        {
            auto& cmdBuffer = commands.cmdBuffers[cmdBufferIdx];
            VkDeviceSize vboOffsets { 0 };
            uint32_t     uboOffsets { 256 * 0 }; //TODO: do not hardcode 256 but use gpu min ubo size

            auto beginInfo = vk::CreateCmdBeginInfo();
            VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &beginInfo));

            const VkClearValue clears { .color = { 0.1f, 0.1f, 0.1f, 1.0f } };
            const auto renderPassInfo = CreateRenderPassBeginInfo(
                context, 
                resources.default_renderPass.renderPass, 
                resources.default_renderPass.framebuffers[cmdBufferIdx],
                &clears
            );
            vkCmdBeginRenderPass (cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline       (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.default_pipeline.pipeline);
            vkCmdPushConstants      (cmdBuffer, resources.default_pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(resources.pushConstants), &resources.pushConstants);
            vkCmdBindVertexBuffers  (cmdBuffer, 0, 1, &resources.default_vb.buffer.buffer, &vboOffsets);
            vkCmdBindIndexBuffer    (cmdBuffer, resources.default_vb.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
            
            FOR_ARRAY(vertexGroups, i)
            {
                const auto& group = vertexGroups[i];
                uboOffsets = 256 * i; //TODO: do not hardcode 256 but use gpu min ubo size
                vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.default_pipeline.layout, 0, 1, &resources.default_shader.sets[cmdBufferIdx], 1, &uboOffsets); 
                vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, resources.default_pipeline.layout, 1, 1, &resources.default_shader.sets[cmdBufferIdx], 1, &uboOffsets); 
                vkCmdDrawIndexed        (cmdBuffer, group.IndexCount(), 1, 0, group.v1, 0);
            }
            
            vkCmdEndRenderPass(cmdBuffer);
            VK_CHECK(vkEndCommandBuffer(cmdBuffer));
        }


        inline void Render(const double dt, const scenes::Scene& scene)
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