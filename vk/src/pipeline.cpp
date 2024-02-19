#include <pipeline.hpp>

namespace er
{

struct PipelineLayout
{
    Device* device;
    VkPipelineLayout handle;
    Device* get_device() { return device; }
    rc<ShaderModule> shader;
    
    PipelineLayout(Device* device, rc<ShaderModule> shader)
        : device(device)
        , shader(std::move(shader)) 
    {
        std::vector<VkDescriptorSetLayout> layouts;

        // for(auto& binding : shader->bindings)
        // {
        //     if(binding.type == Shader::Type::IMAGE)
        //     {
        //         VkDescriptorSetLayoutBinding layout = {
        //             .binding = binding.binding,
        //             .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        //             .descriptorCount = 1,
        //             .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        //             .pImmutableSamplers = 0,
        //         };
        //         layouts.push_back(layout);
        //     }
        // }
        
        VkPipelineLayoutCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = 0,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = 0,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = 0,
        };
    }
};

struct GraphicsPipeline : Pipeline
{
    GraphicsPipeline(Device* device, std::vector<rc<SpecializedShader>> const& shaders) : Pipeline(device)
    {
        std::map<VkShaderStageFlagBits, rc<SpecializedShader>> shader_per_stage;
        size_t scratch_size = 0;

        for(auto& shader : shaders)
		{
            for (auto& [entry, stage] : shader->module->entry_points)
            {
#define CHECK_STAGE(STAGE) \
                if(((STAGE) & stage) && !shader_per_stage.contains((STAGE))) \
                { \
                    shader_per_stage[(STAGE)] = shader; \
                    scratch_size += shader->specialization_data.size() ; \
                }
                CHECK_STAGE(VK_SHADER_STAGE_VERTEX_BIT);
                CHECK_STAGE(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
                CHECK_STAGE(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
                CHECK_STAGE(VK_SHADER_STAGE_GEOMETRY_BIT);
                CHECK_STAGE(VK_SHADER_STAGE_FRAGMENT_BIT);
#undef CHECK_STAGE
            }
		}

        std::vector<VkPipelineShaderStageCreateInfo> stages;
        std::vector<VkSpecializationInfo> spec_infos;
        std::vector<std::vector<VkSpecializationMapEntry>> map_entries;

        std::vector<u64> specialization_data(scratch_size);
        u64* contant_data = specialization_data.data();
        u64 offset = 0;

   //     for (auto& [stage, shader] : shader_per_stage)
   //     {
   //         for(auto& [entry, data] : shader->specialization_data)
			//{
			//	VkSpecializationMapEntry map_entry = {
			//		.constantID = entry,
			//		.offset = offset,
			//		.size   = type->size,
			//	};

   //             offset += 8;

			//	spec_info.dataSize += data.size();
			//	map_entries.push_back(map_entry);
			//}
   //     }

        for(auto& shader : shaders)
        {
            for(auto& [entry, stage] : shader->module->entry_points)
            {
                VkPipelineShaderStageCreateInfo stageInfo = {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = 0,
                    .flags = 0,
                    .stage = stage,
                    .module = shader->module->handle,
                    .pName = entry.c_str(),
                    .pSpecializationInfo = 0,
                };
                stages.push_back(stageInfo);
            }
        }
    }
};

Pipeline::Pipeline(Device* device) : device(device)
{

}

}