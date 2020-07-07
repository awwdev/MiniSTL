//https://github.com/awwdev

#pragma once
#include "mini/Vulkan/Objects/Shader.hpp"
#include "mini/Vulkan/Objects/UniformBuffer.hpp"
#include "mini/Vulkan/Objects/ImageArray.hpp"
#include "mini/Resources/HostResources.hpp"
#include "mini/Utils/Structs.hpp"

namespace mini::vk
{
    inline void CreateShader_Default(Shader& shader)
    {  
        shader.CreateShaderModule("res/Shaders/spv/default.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
        shader.CreateShaderModule("res/Shaders/spv/default.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
    }

}//ns