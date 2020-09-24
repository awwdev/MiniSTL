//https://github.com/awwdev

#pragma once
#include "gpu/Vulkan/Meta/Context.hpp"

namespace rpg::gpu::vuk {

struct General_Wire_Shader
{
    VkShaderModule modules [2];
    VkPipelineShaderStageCreateInfo stageInfo [2];

    void Create()
    {
        CreateShaderModule(
            "res/Shaders/General/spv/generalWire.vert.spv", 
            VK_SHADER_STAGE_VERTEX_BIT,
            modules[0], stageInfo[0]);

        CreateShaderModule(
            "res/Shaders/General/spv/generalWire.frag.spv", 
            VK_SHADER_STAGE_FRAGMENT_BIT,
            modules[1], stageInfo[1]);
    }

    void Destroy()
    {
        FOR_CARRAY(modules, i)
            vkDestroyShaderModule(g_contextPtr->device, modules[i], nullptr);
        stageInfo[0] = stageInfo[1] = {};
    }
    ~General_Wire_Shader()
    {
        Destroy();
    }
};

}//ns