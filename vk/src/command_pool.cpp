#include <command_pool.hpp>

namespace er
{

CommandPool::CommandPool(Queue* queue) : queue(queue)
{
    VkCommandPoolCreateInfo commandPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = queue->familyIndex,
    };
    queue->get_device()->CreateCommandPool(&commandPoolInfo, 0, &handle);
}

}
