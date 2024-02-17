#include <vk.hpp>
#include <windows.h>

using namespace er;

namespace er
{

std::set<std::string> Instance::get_instance_layers()
{
    u32 layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, 0);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());
    std::set<std::string> layers;
    for (auto& layer : layerProperties)
        layers.insert(layer.layerName);
    return layers;
}

std::set<std::string> Instance::get_instance_extensions()
{
    u32 extensionCount;
    vkEnumerateInstanceExtensionProperties(0, &extensionCount, 0);
    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateInstanceExtensionProperties(0, &extensionCount, extensionProperties.data());
    std::set<std::string> extensions;
    for (auto& extension : extensionProperties)
        extensions.insert(extension.extensionName);
    return extensions;
}


void Instance::load_library_and_init()
{
	vkl_init((PFN_vkGetInstanceProcAddr)GetProcAddress(LoadLibraryA("vulkan-1.dll"), "vkGetInstanceProcAddr"));
}

Instance::Instance()
{
    load_library_and_init();
    std::vector<const char*> layers = { 
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_KHRONOS_synchronization2",
    };
    std::vector<const char*> extensions = { "VK_KHR_surface", "VK_KHR_win32_surface" };
    
    std::set<std::string> availableLayers = get_instance_layers();
    std::set<std::string> availableExtensions = get_instance_extensions();

    for (auto& layer : layers)
    {
        if (availableLayers.find(layer) == availableLayers.end())
        {
            std::cerr << "Layer not available: " << layer << "\n";
            ER_ASSERT(false && "Layer not available");
        }
    }

    for (auto& ext : extensions)
    {
        if (availableExtensions.find(ext) == availableExtensions.end())
        {
            std::cerr << "Extension not available: " << ext << "\n";
            ER_ASSERT(false && "Extension not available");
        }
    }

    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "eigenray",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "eigenray",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo instanceInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount   = (u32)layers.size(),
        .ppEnabledLayerNames = layers.data(),
    };

    vkCreateInstance(&instanceInfo, 0, &instance);
    vkl_load_instance_functions(instance);
    
    u32 deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, 0);
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    for (auto physicalDevice : physicalDevices)
    {
        Device* device = new Device(physicalDevice);
        devices.push_back(device);
    }
}

}
