#include <shader.hpp>
#include <spirv_cross.hpp>

namespace er
{

ShaderModule::ShaderModule(Device* device, const std::vector<u8>& code) : device(device)
{
    VkShaderModuleCreateInfo shaderInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = (u32*)code.data(),
    };
    device->CreateShaderModule(&shaderInfo, 0, &handle);
    parse_blob(code);
}

struct TypeComparator
{
    using is_transparent = void;
    bool operator()(rc<Type> const& a, rc<Type> const& b) const
	{
        return 1; // a->id < b->id;
	}
};

VkShaderStageFlagBits execution_model_to_shader_stage(spv::ExecutionModel model)
{
    switch(model) 
    {
    case spv::ExecutionModelVertex: return VK_SHADER_STAGE_VERTEX_BIT;
    case spv::ExecutionModelTessellationControl: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    case spv::ExecutionModelTessellationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    case spv::ExecutionModelGeometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
    case spv::ExecutionModelFragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
    case spv::ExecutionModelGLCompute: return VK_SHADER_STAGE_COMPUTE_BIT;
    case spv::ExecutionModelTaskEXT:
    case spv::ExecutionModelTaskNV: return VK_SHADER_STAGE_TASK_BIT_EXT;
    case spv::ExecutionModelMeshEXT:
    case spv::ExecutionModelMeshNV: return VK_SHADER_STAGE_MESH_BIT_EXT;
    case spv::ExecutionModelRayGenerationKHR: return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    case spv::ExecutionModelIntersectionKHR: return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
    case spv::ExecutionModelAnyHitKHR: return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
    case spv::ExecutionModelClosestHitKHR: return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    case spv::ExecutionModelMissKHR: return VK_SHADER_STAGE_MISS_BIT_KHR;
    case spv::ExecutionModelCallableKHR: return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
    case spv::ExecutionModelKernel: 
    default:
        return VkShaderStageFlagBits(0);
    };
}

struct ShaderParser : spirv_cross::Compiler
{
    using spirv_cross::Compiler::Compiler;
    std::map<spirv_cross::TypeID, rc<Type>> types;

    rc<Type> get_or_build_type(spirv_cross::TypeID id)
    {
        get_type(id);
        auto it = types.find(id);
        if(it != types.end())
            return it->second;
        return types[id] = std::make_shared<Type>(this, id);;
    }
};

void ShaderModule::parse_blob(std::vector<u8> const& blob)
{
    ShaderParser cc((u32*)blob.data(), blob.size() >> 2);
    
    for (auto& [name, model] : cc.get_entry_points_and_stages())
    {
        entry_points.emplace_back(EntryPoint{ name, execution_model_to_shader_stage(model) });
    }

    auto used_variables = cc.get_active_interface_variables();
    auto resources = cc.get_shader_resources(used_variables);

    for(auto& constant : cc.get_specialization_constants())
    {
        spec_constants[constant.constant_id] = cc.get_or_build_type(cc.get_constant(constant.id).constant_type);
    }
    
    std::map<spirv_cross::SmallVector<spirv_cross::Resource>*, VkDescriptorType> descriptors =
    {
       {&resources.uniform_buffers,  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER},
       {&resources.storage_buffers, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
       // {&resources.stage_inputs,},
       // {&resources.stage_outputs, },
       {&resources.subpass_inputs, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT},
       {&resources.storage_images, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE},
       {&resources.sampled_images, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE},
       {&resources.atomic_counters, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
       {&resources.acceleration_structures, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR},
       {&resources.gl_plain_uniforms, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER},
       // There can only be one push constant block,
       // but keep the vector in case this restriction is lifted in the future.
       // {&resources.push_constant_buffers, },
       // {&resources.shader_record_buffers, },
       // For Vulkan GLSL and HLSL source,
       // these correspond to separate texture2D and samplers respectively.
       {&resources.separate_images, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE},
       {&resources.separate_samplers, VK_DESCRIPTOR_TYPE_SAMPLER},
    };

    // SmallVector<BuiltInResource> builtin_inputs;
    // SmallVector<BuiltInResource> builtin_outputs;

    for (auto& [resources, descriptorType] : descriptors)
    {
        for (auto& res : *resources)
        {
            spirv_cross::SPIRType const& type = cc.get_type(res.type_id);
            ShaderModule::Binding binding = {};
            binding.name = cc.get_name(res.id);
            if (binding.name.empty())
                binding.name = cc.get_fallback_name(res.id);
            binding.type = cc.get_or_build_type(res.type_id);
            binding.set = cc.get_decoration(res.id, spv::DecorationDescriptorSet);
            binding.binding = cc.get_decoration(res.id, spv::DecorationBinding);
            binding.descriptorType = descriptorType;
            bindings.push_back(std::move(binding));
        }
    }
}

Type::Type(ShaderParser *parser, u32 type_id)
{
    memset(this, 0, offsetof(Type, tag));

    auto &ty = parser->get_type(type_id);
    
    for (int i = 0; i < ty.array.size(); ++i)
    {
        if (ty.array_size_literal[i])
        {
            known_array_size *= ty.array[i];
            continue;
        }
        // auto constant = parser->get_constant(ty.array[i]);;
        auto constant_id = parser->get_decoration(ty.array[i], spv::DecorationSpecId);
        array_dependent_ids.push_back(constant_id);
    };

    assert(ty.array_size_literal.size() == ty.array.size());
    
    
    switch (ty.basetype)
    {
    case spirv_cross::SPIRType::Void:
        tag = VOID;
        break;
    case spirv_cross::SPIRType::Boolean:
        tag = BOOL;
        break;
    case spirv_cross::SPIRType::UByte:
    case spirv_cross::SPIRType::UShort:
    case spirv_cross::SPIRType::UInt:
    case spirv_cross::SPIRType::UInt64:
        tag = UINT;
        break;
    case spirv_cross::SPIRType::SByte:
    case spirv_cross::SPIRType::Short:
    case spirv_cross::SPIRType::Int:
    case spirv_cross::SPIRType::Int64:
        tag = INT;
        break;
    case spirv_cross::SPIRType::AtomicCounter:
        tag = ATOMIC_COUNTER;
        break;
    case spirv_cross::SPIRType::Half:
    case spirv_cross::SPIRType::Float:
    case spirv_cross::SPIRType::Double:
        tag = FLOAT;
        break;
    case spirv_cross::SPIRType::Struct:
        tag = STRUCT;
        type_size = parser->get_declared_struct_size(ty);
        new (&name) std::string(parser->get_name(type_id));
        new (&fields) std::vector<Field>(ty.member_types.size());
        for (u32 i = 0; i < ty.member_types.size(); ++i)
        {
            u32 index = i;
            fields[i].type = parser->get_or_build_type(ty.member_types[index]);
            if(index < ty.member_type_index_redirection.size())
                index = ty.member_type_index_redirection[index];
            fields[i].name = parser->get_member_name(ty.self, index);
            fields[i].size = parser->get_declared_struct_member_size(ty, index);
            fields[i].offset = parser->type_struct_member_offset(ty, index);
        }
        break;
    case spirv_cross::SPIRType::Image:
    case spirv_cross::SPIRType::SampledImage:
        tag = IMAGE;
        dimension = ty.image.dim;
        depth = ty.image.depth;
        arrayed = ty.image.arrayed;
        ms = ty.image.ms;
        sampled = ty.image.sampled;
        format = ty.image.format;
        write = !parser->get_decoration(type_id, spv::DecorationNonWritable);
        read  = !parser->get_decoration(type_id, spv::DecorationNonReadable);
        // below works only for kernel execution mode
        // switch (ty.image.access)
        // {
        // case spv::AccessQualifierReadWrite:
        //     read = write = true;
        //     break;
        // case spv::AccessQualifierWriteOnly:
        //     write = true;
        //     break;
        // case spv::AccessQualifierReadOnly:
        //     read = true;
        //     break;
        // }
        break;
    }

    switch (ty.basetype)
    {
    case spirv_cross::SPIRType::Boolean:
    case spirv_cross::SPIRType::UByte:
    case spirv_cross::SPIRType::UShort:
    case spirv_cross::SPIRType::UInt:
    case spirv_cross::SPIRType::UInt64:
    case spirv_cross::SPIRType::SByte:
    case spirv_cross::SPIRType::Short:
    case spirv_cross::SPIRType::Int:
    case spirv_cross::SPIRType::Int64:
    case spirv_cross::SPIRType::AtomicCounter:
    case spirv_cross::SPIRType::Half:
    case spirv_cross::SPIRType::Float:
    case spirv_cross::SPIRType::Double:
        bit_width = ty.width;
        columns = ty.vecsize;
        rows = ty.columns;
        type_size = (bit_width >> 3) * columns * rows;
    default:
        break;
    }
}

Type::~Type()
{
    if (tag == STRUCT)
    {
        name.~basic_string();
        fields.~vector();
    }
    tag = NONE;
}


}