//https://github.com/awwdev

#pragma once
#include "res/_Old/CpuResources.hpp"

#include "gpu/Vulkan/Meta/Context.hpp"

#include "gpu/Vulkan/_Old/Factories/Default/Default_RenderPass.hpp"
#include "gpu/Vulkan/_Old/Factories/Default/Default_PipelineVertexColor.hpp"
#include "gpu/Vulkan/_Old/Factories/Default/Default_PipelineTexture.hpp"
#include "gpu/Vulkan/_Old/Factories/Default/Default_PipelineShadow.hpp"
#include "gpu/Vulkan/_Old/Factories/Default/Default_Shader.hpp" //includes shadow
#include "gpu/Vulkan/_Old/Factories/Default/Default_VertexBuffer.hpp"
#include "gpu/Vulkan/_Old/Factories/Default/Default_UniformBuffer.hpp"

namespace rpg::gpu::vuk {

struct Resources_Common
{
    RenderPassTest renderPass;
    ImageArray     textures;

    Shader         shaderVertexColor;
    Shader         shaderTexture;
    Shader         shaderShadow;

    Pipeline       pipelineVertexColor;
    Pipeline       pipelineTexture;
    Pipeline       pipelineShadow;

    UniformBuffer_Groups<gpu::Common_UniformData, gpu::DEFAULT_UBO_MAX_COUNT> ubo;
    VertexBufferOld<com::Common_Vertex, gpu::DEFAULT_VERTEX_MAX_COUNT> vbo;

    void Create(res::CpuResources& hostRes, VkCommandPool cmdPool, Resources_Shadow&)
    {
        textures.Create(hostRes.textures.common, cmdPool);

        Common_CreateVertexBuffer          (vbo, cmdPool, hostRes);
        Common_CreateUniformBuffer         (ubo);
        //Default_CreateShaderVertexColor     (shaderVertexColor, shadow.renderPass);
        //Default_CreateShaderTexture         (shaderTexture, shadow.renderPass, textures);
        //Default_CreateShaderShadow          (shaderShadow);
        Common_CreateRenderPass            (renderPass, cmdPool);
        //Default_CreatePipelineVertexColor   (pipelineVertexColor, shaderVertexColor, renderPass, vbo, ubo);
        //Default_CreatePipelineTexture       (pipelineTexture, shaderTexture, renderPass, vbo, ubo);
        //Default_CreatePipelineShadow        (pipelineShadow, shaderShadow, shadow.renderPass, vbo, ubo);
    }

    void Recreate(VkCommandPool cmdPool, Resources_Shadow&)
    {
        //shaderVertexColor.Clear();
        //shaderTexture.Clear();
        //pipelineVertexColor.~Pipeline();
        //pipelineTexture.~Pipeline();
        //pipelineShadow.~Pipeline();
        renderPass.Clear();

        //TODO: recreate whole shader is wrong, only sampler needs recreation (due to img resize)
        //Default_CreateShaderVertexColor     (shaderVertexColor, shadow.renderPass);
        //Default_CreateShaderTexture         (shaderTexture, shadow.renderPass, textures);
        Common_CreateRenderPass            (renderPass, cmdPool);
        //Default_CreatePipelineVertexColor   (pipelineVertexColor, shaderVertexColor, renderPass, vbo, ubo);
        //Default_CreatePipelineTexture       (pipelineTexture, shaderTexture, renderPass, vbo, ubo);
        //Default_CreatePipelineShadow        (pipelineShadow, shaderShadow, shadow.renderPass, vbo, ubo);
    }
};

}//NS