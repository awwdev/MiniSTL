//https://github.com/awwdev

#pragma once
#include "Vulkan/Core.hpp"
#include "Vulkan/Context.hpp"
#include "Vulkan/Objects/Image.hpp"

#include "Box/Array.hpp"

namespace rpg::vk {

inline VkRenderPassBeginInfo CreateRenderPassBeginInfo(
    VkRenderPass  renderPass,
    VkFramebuffer framebuffer,
    const VkExtent2D& extent,
    const uint32_t clearValueCount = 0,
    const VkClearValue* clears = nullptr)
{
    return {
        .sType          = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext          = nullptr,
        .renderPass     = renderPass,
        .framebuffer    = framebuffer,
        .renderArea     = {
            .offset     = VkOffset2D {0, 0},
            .extent     = extent
        },
        .clearValueCount= clearValueCount,
        .pClearValues   = clears
    };
}

struct RenderPass
{
    VkRenderPass renderPass;
    box::Array<VkFramebuffer, 4> framebuffers;
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT; //!set in factory
    VkRenderPassBeginInfo beginInfo; //!set in factory
    uint32_t width, height;

    box::Optional<DepthImage> depthImage;
    box::Optional<MSAAImage>  msaaImage;

    ~RenderPass()
    {
        Clear();
    }

    void Clear()
    {
        vkDestroyRenderPass(g_contextPtr->device, renderPass, nullptr);
        FOR_ARRAY(framebuffers, i) 
            vkDestroyFramebuffer(g_contextPtr->device, framebuffers[i], nullptr);
        framebuffers.Clear(); //clear because dtor is called on recreation

        if(depthImage)
           depthImage->Clear();

        if (msaaImage)
            msaaImage->Clear();
            
    }

    VkRenderPassBeginInfo GetBeginInfo(
        const u32 framBufferIdx, 
        const u32 clearCount = 0, 
        const VkClearValue* clears = nullptr) 
    {
        return {
            .sType          = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext          = nullptr,
            .renderPass     = renderPass,
            .framebuffer    = framebuffers[framBufferIdx],
            .renderArea     = {
                .offset     = VkOffset2D {0, 0},
                .extent     = { width, height }
            },
            .clearValueCount= clearCount,
            .pClearValues   = clears
        };
    }
    
};

struct RenderPassDepth
{
    VkRenderPass renderPass;
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
    VkRenderPassBeginInfo beginInfo;
    uint32_t width, height;
    uint32_t layerCount;

    DepthImageArray depthImageArray; //should really be the image array we already have
    box::Array<VkImageView, 3>   imageViews;
    box::Array<VkFramebuffer, 3> framebuffers;

    VkRenderPassBeginInfo GetBeginInfo(
        const u32 layerIndex,
        const u32 clearCount = 0, 
        const VkClearValue* clears = nullptr) 
    {
        return {
            .sType          = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext          = nullptr,
            .renderPass     = renderPass,
            .framebuffer    = framebuffers[layerIndex],
            .renderArea     = {
                .offset     = VkOffset2D {0, 0},
                .extent     = { width, height }
            },
            .clearValueCount= clearCount,
            .pClearValues   = clears
        };
    }

    ~RenderPassDepth()
    {
        Clear();
    }

    void Clear()
    {
        vkDestroyRenderPass(g_contextPtr->device, renderPass, nullptr);

        FOR_ARRAY(imageViews, i)
            vkDestroyImageView(g_contextPtr->device, imageViews[i], nullptr);
        imageViews.Clear();

        FOR_ARRAY(framebuffers, i)
            vkDestroyFramebuffer(g_contextPtr->device, framebuffers[i], nullptr);
        framebuffers.Clear();

        depthImageArray.Clear();
    }
};

}//ns