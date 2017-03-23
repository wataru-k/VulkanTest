#pragma once

#include <stdint.h>

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include "linmath.h"


//Valkan�֘A�֗̕��֐��B
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
}