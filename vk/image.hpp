#pragma once

#include <device.hpp>
#include <vec.hpp>

namespace er
{

struct Image
{
    enum Usage
    {
        TRANSFER_SRC = 1<<0,
        TRANSFER_DST = 1<<1,
        SAMPLED = 1<<2,
        STORAGE = 1<<3,
        COLOR_ATTACHMENT = 1<<4,
        DEPTH_STENCIL_ATTACHMENT = 1<<5,
        TRANSIENT_ATTACHMENT = 1<<6,
        INPUT_ATTACHMENT = 1<<7,
    };

    struct CreateInfo
    {
        row_vec<u32, 3> extent;
        VkFormat    format;
        Usage       usage;
    };
    
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