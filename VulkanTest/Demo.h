#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <memory>


#include "util_valkan.h"


struct vkcube_vs_uniform {
    // Must start with MVP
    float mvp[4][4];
    float position[12 * 3][4];
    float color[12 * 3][4];
};

struct vktexcube_vs_uniform {
    // Must start with MVP
    float mvp[4][4];
    float position[12 * 3][4];
    float attr[12 * 3][4];
};

struct texture_object {
    vk::Sampler sampler;

    vk::Image image;
    vk::ImageLayout imageLayout;

    vk::MemoryAllocateInfo mem_alloc;
    vk::DeviceMemory mem;
    vk::ImageView view;

    int32_t tex_width;
    int32_t tex_height;
};


#define APP_NAME_STR_LEN 80

// Allow a maximum of two outstanding presentation operations.
#define FRAME_LAG 2


class Demo 
{
public:
    Demo();

    //#1
    void init(int argc, char** argv, const char* name);

    //#2
    void init_vk_swapchain(HINSTANCE inst, HWND wnd);

    //#3
    void prepare();

private:

    //call from init()
    void init_vk();

public:

    void cleanup();

    void destroy_texture_image(texture_object *tex_objs);

    void draw();

    void draw_build_cmd(vk::CommandBuffer commandBuffer);

    void flush_init_cmd();


    void prepare_buffers();

    void prepare_cube_data_buffer();

    void prepare_depth();

    void prepare_descriptor_layout();

    void prepare_descriptor_pool();

    void prepare_descriptor_set();

    void prepare_framebuffers();

    vk::ShaderModule prepare_fs();

    void prepare_pipeline();

    void prepare_render_pass();

    vk::ShaderModule prepare_shader_module(const void *code, size_t size);

    void prepare_texture_image(const char *filename, texture_object *tex_obj,
        vk::ImageTiling tiling,
        vk::ImageUsageFlags usage,
        vk::MemoryPropertyFlags required_props);

    void prepare_textures();

    vk::ShaderModule prepare_vs();

    char *read_spv(const char *filename, size_t *psize);

    void resize();

    void set_image_layout(vk::Image image, vk::ImageAspectFlags aspectMask,
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::AccessFlags srcAccessMask,
        vk::PipelineStageFlags src_stages,
        vk::PipelineStageFlags dest_stages);

    void update_data_buffer();

    bool loadTexture(const char *filename, uint8_t *rgba_data,
        vk::SubresourceLayout *layout, int32_t *width,
        int32_t *height);

    bool memory_type_from_properties(uint32_t typeBits,
        vk::MemoryPropertyFlags requirements_mask,
        uint32_t *typeIndex);

    void run();


private:
    HINSTANCE connection_;        // hInstance - Windows Instance
    HWND window;                 // hWnd - window handle
    char name_[APP_NAME_STR_LEN]; // Name to put on the window/icon

    //Validation
    bool validate_; //valiationするか否か。
    uint32_t enabled_layer_count_; //有効化レイヤー数。
    char const *enabled_layers_[64]; //有効化レイヤー名の配列。

    uint32_t enabled_extension_count;
    char const *extension_names[64];

    uint32_t queue_family_count;
    std::unique_ptr<vk::QueueFamilyProperties[]> queue_props;

    vk::SurfaceKHR surface;
    vk::Instance inst;
    vk::PhysicalDevice gpu;

    bool separate_present_queue;
    uint32_t graphics_queue_family_index;
    uint32_t present_queue_family_index;

    vk::Device device;
    vk::Queue graphics_queue;
    vk::Queue present_queue;

    vk::Format format;
    vk::ColorSpaceKHR color_space;

    vk::Fence fences[FRAME_LAG];
    vk::Semaphore image_acquired_semaphores[FRAME_LAG];
    vk::Semaphore draw_complete_semaphores[FRAME_LAG];
    vk::Semaphore image_ownership_semaphores[FRAME_LAG];

    vk::CommandPool cmd_pool;
    vk::CommandPool present_cmd_pool;


    bool prepared;
    bool use_staging_buffer;

    vk::PhysicalDeviceProperties gpu_props;
    vk::PhysicalDeviceMemoryProperties memory_properties;


    uint32_t width_;
    uint32_t height_;

    uint32_t swapchainImageCount;
    vk::SwapchainKHR swapchain;
    std::unique_ptr<SwapchainBuffers[]> buffers;
    uint32_t frame_index;


    struct {
        vk::Format format;
        vk::Image image;
        vk::MemoryAllocateInfo mem_alloc;
        vk::DeviceMemory mem;
        vk::ImageView view;
    } depth;

    static int32_t const texture_count = 1;
    texture_object textures[texture_count];

    struct {
        vk::Buffer buf;
        vk::MemoryAllocateInfo mem_alloc;
        vk::DeviceMemory mem;
        vk::DescriptorBufferInfo buffer_info;
    } uniform_data;

    vk::CommandBuffer cmd; // Buffer for initialization commands
    vk::PipelineLayout pipeline_layout;
    vk::DescriptorSetLayout desc_layout;
    vk::PipelineCache pipelineCache;
    vk::RenderPass render_pass;
    vk::Pipeline pipeline;

    mat4x4 projection_matrix;
    mat4x4 view_matrix;
    mat4x4 model_matrix;

    float spin_angle;
    float spin_increment;
    bool pause;

    vk::ShaderModule vert_shader_module;
    vk::ShaderModule frag_shader_module;

    vk::DescriptorPool desc_pool;
    vk::DescriptorSet desc_set;

    std::unique_ptr<vk::Framebuffer[]> framebuffers;

    bool quit;
    uint32_t curFrame;
    uint32_t frameCount;
    bool use_break;
    bool suppress_popups;

    uint32_t current_buffer;
};





