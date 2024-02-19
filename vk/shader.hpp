#pragma once

#include <device.hpp>

namespace er
{

struct Type
{
    struct Field
    {
        std::string name;
        rc<Type> type;
        u32 offset;
        u32 size;
    };

    union
    {
        // Scalars, vectors and matrices
        struct
        {
            u32 bit_width, columns, rows;
        };

        // Structs
        struct
        {
            std::string name;
            std::vector<Field> fields;
        };

        // Images
        struct 		
        {
			u32 dimension;
			bool depth;
			bool arrayed;
			bool ms;
			bool sampled;
            bool read;
            bool write;
			u32 format;
		};
    };

    enum
    {
        NONE,

        VOID,

        BOOL,
        INT,
        UINT,
        FLOAT,

        STRUCT,

        IMAGE,
        SAMPLER,
        SAMPLED_IMAGE,

        ATOMIC_COUNTER,
        ACCELERATION_STRUCTURE,
        RAY_QUERY,
    } tag = NONE;

    u32 type_size = 0;
    u32 array_stride = 0;
    u32 known_array_size = 1;
    std::vector<u32> array_dependent_ids;
    
    Type(struct ShaderParser *parser, u32 type_id);
    ~Type();
};


struct ShaderModule
{
    struct Binding
    {
        std::string name;
        u32 set;
        u32 binding;
        VkDescriptorType descriptorType;
        rc<Type> type;
    };

    struct EntryPoint
    {
        std::string name;
        VkShaderStageFlagBits stage;
    };

	Device* device;
	VkShaderModule handle;
    std::vector<Binding> bindings;
    std::vector<EntryPoint> entry_points;
    std::map<u32, rc<Type>> spec_constants;

	Device* get_device() { return device; }
	ShaderModule(Device* device, const std::vector<u8>& code);
    void parse_blob(std::vector<u8> const& blob);
};

struct SpecializedShader
{
    rc<ShaderModule> module;
    std::map<u32, u64> specialization_data;
    SpecializedShader(rc<ShaderModule> module);
};

}