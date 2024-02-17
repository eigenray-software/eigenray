#pragma once

#include <command_buffer.hpp>

namespace er
{

CommandBuffer::CommandBuffer(CommandPool* pool) : VklCommandFunctions{pool->get_device()}
{
    VkCommandBufferAllocateInfo commandBufferInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool->handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    get_device()->AllocateCommandBuffers(&commandBufferInfo, &handle);
}

}