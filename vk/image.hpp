#pragma once

#include <device.hpp>

namespace er
{

struct Image
{
    Device* device;
	VkImage handle;
	VkDeviceMemory memory;
	VkExtent3D extent;
	VkFormat format;
	VkImageUsageFlags usage;
	VkMemoryPropertyFlags properties;
    
	Device* get_device() { return device; }

	Image(Image&& other);
    Image(Device* device, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
};

}