//https://github.com/awwdev

#pragma once
#include "mini/Vulkan/Core.hpp"
#include "mini/Vulkan/Context.hpp"
#include "mini/Vulkan/Objects/RenderPass.hpp"

namespace mini::vk
{
    inline void CreateRenderPass_Default(RenderPass& rp, VkCommandPool cmdPool)
    {  
        const VkAttachmentDescription colorDesc {
            .flags          = 0 ,
            .format         = g_contextPtr->format, 
            .samples        = rp.SAMPLE_COUNT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        constexpr VkFormat DEPTH_FORMAT = VK_FORMAT_D32_SFLOAT; //VK_FORMAT_D16_UNORM, 
        const VkAttachmentDescription depthDesc {
            .flags          = 0 ,
            .format         = DEPTH_FORMAT, 
            .samples        = rp.SAMPLE_COUNT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        const VkAttachmentReference colorRef {
            .attachment = 0,
            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };
        const VkAttachmentReference depthRef {
            .attachment = 1,
            .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };
        
        const VkSubpassDescription subpassDesc {
            .flags                   = 0,
            .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount    = 0,
            .pInputAttachments       = nullptr,
            .colorAttachmentCount    = 1,
            .pColorAttachments       = &colorRef,
            .pResolveAttachments     = nullptr,
            .pDepthStencilAttachment = &depthRef,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments    = nullptr
        };

        const VkAttachmentDescription descs [] {
            colorDesc,
            depthDesc
        };

        //why is this needed??
        const VkSubpassDependency dependency {
            .srcSubpass     = VK_SUBPASS_EXTERNAL,
            .dstSubpass     = 0,
            .srcStageMask   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask  = 0,
            .dstAccessMask  = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        };        

        const VkRenderPassCreateInfo renderPassInfo {
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0,
            .attachmentCount = ARRAY_COUNT(descs),
            .pAttachments    = descs,
            .subpassCount    = 1,
            .pSubpasses      = &subpassDesc,
            .dependencyCount = 1,
            .pDependencies   = &dependency
        };

        VK_CHECK(vkCreateRenderPass(g_contextPtr->device, &renderPassInfo, nullptr, &rp.renderPass));

        //depth image
        rp.depthImage.Create(cmdPool, DEPTH_FORMAT);

        //? framebuffers
        for (u32 i = 0; i < g_contextPtr->swapImages.count; ++i)
        {
            rp.framebuffers.Append();

            const VkImageView views [] {
                g_contextPtr->swapImageViews[i],
                rp.depthImage.view
            };

            const VkFramebufferCreateInfo framebufferInfo{
                .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext           = nullptr,
                .flags           = 0,
                .renderPass      = rp.renderPass,
                .attachmentCount = ARRAY_COUNT(views),
                .pAttachments    = views,
                .width           = g_contextPtr->surfaceCapabilities.currentExtent.width,
                .height          = g_contextPtr->surfaceCapabilities.currentExtent.height,
                .layers          = 1
            };
            
            VK_CHECK(vkCreateFramebuffer(g_contextPtr->device, &framebufferInfo, nullptr, &rp.framebuffers[i]));
        }
    }
    

}//ns