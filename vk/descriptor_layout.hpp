#pragma once

#include <device.hpp>

namespace er
{

struct DescriptorLayout
{
    VkDescriptorSetLayout handle;
    Device* device;

    DescriptorLayout(Device* device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
};

} // namespace er


