#pragma once

#include <command_pool.hpp>

namespace er
{

struct CommandBuffer : VklCommandFunctions
{
	VkCommandBuffer handle;
    
	Device* get_device() { return (Device*)fnptrs; }
    CommandBuffer(CommandPool* pool);
};

}