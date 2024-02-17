#pragma once

#include <image.hpp>

namespace er
{

Image::Image(Image&& other) : device(other.device), handle(other.handle), memory(other.memory), extent(other.extent), format(other.format), usage(other.usage), properties(other.properties)
{

}

Image::Image(Device* device, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) : device(device), extent(extent), format(format), usage(usage), properties(properties)
{
    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = 0,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = extent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    device->CreateImage(&imageInfo, 0, &handle);
    VkMemoryRequirements memoryRequirements;
    device->GetImageMemoryRequirements(handle, &memoryRequirements);
    VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = 0,
    };
    u32 memoryTypeIndex = 0;
    for (u32 i = 0; i < device->memoryProperties.memoryTypeCount; i++)
    {
        if ((memoryRequirements.memoryTypeBits & (1 << i)) && (device->memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            memoryTypeIndex = i;
            break;
        }
    }
    memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
    device->AllocateMemory(&memoryAllocateInfo, 0, &memory);
    device->BindImageMemory(handle, memory, 0);
}

}