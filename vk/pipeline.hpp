#pragma once

#include <shader.hpp>

namespace er
{

struct Pipeline
{
	Device* device;
	VkPipeline handle;
	Device* get_device() { return device; }
	Pipeline(Device* device);
};

}