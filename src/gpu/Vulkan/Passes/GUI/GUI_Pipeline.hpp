//https://github.com/awwdev

#pragma once
#include "gpu/Vulkan/Context.hpp"
#include "gpu/Vulkan/Utils/Initializers.hpp"


#include "gpu/Vulkan/Passes/gui/GUI_RenderPass.hpp"
#include "gpu/Vulkan/Passes/gui/GUI_Shader.hpp"
#include "gpu/Vulkan/Passes/gui/GUI_Uniforms.hpp"

namespace rpg::gpu::vuk {

struct GUI_Pipeline
{
    VkPipeline pipeline;
    VkPipelineLayout layout;

    void Create(GUI_RenderPass& renderPass, GUI_Shader& shader, GUI_Uniforms& uniforms)
    {
        const auto vertexInput      = VertexInputInfoEmpty();
        const auto inputAssembly    = InputAssemblyDefault();
        const auto viewport         = Viewport(renderPass.width, renderPass.height);
        const auto scissor          = Scissor(renderPass.width, renderPass.height);
        const auto viewportState    = ViewportState(viewport, scissor);
        const auto rasterization    = Rasterization();
        const auto multisampling    = Multisampling();
        const auto depthStencil     = DepthStencil();
        const auto blendAttachment  = BlendAttachment(VK_TRUE);
        const auto blendState       = BlendState(blendAttachment);   
        
        const auto layoutInfo = PipelineLayoutInfo(&uniforms.descriptors.descSetLayout, 1);
        VkCheck(vkCreatePipelineLayout(g_contextPtr->device, &layoutInfo, nullptr, &layout));

        const VkGraphicsPipelineCreateInfo pipelineInfo
        {
            .sType                      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext                      = nullptr,
            .flags                      = 0,
            .stageCount                 = (uint32_t) array_extent(shader.stageInfo),
            .pStages                    = shader.stageInfo,
            .pVertexInputState          = &vertexInput,
            .pInputAssemblyState        = &inputAssembly,
            .pTessellationState         = nullptr,
            .pViewportState             = &viewportState,
            .pRasterizationState        = &rasterization,
            .pMultisampleState          = &multisampling,
            .pDepthStencilState         = &depthStencil,
            .pColorBlendState           = &blendState,
            .pDynamicState              = nullptr,
            .layout                     = layout,
            .renderPass                 = renderPass.renderPass,
            .subpass                    = 0,
            .basePipelineHandle         = VK_NULL_HANDLE,
            .basePipelineIndex          = -1
        };

        VkCheck(vkCreateGraphicsPipelines(g_contextPtr->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));

    }

    void Destroy()
    {
        vkDestroyPipeline(g_contextPtr->device, pipeline, nullptr);
        vkDestroyPipelineLayout(g_contextPtr->device, layout, nullptr);
    }

    ~GUI_Pipeline()
    {
        Destroy();
    }
};

}//ns