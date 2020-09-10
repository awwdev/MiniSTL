//https://github.com/awwdev

#pragma once
#include "Vulkan/Core.hpp"
#include "Vulkan/Context.hpp"
#include "Vulkan/Objects/RenderPass.hpp"

namespace rpg::vk {

inline void Default_CreateRenderPass(RenderPass& rp, VkCommandPool cmdPool)
{  
    rp.sampleCount = VK_SAMPLE_COUNT_4_BIT;
    rp.width  = g_contextPtr->surfaceCapabilities.currentExtent.width;
    rp.height = g_contextPtr->surfaceCapabilities.currentExtent.height;

    constexpr VkFormat DEPTH_FORMAT = VK_FORMAT_D32_SFLOAT;
    
    rp.msaaImage.t = {};
    rp.msaaImage.hasValue = true;
    rp.depthImage.t = {};
    rp.depthImage.hasValue = true;

    rp.msaaImage->Create(cmdPool, g_contextPtr->format, rp.sampleCount);
    rp.depthImage->Create(
        cmdPool, 
        DEPTH_FORMAT, 
        rp.width,
        rp.height,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        rp.sampleCount);


    const VkAttachmentDescription colorDesc {
        .flags          = 0 ,
        .format         = g_contextPtr->format, 
        .samples        = rp.sampleCount,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    const VkAttachmentDescription depthDesc {
        .flags          = 0 ,
        .format         = DEPTH_FORMAT, 
        .samples        = rp.sampleCount,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    const VkAttachmentDescription resolveDesc {
        .flags          = 0 ,
        .format         = g_contextPtr->format, 
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    const VkAttachmentReference colorRef {
        .attachment = 0,
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    const VkAttachmentReference depthRef {
        .attachment = 1,
        .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };
    const VkAttachmentReference resolveRef {
        .attachment = 2,
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    
    const VkSubpassDescription subpassDesc {
        .flags                   = 0,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount    = 0,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 1,
        .pColorAttachments       = &colorRef,
        .pResolveAttachments     = &resolveRef,
        .pDepthStencilAttachment = &depthRef,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments    = nullptr
    };

    const VkAttachmentDescription descs [] {
        colorDesc,
        depthDesc,
        resolveDesc
    };

    const VkSubpassDependency dependencies [] {
        {
            .srcSubpass      = VK_SUBPASS_EXTERNAL,
            .dstSubpass      = 0,
            .srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT,
            .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        },
        {
            .srcSubpass      = 0,
            .dstSubpass      = VK_SUBPASS_EXTERNAL,
            .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        },
    };     

    const VkRenderPassCreateInfo renderPassInfo {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .attachmentCount = ArrayCount(descs),
        .pAttachments    = descs,
        .subpassCount    = 1,
        .pSubpasses      = &subpassDesc,
        .dependencyCount = ArrayCount(dependencies),
        .pDependencies   = dependencies
    };

    VkCheck(vkCreateRenderPass(g_contextPtr->device, &renderPassInfo, nullptr, &rp.renderPass));

    //? framebuffers
    for (u32 i = 0; i < g_contextPtr->swapImages.count; ++i)
    {
        rp.framebuffers.Append();

        //const VkImageView views [] {
        //    g_contextPtr->swapImageViews[i],
        //    rp.depthImage.view,
        //};

        const VkImageView views [] {
            rp.msaaImage->view,
            rp.depthImage->view,
            g_contextPtr->swapImageViews[i],
        };

        const VkFramebufferCreateInfo framebufferInfo{
            .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext           = nullptr,
            .flags           = 0,
            .renderPass      = rp.renderPass,
            .attachmentCount = ArrayCount(views),
            .pAttachments    = views,
            .width           = rp.width,
            .height          = rp.height,
            .layers          = 1
        };
        
        VkCheck(vkCreateFramebuffer(g_contextPtr->device, &framebufferInfo, nullptr, &rp.framebuffers[i]));
    }
}

}//ns