//https://github.com/awwdev

#pragma once
#include "GPU/Vulkan/States/Shadow/State_Shadow.hpp"
#include "GPU/Vulkan/States/General/State_General.hpp"
#include "GPU/Vulkan/States/Post/State_Post.hpp"
#include "GPU/Vulkan/States/GUI/State_GUI.hpp"

#include "Resources/CpuResources.hpp"
#include "Common/ThreadPool.hpp"

namespace rpg::gpu::vuk {

struct States
{
    State_Shadow  shadow;
    State_General general;
    State_Post    post;
    State_GUI     gui;

    void Create(res::HostResources& hostRes, VkCommandPool cmdPool)
    {
        shadow  .Create(cmdPool);
        general .Create(cmdPool);
        post    .Create(cmdPool, general);
        gui     .Create(cmdPool, hostRes);
    }

    void Update(gpu::RenderData& renderData)
    {
        shadow  .Update(renderData);
        general .Update(renderData);
        post    .Update(renderData);
        gui     .Update(renderData);
    }

    com::Array<VkCommandBuffer, Commands::STATE_COUNT_MAX> Record(
    Commands& commands, const uint32_t cmdBufferIdx, com::ThreadPool<4>& threadPool)
    {
        constexpr auto SHADOW_STATE_IDX  = 0;
        constexpr auto GENERAL_STATE_IDX = 1;
        constexpr auto POST_STATE_IDX    = 2;
        constexpr auto GUI_STATE_IDX     = 3;

        const auto beginInfo = CommandBufferBeginInfo();

        com::Array<VkCommandBuffer, Commands::STATE_COUNT_MAX> cmds;
        VkCommandBuffer cmdBuffer = nullptr;

        cmdBuffer = cmds.Append(commands.stateCommands[SHADOW_STATE_IDX].cmdBuffers[cmdBufferIdx]);
        threadPool.AssignTask(0, [this, cmdBuffer, beginInfo, cmdBufferIdx]{
            VkCheck(vkBeginCommandBuffer(cmdBuffer, &beginInfo));
            shadow.Record(cmdBuffer);
            VkCheck(vkEndCommandBuffer(cmdBuffer));
        });
        
        cmdBuffer = cmds.Append(commands.stateCommands[GENERAL_STATE_IDX].cmdBuffers[cmdBufferIdx]);
        threadPool.AssignTask(1, [this, cmdBuffer, beginInfo, cmdBufferIdx]{
            VkCheck(vkBeginCommandBuffer(cmdBuffer, &beginInfo));
            general.Record(cmdBuffer);
            VkCheck(vkEndCommandBuffer(cmdBuffer));
        });
            
        cmdBuffer = cmds.Append(commands.stateCommands[POST_STATE_IDX].cmdBuffers[cmdBufferIdx]);
        threadPool.AssignTask(2, [this, cmdBuffer, beginInfo, cmdBufferIdx]{
            VkCheck(vkBeginCommandBuffer(cmdBuffer, &beginInfo));
            post.Record(cmdBuffer, cmdBufferIdx);
            VkCheck(vkEndCommandBuffer(cmdBuffer));
        });

        cmdBuffer = cmds.Append(commands.stateCommands[GUI_STATE_IDX].cmdBuffers[cmdBufferIdx]);
        threadPool.AssignTask(3, [this, cmdBuffer, beginInfo, cmdBufferIdx]{
            VkCheck(vkBeginCommandBuffer(cmdBuffer, &beginInfo));
            gui.Record(cmdBuffer, cmdBufferIdx);
            VkCheck(vkEndCommandBuffer(cmdBuffer));
        });

        threadPool.WaitForAllTasks();

        return cmds;
    }

    void Destroy()
    {
        shadow  .Destroy();
        general .Destroy();
        post    .Destroy();
        gui     .Destroy();
    }
};

} //ns