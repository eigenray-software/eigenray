#pragma once

#include <device.hpp>

namespace er
{

struct Instance
{
    VkInstance instance;
    std::vector<Device*> devices;

    static std::set<std::string> get_instance_layers();
    static std::set<std::string> get_instance_extensions();
	void load_library_and_init();
    Instance();
};
}