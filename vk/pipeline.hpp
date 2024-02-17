#pragma once

#include <device.hpp>

namespace er
{

struct Pipeline
{
	Device* device;
	VkPipeline handle;
	Device* get_device() { return device; }
	Pipeline(Device* device, VkPipeline handle) : device(device), handle(handle) {}
};

}