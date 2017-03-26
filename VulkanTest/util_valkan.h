#pragma once

#include <stdint.h>

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include "linmath.h"


typedef struct {
    vk::Image image;
    vk::CommandBuffer cmd;
    vk::CommandBuffer graphics_to_present_cmd;
    vk::ImageView view;
} SwapchainBuffers;


//Utility�ȑO�̏�ԁBValkan�֘A�̏����𕔕��؂�o�����B
namespace vkUtil {
    
    //ValidationLayer�𒲂ׂ�B
    //instance_validation_layers_alt1��T���Č�����ΐ����B����������Ȃ���΁A
    //instance_validation_layers_alt2��T���Č�����ΐ����B
    //�����Ȃ�ȉ����X�V����return true
    //[out] _enabled_layer_count �L���ȃ��C������Ԃ��B
    //[out] _enalble_layers[] �L���ȃ��C���̖��O������ւ̃|�C���^��Ԃ��B
    //���s�Ȃ�return false
    bool find_validation_layer(
        uint32_t &_enabled_layer_count, 
        const char *_enabled_layers[64]);
     

    //Instance Extension��(3��)���ׂ�B
    //[out] _enabled_extension_count �L����Extension�̐�0-3��Ԃ�
    //[out] _extension_names �L����Extension��(������)�ւ̃|�C���^��Ԃ�
    //[out] _surfaceExtFound VK_KHR_SURFACE_EXTENSION_NAME���L���Ȃ�True
    //[out] _platformSurfaceExtFound VK_KHR_WIN32_SURFACE_EXTENSION_NAME���L���Ȃ�True
    //[out] _debugReportExtFound VK_EXT_DEBUG_REPORT_EXTENSION_NAME���L���Ȃ�True
    //
    void find_instance_extensions(
        uint32_t &_enabled_extension_count,
        const char *_extension_names[64],
        bool & _surfaceExtFound,
        bool & _platformSurfaceExtFound,
        bool & _debugReportExtFound);


    //DebugReportCallback�̐ݒ�B
    void init_debug_report_callback(vk::Instance &inst);

    //�����f�o�C�X��񋓂���ڂ��擾����B
    //[in] inst
    //[out] gpu
    //return true ����
    bool find_first_physical_device(vk::Instance& inst, vk::PhysicalDevice& gpu);

    //Device Extension����swapchain�𒲂ׂ�B
    bool find_swapchain_in_device_extensions(
        vk::PhysicalDevice &phyDev,
        uint32_t &_enabled_extension_count,
        const char *_extension_names[64]);


    //TODO:�C���X�^���X�N���X�s���H
    //(Win32)�T�[�t�F�C�X�̐����B
    void createWin32Surface(vk::Instance &inst, HINSTANCE hinst, HWND hwnd, vk::SurfaceKHR &out_surface);

    //�O���t�B�b�N�L���[�t�@�~����T���B�������炳��ɁA
    //�O���t�B�b�N�ƃv���[���g�𗼕��T�|�[�g����L���[�t�@�~����T���B
    //������΁A�Z�p���[�g�v���[���g�L���[�t�@�~����T���B
    //[out] out_graphic_queue_family_index: if return UINT32_MAX then no graphic queue.
    //[out] out_present_queue_family_index : if return UINT32_MAX then no present queue.
    //[out] out_separete_present_queue    : if graphic and present queue are separated then return true.
    void findQueueFamilyIndeciesForGraphicsAndPresent(
        vk::PhysicalDevice &in_gpu,
        vk::SurfaceKHR &in_surface,
        int in_queue_family_count,
        const vk::QueueFamilyProperties *queue_family_propaties,
        uint32_t &out_graphic_queue_family_index,
        uint32_t &out_preset_queue_family_index,
        bool &out_separete_present_queue);


    // �i�_���j�f�o�C�X�̐����B
    // [in] in_gpu �����f�o�C�X(GPU)
    // [in] in_graphic_queue_family_index �O���t�B�b�N�L���[�t�@�~���̃C���f�b�N�X�B
    // [in] in_present_queue_family_index �v���[���g�L���[�t�@�~���̃C���f�b�N�X�B
    // [in] in_separete_present_queue �v���[���g�L���[���O���t�B�b�N�L���[�ƕʂ��ǂ����B
    // [in] in_enabled_extension_count �L���������g���@�\�̐��B
    // [in] in_extension_names�@�L���������g���@�\�̖��O�B
    // [out] out_device �������ꂽ�f�o�C�X�B
    void createDevice(
        vk::PhysicalDevice &in_gpu,
        uint32_t in_graphic_queue_family_index,
        uint32_t in_present_queue_family_index,
        bool in_separate_present_queue,
        uint32_t in_enabled_extension_count,
        const char *in_extension_names[64],
        vk::Device &out_device);


    // �f�o�C�X����L���[���擾�B
    void getQueue(
        vk::Device &in_device,
        uint32_t in_graphic_queue_family_index,
        uint32_t in_present_queue_family_index,
        bool in_separate_present_queue,
        vk::Queue &out_graphics_queue,
        vk::Queue &out_present_queue);



    //TODO: Surface�N���X�s���H
    //GPU�ƃT�[�t�F�C�X����
    //�T�[�t�F�X�t�H�[�}�b�g�ƃt�H�[�}�b�g�ƃJ���[�X�y�[�X���擾�B
    void getFormatAndColorSpace(
        vk::PhysicalDevice &in_gpu,
        vk::SurfaceKHR &in_surface,
        vk::Format &out_format,
        vk::ColorSpaceKHR &out_color_space);

    //�f�o�C�X����t���[�����O���̃t�F���X��3��̃Z�}�t�H�𐶐�����
    void createFencesAndSemaphores(
        vk::Device &in_device,
        int in_frame_lag,
        bool in_separate_present_queue,
        vk::Fence *out_fences,
        vk::Semaphore *out_semaphoresImageAcquired,
        vk::Semaphore *out_semaphoresDrawComplete,
        vk::Semaphore *out_semaphoresImageOwnershop
        );


    void createCommandPool(
        vk::Device &in_device,
        uint32_t in_queue_family_index,
        vk::CommandPool &out_cmd_pool);


    void allocateCommandBuffers(
        vk::Device &in_device,
        vk::CommandPool &in_cmd_pool,
        int in_swap_chain_image_count,
        SwapchainBuffers *out_swap_chain_buffers
        );

    void build_image_ownership_cmd(
        uint32_t in_graphic_queue_family_index,
        uint32_t in_present_queue_family_index,
        SwapchainBuffers &out_swapchain_buffer
        );

    void allocateCommandBuffersForPresent(
        vk::Device &in_device,
        vk::CommandPool &in_cmd_pool,
        int in_swap_chain_image_count,
        uint32_t in_graphic_queue_family_index,
        uint32_t in_present_queue_family_index,
        SwapchainBuffers *out_swap_chain_buffers
        );

    //TODO: Surface�N���X�s���H
    //GPU,�T�[�t�F�C�X����v���[���g���[�h�̃��X�g���擾�B
    void getSurfacePresentModesKHR(
        vk::PhysicalDevice &in_gpu,
        vk::SurfaceKHR &in_surface,
        std::vector<vk::PresentModeKHR> &out_presentModes);

    //TODO: Surface�N���X�s���H
    //GPU,�T�[�t�F�C�X����P�C�p�r���e�B�����擾�B
    void getSurfaceCapabilitiesKHR(
        vk::PhysicalDevice &in_gpu,
        vk::SurfaceKHR &in_surface,
        vk::SurfaceCapabilitiesKHR &out);


    //GPU,�T�[�t�F�C�X����v���[���g���[�h��I���B
    //�S��ނ��邪�A����̓f�t�H���g��FIFO���[�h�B
    vk::PresentModeKHR selectPresentMode(
        vk::PhysicalDevice &in_gpu,
        vk::SurfaceKHR &in_surface);

    //SwapChain�̃T�[�t�F�C�X�̃T�C�Y�����肷��B
    //Capabilities�ɒ�`����Ă���΂�������A
    //����`�Ȃ�current(desired)��width,height���̗p����B
    void determineSwapChainExtent(
        vk::SurfaceCapabilitiesKHR &in_surfCapabilities, 
        uint32_t &io_current_width,
        uint32_t &io_current_height,
        vk::Extent2D &out_swapchainExtent);

    //SwapChain�C���[�W�̐������肷��B
    uint32_t determineDesiredNumberOfSwapchainImages(
        vk::SurfaceCapabilitiesKHR &in_surfCapabilities);


    void createSwapchainKHR(
        vk::Device &in_device,
        vk::SurfaceCapabilitiesKHR &surfCapabilities,
        vk::PresentModeKHR swapchainPresentMode,
        vk::SurfaceKHR &in_surface,
        vk::Extent2D swapchainExtent,
        vk::Format format,
        vk::ColorSpaceKHR color_space,
        vk::SwapchainKHR &io_swapchain);

}