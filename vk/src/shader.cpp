#include <shader.hpp>

namespace er
{

Shader::Shader(Device* device, const std::vector<u32>& code) : device(device)
{
    VkShaderModuleCreateInfo shaderInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size() * sizeof(u32),
        .pCode = code.data(),
    };
    device->CreateShaderModule(&shaderInfo, 0, &handle);
}

}