#pragma once
#include <queue.hpp>

namespace er
{

struct CommandPool 
{
	Queue* queue;
    VkCommandPool handle;

    Device* get_device() { return queue->get_device(); }
    CommandPool(Queue* queue);
};

}