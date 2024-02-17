#pragma once

#include <device.hpp>

namespace er
{
VkFormatProperties Device::get_format_properties(VkFormat format)
{
    auto it = formatProperties.find(format);
    if (it != formatProperties.end())
        return it->second;

    VkFormatProperties formatProperty;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperty);
    formatProperties[format] = formatProperty;
    return formatProperty;
}

std::set<std::string> Device::get_device_extensions()
{
    u32 extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, 0, &extensionCount, 0);
    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, 0, &extensionCount, extensionProperties.data());
    std::set<std::string> extensions;
    for (auto &extension : extensionProperties)
        extensions.insert(extension.extensionName);
    return extensions;
}

Device::Device(VkPhysicalDevice physicalDevice) : physicalDevice(physicalDevice)
{
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    u32 queueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, 0);
    queueFamilyProperties.resize(queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

    std::vector<const char *> extensions = {};

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::vector<f32> priorities;

    for (u32 i = 0; i < queueFamilyProperties.size(); i++)
    {
        priorities.resize(queueFamilyProperties[i].queueCount, 1.0f);
    }

    for (u32 i = 0; i < queueFamilyProperties.size(); i++)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = i,
            .queueCount = queueFamilyProperties[i].queueCount,
            .pQueuePriorities = priorities.data(),
        };
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo deviceInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = (u32)queueCreateInfos.size(),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = (u32)extensions.size(),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = &features,
    };

    vkCreateDevice(physicalDevice, &deviceInfo, 0, &handle);
    vkl_load_device_functions(handle, this);
}
}