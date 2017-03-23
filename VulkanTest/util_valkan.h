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


    //(Win32)サーフェイスの生成。
    void createWin32Surface(vk::Instance &inst, HINSTANCE hinst, HWND hwnd, vk::SurfaceKHR &out_surface);

    //グラフィックキューファミリを探す。見つけたらさらに、
    //グラフィックとプレゼントを両方サポートするキューファミリを探す。
    //無ければ、セパレートプレゼントキューファミリを探す。
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


    // （論理）デバイスの生成。
    // [in] in_gpu 物理デバイス(GPU)
    // [in] in_graphic_queue_family_index グラフィックキューファミリのインデックス。
    // [in] in_present_queue_family_index プレゼントキューファミリのインデックス。
    // [in] in_separete_present_queue プレゼントキューがグラフィックキューと別かどうか。
    // [in] in_enabled_extension_count 有効化される拡張機能の数。
    // [in] in_extension_names　有効化される拡張機能の名前。
    // [out] out_device 生成されたデバイス。
    void createDevice(
        vk::PhysicalDevice &in_gpu,
        uint32_t in_graphic_queue_family_index,
        uint32_t in_present_queue_family_index,
        bool in_separate_present_queue,
        uint32_t in_enabled_extension_count,
        const char *in_extension_names[64],
        vk::Device &out_device);
}