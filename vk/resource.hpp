#pragma once

#include <device.hpp>

namespace er
{

struct DeviceMemory
{
    struct CreateInfo
    {
        VkDeviceSize size;
        u32 memoryTypeIndex;
    };

    Device* device;


    // Each pNext member of any structure (including this one) in the pNext chain must be either NULL or a pointer to a valid instance of 
    // VkDedicatedAllocationMemoryAllocateInfoNV, 

    // VkExportMemoryAllocateInfo, 
    // VkExportMemoryAllocateInfoNV, 
    // VkExportMemoryWin32HandleInfoKHR, 
    // VkExportMemoryWin32HandleInfoNV, 
    // VkExportMetalObjectCreateInfoEXT, 

    // VkImportAndroidHardwareBufferInfoANDROID, 
    // VkImportMemoryBufferCollectionFUCHSIA, 
    // VkImportMemoryFdInfoKHR, 
    // VkImportMemoryHostPointerInfoEXT, 
    // VkImportMemoryWin32HandleInfoKHR, 
    // VkImportMemoryWin32HandleInfoNV, 
    // VkImportMemoryZirconHandleInfoFUCHSIA, 
    // VkImportMetalBufferInfoEXT, 
    // VkImportScreenBufferInfoQNX, 
    
    // VkMemoryAllocateFlagsInfo, 
    // VkMemoryDedicatedAllocateInfo, 
    // VkMemoryOpaqueCaptureAddressAllocateInfo, 
    // VkMemoryPriorityAllocateInfoEXT
    
    DeviceMemory(Device* device, CreateInfo const& info) : device(device)
    {
        VkMemoryAllocateInfo memoryAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = size,
            .memoryTypeIndex = memoryTypeIndex,
        };
        device->AllocateMemory(&memoryAllocateInfo, 0, &handle);
    }
};

struct Allocation
{
    VkDeviceMemory memory;
    VkDeviceSize offset;
    VkDeviceSize size;
    
};

}