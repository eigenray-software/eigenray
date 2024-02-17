#pragma once

#include <device.hpp>

namespace er
{

struct Shader
{
	Device* device;
	VkShaderModule handle;
	Device* get_device() { return device; }

	Shader(Device* device, const std::vector<u32>& code);
};

}