#pragma once

#include <stdint.h>

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include "linmath.h"


//Valkan�֘A�֗̕��֐��B
namespace vkUtil {
    

    //ValidationLayer�𒲂ׂ�
    //instance_validation_layers_alt1��T���Č�����ΐ����B����������Ȃ���΁A
    //instance_validation_layers_alt2��T���Č�����ΐ����B
    //�����Ȃ�ȉ����X�V����return true
    //[out] _enabled_layer_count �L���ȃ��C������Ԃ��B
    //[out] _enalble_layers[] �L���ȃ��C���̖��O������ւ̃|�C���^��Ԃ��B
    //���s�Ȃ�return false
    bool find_validation_layer(
        uint32_t &_enabled_layer_count, 
        const char *_enabled_layers[64]);
     

    //Extension��(3��)���ׂ�
    //[out] _enabled_extension_count �L����Extension�̐�0-3��Ԃ�
    //[out] _extension_names �L����Extension��(������)�ւ̃|�C���^��Ԃ�
    //[out] _surfaceExtFound VK_KHR_SURFACE_EXTENSION_NAME���L���Ȃ�True
    //[out] _platformSurfaceExtFound VK_KHR_WIN32_SURFACE_EXTENSION_NAME���L���Ȃ�True
    //[out] _debugReportExtFound VK_EXT_DEBUG_REPORT_EXTENSION_NAME���L���Ȃ�True
    //
    void find_extensions(
        uint32_t &_enabled_extension_count,
        const char *_extension_names[64],
        bool & _surfaceExtFound,
        bool & _platformSurfaceExtFound,
        bool & _debugReportExtFound);



    void init_debug_report_callback(vk::Instance &inst);

}

