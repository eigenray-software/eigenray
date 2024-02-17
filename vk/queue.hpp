#pragma once

#include <device.hpp>

namespace er
{

struct Queue : VklQueueFunctions
{
	u32 familyIndex;
	u32 index;
    
    Device* get_device() { return (Device*)fnptrs; }

    Queue(Device* device, u32 familyIndex, u32 index);
};

}