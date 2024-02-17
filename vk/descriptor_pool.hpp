#pragma once
#include <device.hpp>

namespace er
{

struct DescriptorPool
{
    Device* device;
    VkDescriptorPool handle;
    Device* get_device() { return device; }
    DescriptorPool(Device* device, VkDescriptorPool handle) : device(device), handle(handle) {}
};

struct DescriptorSet
{
    Device* device;
    VkDescriptorSet handle;
    DescriptorSet(Device* device, VkDescriptorSet handle) : device(device), handle(handle) {}
};

struct DescriptorSetLayout
{
    Device* device;
    VkDescriptorSetLayout handle;
    Device* get_device() { return device; }
    DescriptorSetLayout(Device* device, VkDescriptorSetLayout handle) : device(device), handle(handle) {}
};

}