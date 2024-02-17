#include <queue.hpp>

namespace er
{

Queue::Queue(Device* device, u32 familyIndex, u32 index) : VklQueueFunctions{ device }, familyIndex(familyIndex), index(index)
{
    get_device()->GetDeviceQueue(familyIndex, index, &handle);
}

}