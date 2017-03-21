#pragma once

#include <stdint.h>

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include "linmath.h"


//Valkan関連の便利関数。
namespace vkUtil {
    

    //ValidationLayerを調べる。
    //instance_validation_layers_alt1を探して見つかれば成功。もし見つからなければ、
    //instance_validation_layers_alt2を探して見つかれば成功。
    //成功なら以下を更新してreturn true
    //[out] _enabled_layer_count 有効なレイヤ数を返す。
    //[out] _enalble_layers[] 有効なレイヤの名前文字列へのポインタを返す。
    //失敗ならreturn false
    bool find_validation_layer(
        uint32_t &_enabled_layer_count, 
        const char *_enabled_layers[64]);
     

    //Instance Extensionを(3つ)調べる。
    //[out] _enabled_extension_count 有効なExtensionの数0-3を返す
    //[out] _extension_names 有効なExtension名(文字列)へのポインタを返す
    //[out] _surfaceExtFound VK_KHR_SURFACE_EXTENSION_NAMEが有効ならTrue
    //[out] _platformSurfaceExtFound VK_KHR_WIN32_SURFACE_EXTENSION_NAMEが有効ならTrue
    //[out] _debugReportExtFound VK_EXT_DEBUG_REPORT_EXTENSION_NAMEが有効ならTrue
    //
    void find_instance_extensions(
        uint32_t &_enabled_extension_count,
        const char *_extension_names[64],
        bool & _surfaceExtFound,
        bool & _platformSurfaceExtFound,
        bool & _debugReportExtFound);


    //DebugReportCallbackの設定。
    void init_debug_report_callback(vk::Instance &inst);

    //物理デバイスを列挙し一つ目を取得する。
    //[in] inst
    //[out] gpu
    //return true 成功
    bool find_first_physical_device(vk::Instance& inst, vk::PhysicalDevice& gpu);

    //Device Extensionからswapchainを調べる。
    bool find_swapchain_in_device_extensions(
        vk::PhysicalDevice &phyDev,
        uint32_t &_enabled_extension_count,
        const char *_extension_names[64]);

}