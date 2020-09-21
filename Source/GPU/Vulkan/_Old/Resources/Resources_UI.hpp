//https://github.com/awwdev

#pragma once

#include "GPU/Vulkan/Meta/Context.hpp"
#include "Resources/CpuResources.hpp"

#include "GPU/Vulkan/_Old/Factories/UI/UI_RenderPass.hpp"
#include "GPU/Vulkan/_Old/Factories/UI/UI_Pipeline.hpp"
#include "GPU/Vulkan/_Old/Factories/UI/UI_Shader.hpp"
#include "GPU/Vulkan/_Old/Factories/UI/UI_UniformBuffer.hpp"

namespace rpg::gpu::vuk {

struct Resources_UI
{
    ImageArray fontImages;

    UI_PushConstants pushConsts;
    RenderPass       renderPass;
    Shader           shader;
    Pipeline         pipeline;
    UniformBuffer_Groups<gpu::RenderData_GUI::UBO_Text, gpu::RenderData_GUI::UBO_GUI_TEXT_MAX> ubo; //one group only

    void Create(res::CpuResources& hostRes, VkCommandPool cmdPool)
    {
        fontImages.Create(hostRes.textures.monospaceFont, cmdPool);

        UI_CreateUniformBuffer    (ubo);
        UI_CreateShader           (shader, fontImages);
        UI_CreateRenderPass       (renderPass);
        UI_CreatePipeline         (pipeline, shader, renderPass, ubo);
    }

    void Recreate()
    {
        renderPass.Clear();
        pipeline.Clear();
        UI_CreateRenderPass       (renderPass);
        UI_CreatePipeline         (pipeline, shader, renderPass, ubo);
    }
};

}//NS