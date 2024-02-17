#include <vec.hpp>
#include <vk.hpp>

#include <iostream>
#include <Windows.h>

#include <vector>
#include <map>
#include <set>

using namespace er;


int main()
{
    er::vec<f32, 4> v = {1, 2, 3, 4};
    std::cout << ((v, v, v, v)) << "\n";
    std::cout << ((v * (v, v, v, v)) * v) << "\n";
    std::cout << (v * ((v, v, v, v) * v)) << "\n";

    Instance instance;
    auto vk = instance.devices[0];
    Buffer buf(vk, 1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    return 0;
}