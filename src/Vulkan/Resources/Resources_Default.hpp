//https://github.com/awwdev

#pragma once
#include "Resources/HostResources.hpp"
#include "Vulkan/Core.hpp"
#include "Vulkan/Context.hpp"

#include "Vulkan/Factories/Default/Default_RenderPass.hpp"
#include "Vulkan/Factories/Default/Default_PipelineVertexColor.hpp"
#include "Vulkan/Factories/Default/Default_PipelineTexture.hpp"
#include "Vulkan/Factories/Default/Default_PipelineShadow.hpp"
#include "Vulkan/Factories/Default/Default_Shader.hpp" //includes shadow
#include "Vulkan/Factories/Default/Default_VertexBuffer.hpp"
#include "Vulkan/Factories/Default/Default_UniformBuffer.hpp"

namespace rpg::vk {

struct Resources_Common
{
    RenderPass  renderPass;
    ImageArray  textures;

    Shader      shaderVertexColor;
    Shader      shaderTexture;
    Shader      shaderShadow;

    Pipeline    pipelineVertexColor;
    Pipeline    pipelineTexture;
    Pipeline    pipelineShadow;

    UniformBuffer_Groups<gpu::Default_UniformData, gpu::DEFAULT_UBO_MAX_COUNT> ubo;
    VertexBuffer<use::Common_Vertex, gpu::DEFAULT_VERTEX_MAX_COUNT> vbo;

    void Create(res::HostResources& hostRes, VkCommandPool cmdPool, Resources_Shadow&)
    {
        textures.Create(hostRes.textures.common, cmdPool);

        Default_CreateVertexBuffer          (vbo, cmdPool, hostRes);
        Default_CreateUniformBuffer         (ubo);
        //Default_CreateShaderVertexColor     (shaderVertexColor, shadow.renderPass);
        //Default_CreateShaderTexture         (shaderTexture, shadow.renderPass, textures);
        //Default_CreateShaderShadow          (shaderShadow);
        Default_CreateRenderPass            (renderPass, cmdPool);
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
        Default_CreateRenderPass            (renderPass, cmdPool);
        //Default_CreatePipelineVertexColor   (pipelineVertexColor, shaderVertexColor, renderPass, vbo, ubo);
        //Default_CreatePipelineTexture       (pipelineTexture, shaderTexture, renderPass, vbo, ubo);
        //Default_CreatePipelineShadow        (pipelineShadow, shaderShadow, shadow.renderPass, vbo, ubo);
    }
};

}//NS