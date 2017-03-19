#include "util_valkan.h"


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#ifdef _DEBUG
#define VERIFY(x) assert(x)
#else
#define VERIFY(x) ((void)(x))
#endif


//
//returns VK_TRUE when All check_names are found
//        0       when All check_names are NOT found
static vk::Bool32
check_layers(
    uint32_t check_count,
    char const *const *const check_names,
    uint32_t layer_count,
    vk::LayerProperties *layers)
{
    for (uint32_t i = 0; i < check_count; i++) {
        vk::Bool32 found = VK_FALSE;
        for (uint32_t j = 0; j < layer_count; j++) {
            if (!strcmp(check_names[i], layers[j].layerName)) {
                found = VK_TRUE;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "Cannot find layer: %s\n", check_names[i]);
            return 0;
        }
    }
    return VK_TRUE;
}

//ValidationLayerを調べる
//instance_validation_layers_alt1を探して見つかれば成功
//見つからなければ
//instance_validation_layers_alt2を探して見つかれば成功
//成功なら以下を更新してreturn true
//_enabled_layer_count
//_enalble_layers[]
//失敗ならreturn false
bool
vkUtil::find_validation_layer(
    uint32_t &_enabled_layer_count,
    const char *_enabled_layers[64]
    )
{
    for (int i = 0; i < 64; i++) {
        _enabled_layers[i] = nullptr;
    }

    uint32_t instance_layer_count = 0;
    uint32_t validation_layer_count = 0;

    static char const *const instance_validation_layers_alt1[] = {
        "VK_LAYER_LUNARG_standard_validation" };

    static char const *const instance_validation_layers_alt2[] = {
        "VK_LAYER_GOOGLE_threading",
        "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_image",
        "VK_LAYER_LUNARG_core_validation",
        "VK_LAYER_LUNARG_swapchain",
        "VK_LAYER_GOOGLE_unique_objects" };

    // Look for validation layers
    char const *const *  instance_validation_layers = nullptr;

    vk::Bool32 validation_found = VK_FALSE;

    auto result = vk::enumerateInstanceLayerProperties(&instance_layer_count, nullptr);
    VERIFY(result == vk::Result::eSuccess);

    instance_validation_layers = instance_validation_layers_alt1;

    if (instance_layer_count > 0) {
        std::unique_ptr<vk::LayerProperties[]>
            instance_layers(new vk::LayerProperties[instance_layer_count]);

        result = vk::enumerateInstanceLayerProperties(&instance_layer_count, instance_layers.get());
        VERIFY(result == vk::Result::eSuccess);

        validation_found =
            check_layers(
                ARRAY_SIZE(instance_validation_layers_alt1),
                instance_validation_layers,
                instance_layer_count, instance_layers.get());

        if (validation_found) {
            _enabled_layer_count =
                ARRAY_SIZE(instance_validation_layers_alt1);
            _enabled_layers[0] = "VK_LAYER_LUNARG_standard_validation";
            validation_layer_count = 1;
        }
        else {
            // use alternative set of validation layers
            instance_validation_layers =
                instance_validation_layers_alt2;
            _enabled_layer_count =
                ARRAY_SIZE(instance_validation_layers_alt2);
            validation_found = check_layers(
                ARRAY_SIZE(instance_validation_layers_alt2),
                instance_validation_layers, instance_layer_count,
                instance_layers.get());
            validation_layer_count =
                ARRAY_SIZE(instance_validation_layers_alt2);
            for (uint32_t i = 0; i < validation_layer_count; i++) {
                _enabled_layers[i] = instance_validation_layers[i];
            }
        }
    }

    return (bool)validation_found;
}


//Extensionを(3つ)調べる
//[out] _enabled_extension_count 有効なExtensionの数0-3を返す
//[out] _extension_names 有効なExtension名(文字列)へのポインタを返す
//[out] _surfaceExtFound VK_KHR_SURFACE_EXTENSION_NAMEが有効ならTrue
//[out] _platformSurfaceExtFound VK_KHR_WIN32_SURFACE_EXTENSION_NAMEが有効ならTrue
//[out] _debugReportExtFound VK_EXT_DEBUG_REPORT_EXTENSION_NAMEが有効ならTrue
//
void
vkUtil::find_extensions(
    uint32_t &_enabled_extension_count,
    const char *_extension_names[64],
    bool & _surfaceExtFound,
    bool & _platformSurfaceExtFound,
    bool & _debugReportExtFound)
{
    uint32_t instance_extension_count = 0;

    for (int i = 0; i < 64; i++) {
        _extension_names[i] = nullptr;
    }

    auto result = vk::enumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr);
    VERIFY(result == vk::Result::eSuccess);

    if (instance_extension_count > 0) {
        std::unique_ptr<vk::ExtensionProperties[]> instance_extensions(
            new vk::ExtensionProperties[instance_extension_count]);

        result = vk::enumerateInstanceExtensionProperties(
            nullptr, &instance_extension_count, instance_extensions.get());
        VERIFY(result == vk::Result::eSuccess);

        for (uint32_t i = 0; i < instance_extension_count; i++) {
            if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME,
                instance_extensions[i].extensionName)) {
                _surfaceExtFound = true;
                _extension_names[_enabled_extension_count++] =
                    VK_KHR_SURFACE_EXTENSION_NAME;
            }
            else if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                instance_extensions[i].extensionName)) {
                _platformSurfaceExtFound = true;
                _extension_names[_enabled_extension_count++] =
                    VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
            }
            else if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
                instance_extensions[i].extensionName)) {
                _debugReportExtFound = true;
                _extension_names[_enabled_extension_count++] =
                    VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
            }
            assert(_enabled_extension_count < 64);
        }
    }
}


VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
    VkDebugReportFlagsEXT       flags,
    VkDebugReportObjectTypeEXT  objectType,
    uint64_t                    object,
    size_t                      location,
    int32_t                     messageCode,
    const char*                 pLayerPrefix,
    const char*                 pMessage,
    void*                       pUserData)
{
    printf("Error! MyDebugReportCallback : %s\n", pMessage);
    return VK_FALSE;
}

void vkUtil::init_debug_report_callback(vk::Instance &instance)
{

    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
        reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>
        (instance.getProcAddr("vkCreateDebugReportCallbackEXT"));

#if 0
    PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT =
        reinterpret_cast<PFN_vkDebugReportMessageEXT>
        (instance.getProcAddr("vkDebugReportMessageEXT"));

    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT =
        reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>
        (instance.getProcAddr("vkDestroyDebugReportCallbackEXT"));
#endif


    VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
    callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    callbackCreateInfo.pNext = nullptr;
    callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    callbackCreateInfo.pfnCallback = &MyDebugReportCallback;
    callbackCreateInfo.pUserData = nullptr;

    /* Register the callback */
    VkDebugReportCallbackEXT callback;
    VkResult result = vkCreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &callback);

    //vk::DebugReportCallbackCreateInfoEXT().setSType(eDebugReportCallbackCreateInfoEXT)

}
