//https://github.com/awwdev

#pragma once
#include "mini/Vulkan/Core.hpp"
#include "mini/Vulkan/Context.hpp"

#include "mini/Box/Array.hpp"
#include "mini/Memory/Allocator.hpp"

#include <fstream>
#include <unordered_map>

//TODO: remove VkArray and make it an actual helper class namespace mini::box

namespace mini::vk
{
    struct Refs
    {
        VkDevice device;
        Refs(Context& context) : device { context.device } {;}
    };

    struct Shader : Refs
    {
        enum Type { Vertex, Fragment, ENUM_END };

        VkShaderModule modules [Type::ENUM_END] { nullptr };
        uint32_t stageCount = 0;

        box::Array<VkVertexInputBindingDescription, 10>   vertexBindings;
        box::Array<VkVertexInputAttributeDescription, 10> vertexAttributes;

        VkDescriptorPool descPool;
        box::Array<VkDescriptorSetLayoutBinding, 10> descSetLayoutBindings;
        VkArray<VkDescriptorSetLayout, 4> descSetLayouts;
        VkArray<VkDescriptorSet, 4> descSets;

        box::Array<VkSampler, 10> samplers;

        //box::Array<VkDescriptorImageInfo, 10> imageInfos;
        std::unordered_map<uint32_t, VkDescriptorImageInfo>  imageInfos;
        std::unordered_map<uint32_t, VkDescriptorBufferInfo> bufferInfos;


        inline void AddDescriptor(
            const uint32_t binding, 
            const VkDescriptorType type,
            const uint32_t descCount,
            const VkShaderStageFlags stage)
        {
            descSetLayoutBindings.Append(binding, type, descCount, stage, nullptr);
        }

        inline void AddImage(
            const uint32_t binding, 
            VkSampler sampler, 
            VkImageView view, 
            VkImageLayout layout)
        {
            imageInfos.emplace(binding, VkDescriptorImageInfo {sampler, view, layout});
        }

        inline void AddSampler()
        {
            const VkSamplerCreateInfo samplerInfo {
                .sType                  = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext                  = nullptr,
                .flags                  = 0,
                .magFilter              = VK_FILTER_NEAREST,//VK_FILTER_LINEAR,
                .minFilter              = VK_FILTER_NEAREST,//VK_FILTER_LINEAR, 
                .mipmapMode             = VK_SAMPLER_MIPMAP_MODE_NEAREST,//VK_SAMPLER_MIPMAP_MODE_LINEAR,
                .addressModeU           = VK_SAMPLER_ADDRESS_MODE_REPEAT, 
                .addressModeV           = VK_SAMPLER_ADDRESS_MODE_REPEAT, 
                .addressModeW           = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .mipLodBias             = 0, 
                .anisotropyEnable       = VK_FALSE, 
                .maxAnisotropy          = 16.0f, 
                .compareEnable          = VK_FALSE,
                .compareOp              = VK_COMPARE_OP_ALWAYS, 
                .minLod                 = 0,
                .maxLod                 = 0, 
                .borderColor            = VK_BORDER_COLOR_INT_OPAQUE_BLACK, 
                .unnormalizedCoordinates = VK_FALSE
            };
            samplers.Append();
            VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &samplers.Last()));
        }

        inline auto CreatePipelineInfo() 
        {
            VkArray<VkPipelineShaderStageCreateInfo, Type::ENUM_END> stages;

            FOR_CARRAY(modules, i)
            {
                if (modules[i] == nullptr) continue;
                
                stages[stages.count] =
                {
                    .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext  = nullptr,
                    .flags  = 0,
                    .stage  = [&]()
                    {
                        switch(i){
                            case Type::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
                            case Type::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;

                            default: return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
                        }
                    }(),
                    .module = modules[i],
                    .pName  = "main",
                    .pSpecializationInfo = nullptr 
                };

                stages.count++;
            }

            return stages;
        }

        inline void AddVertexBinding(
            const uint32_t binding, 
            const uint32_t stride, 
            const VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX)
        {
            vertexBindings.Append(binding, stride, rate);
        }

        inline void AddVertexAttribute(
            const uint32_t binding, 
            const uint32_t location, 
            const VkFormat format, 
            const uint32_t offset)
        {
            vertexAttributes.Append(location, binding, format, offset);
        }

        template<std::size_t BUFFER_CAPACITY = 10000>
        inline void Load(const Type type, chars_t path)
        {
            std::ifstream file(path, std::ios::ate | std::ios::binary);
            mini::Assert(file.is_open(), "cannot open shader file");

            const uint32_t size = file.tellg();
            auto ptrBuffer = mini::mem::ClaimBlock<char[BUFFER_CAPACITY]>();
            file.seekg(std::ios::beg);
            file.read(*ptrBuffer, size);

            const VkShaderModuleCreateInfo moduleInfo {
                .sType      = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .pNext      = nullptr,
                .flags      = 0,
                .codeSize   = size,
                .pCode      = reinterpret_cast<const uint32_t*>(*ptrBuffer)
            };

            VK_CHECK(vkCreateShaderModule(device, &moduleInfo, nullptr, &modules[type]));
            ++stageCount;
        }

        inline void WriteDescriptors(Context& context)
        {
            const VkDescriptorSetLayoutCreateInfo layoutSetInfo
            {
                .sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext          = nullptr,
                .flags          = 0,
                .bindingCount   = descSetLayoutBindings.Count(),
                .pBindings      = descSetLayoutBindings.Data()
            };

            descSetLayouts.count = context.swapImages.count;
            FOR_VK_ARRAY(descSetLayouts, i)
                VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutSetInfo, nullptr, &descSetLayouts[i]));

            VkArray<VkDescriptorPoolSize, 10> poolSizes;
            poolSizes.count = descSetLayoutBindings.Count();
            FOR_ARRAY(descSetLayoutBindings, i) {
                poolSizes[i] = {
                    .type = descSetLayoutBindings[i].descriptorType,
                    .descriptorCount = context.swapImages.count
                };
            }

            const VkDescriptorPoolCreateInfo poolInfo
             {
                .sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .pNext          = nullptr,
                .flags          = 0,
                .maxSets        = context.swapImages.count,
                .poolSizeCount  = poolSizes.count,
                .pPoolSizes     = poolSizes.data
            };
            VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descPool));

            const VkDescriptorSetAllocateInfo allocInfo 
            {
                .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .pNext              = nullptr,
                .descriptorPool     = descPool,
                .descriptorSetCount = descSetLayouts.count,
                .pSetLayouts        = descSetLayouts.data
            };
            descSets.count = descSetLayouts.count;
            VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, descSets.data));

            VkArray<VkWriteDescriptorSet, 4> writes { 0 };
            writes.count = context.swapImages.count;

            uint32_t idx = 0;
            for(auto i=0; i<context.swapImages.count; ++i)
            {
                for (uint32_t bindingNum = 0; bindingNum < descSetLayoutBindings.Count(); ++bindingNum)
                {
                    writes[idx] = {
                        .sType              = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .pNext              = nullptr,
                        .dstSet             = descSets[i],
                        .dstBinding         = bindingNum,
                        .dstArrayElement    = 0,
                        .descriptorCount    = 1,
                        .descriptorType     = descSetLayoutBindings[bindingNum].descriptorType,
                        .pImageInfo         = imageInfos.contains(bindingNum)  ? &imageInfos[bindingNum]  : nullptr,
                        .pBufferInfo        = bufferInfos.contains(bindingNum) ? &bufferInfos[bindingNum] : nullptr,
                        .pTexelBufferView   = nullptr
                    };
                    ++idx;
                }
            }
            vkUpdateDescriptorSets(device, writes.count, writes.data, 0, nullptr);
                 
        }

        ~Shader()
        {
            vkDestroyDescriptorPool(device, descPool, nullptr);

            FOR_CARRAY(modules, i) {
                if (modules[i] != nullptr)
                    vkDestroyShaderModule(device, modules[i], nullptr);
            }       
            
            FOR_ARRAY(samplers, i)          vkDestroySampler(device, samplers[i], nullptr);
            FOR_VK_ARRAY(descSetLayouts, i) vkDestroyDescriptorSetLayout(device, descSetLayouts[i], nullptr);
        }
    };

    //? FACTORIES

    inline void CreateShader_Default(Context& context, Shader& shader, Image& image)
    {
        shader.AddVertexBinding(0, sizeof(float) * 2);
        shader.AddVertexAttribute(0, 0, VK_FORMAT_R32_SFLOAT, sizeof(float) * 0);
        shader.AddVertexAttribute(0, 1, VK_FORMAT_R32_SFLOAT, sizeof(float) * 1);
        shader.AddDescriptor(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT);
        shader.AddSampler();
        shader.AddImage(0, shader.samplers[0], image.view, image.layout);
        shader.WriteDescriptors(context);
        shader.Load(Shader::Type::Vertex,   "res/Shaders/default.vert.spv");
        shader.Load(Shader::Type::Fragment, "res/Shaders/default.frag.spv");
    }

}//ns