#pragma once

#include <Device.hpp>

namespace er
{

struct Buffer
{
    Device* device;
    VkBuffer handle;
	VkDeviceMemory memory;
	VkDeviceSize size;
	VkBufferUsageFlags usage;
	VkMemoryPropertyFlags properties;

	Device* get_device() { return device; }

	~Buffer()
	{
		if(handle)
			get_device()->DestroyBuffer(handle, 0);
		if(memory)
			get_device()->FreeMemory(memory, 0);
	}

	Buffer(Buffer&& other) : device(other.device), handle(other.handle), memory(other.memory), size(other.size), usage(other.usage), properties(other.properties) 
	{
		other.handle = 0;
		other.memory = 0;
	}

	Buffer(const Buffer&) = delete;
	Buffer& operator=(const Buffer&) = delete;
	Buffer& operator=(Buffer&& other)
	{
		if (this != &other)
		{
			device = other.device;
			handle = other.handle;
			memory = other.memory;
			size = other.size;
			usage = other.usage;
			properties = other.properties;
			other.handle = 0;
			other.memory = 0;
		}
		return *this;
	}

    Buffer(Device* device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) : device(device), size(size), usage(usage), properties(properties)
    {
        VkBufferCreateInfo bufferInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = size,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		};
		device->CreateBuffer(&bufferInfo, 0, &handle);

		VkMemoryRequirements memoryRequirements;
		device->GetBufferMemoryRequirements(handle, &memoryRequirements);

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
		device->BindBufferMemory(handle, memory, 0);
	}
};

}