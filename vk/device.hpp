#pragma once

#include <vkl.h>
#include <defines.hpp>
#include <vector>
#include <map>
#include <set>
#include <iostream>

namespace er
{

struct Device : VklDeviceFunctions
{
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memoryProperties;

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    std::map<VkFormat, VkFormatProperties> formatProperties;

    VkFormatProperties get_format_properties(VkFormat format);
    std::set<std::string> get_device_extensions();
    Device(VkPhysicalDevice physicalDevice);
};

struct DeviceChild
{
    Device* device;
    Device* get_device() { return device; }
    DeviceChild(Device* device) : device(device) {}
};

}