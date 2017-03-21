#include "util_valkan.h"


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#ifdef _DEBUG
#define VERIFY(x) assert(x)
#else
#define VERIFY(x) ((void)(x))
#endif

template<typename T> class TempArray {
private:
    std::unique_ptr<T[]> ptr_;
public:
    TempArray<T>(int count) { ptr_.reset(new T[count]); }
    T* get() { return ptr_.get(); }
    T operator [](int i) const { return ptr_[i]; }
};

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

//ValidationLayer�𒲂ׂ�
//instance_validation_layers_alt1��T���Č�����ΐ���
//������Ȃ����
//instance_validation_layers_alt2��T���Č�����ΐ���
//�����Ȃ�ȉ����X�V����return true
//_enabled_layer_count
//_enalble_layers[]
//���s�Ȃ�return false
bool
vkUtil::find_validation_layer(
    uint32_t &_enabled_layer_count,
    const char *_enabled_layers[64]
    )
{
    for (int i = 0; i < 64; i++) {
        _enabled_layers[i] = nullptr;
    }

    uint32_t layer_count = 0;
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

    vk::Bool32 validation_found = VK_FALSE;

    auto result = vk::enumerateInstanceLayerProperties(&layer_count, nullptr);
    VERIFY(result == vk::Result::eSuccess);

    if (layer_count > 0) {
        TempArray<vk::LayerProperties> layers(layer_count);

        result = vk::enumerateInstanceLayerProperties(&layer_count, layers.get());
        VERIFY(result == vk::Result::eSuccess);

        validation_found = check_layers(
            ARRAY_SIZE(instance_validation_layers_alt1),
            instance_validation_layers_alt1,
            layer_count, layers.get());

        if (validation_found) {
            _enabled_layer_count =
                ARRAY_SIZE(instance_validation_layers_alt1);
            _enabled_layers[0] = instance_validation_layers_alt1[0];
        }
        else {
            // use alternative set of validation layers

            validation_found = check_layers(
                ARRAY_SIZE(instance_validation_layers_alt2),
                instance_validation_layers_alt2, 
                layer_count, layers.get());

            if (validation_found) {
                _enabled_layer_count =
                    ARRAY_SIZE(instance_validation_layers_alt2);
                for (uint32_t i = 0; i < ARRAY_SIZE(instance_validation_layers_alt2); i++) {
                    _enabled_layers[i] = instance_validation_layers_alt2[i];
                }
            }
        }
    }

    return validation_found ? true : false;
}


//Extension��(3��)���ׂ�
//[out] _enabled_extension_count �L����Extension�̐�0-3��Ԃ�
//[out] _extension_names �L����Extension��(������)�ւ̃|�C���^��Ԃ�
//[out] _surfaceExtFound VK_KHR_SURFACE_EXTENSION_NAME���L���Ȃ�True
//[out] _platformSurfaceExtFound VK_KHR_WIN32_SURFACE_EXTENSION_NAME���L���Ȃ�True
//[out] _debugReportExtFound VK_EXT_DEBUG_REPORT_EXTENSION_NAME���L���Ȃ�True
//
void
vkUtil::find_instance_extensions(
    uint32_t &_enabled_extension_count,
    const char *_extension_names[64],
    bool & _surfaceExtFound,
    bool & _platformSurfaceExtFound,
    bool & _debugReportExtFound)
{
    uint32_t extension_count = 0;

    for (int i = 0; i < 64; i++) {
        _extension_names[i] = nullptr;
    }
    _surfaceExtFound = false;
    _platformSurfaceExtFound = false;
    _debugReportExtFound = false;

    auto result = vk::enumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    VERIFY(result == vk::Result::eSuccess);

    if (extension_count > 0) {
        TempArray<vk::ExtensionProperties> extensions(extension_count);

        result = vk::enumerateInstanceExtensionProperties(
            nullptr, &extension_count, extensions.get());
        VERIFY(result == vk::Result::eSuccess);

        for (uint32_t i = 0; i < extension_count; i++) {
            if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME,
                extensions[i].extensionName)) {
                _surfaceExtFound = true;
                _extension_names[_enabled_extension_count++] =
                    VK_KHR_SURFACE_EXTENSION_NAME;
            }
            else if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                extensions[i].extensionName)) {
                _platformSurfaceExtFound = true;
                _extension_names[_enabled_extension_count++] =
                    VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
            }
            else if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
                extensions[i].extensionName)) {
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

    return VK_FALSE; /* not abort */
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



bool vkUtil::find_first_physical_device(vk::Instance &inst, vk::PhysicalDevice &gpu)
{
    /* Make initial call to query gpu_count, then second call for gpu info*/

    uint32_t gpu_count;
    auto result = inst.enumeratePhysicalDevices(&gpu_count, nullptr);
    VERIFY(result == vk::Result::eSuccess);
    assert(gpu_count > 0);

    if (gpu_count > 0) {
        TempArray<vk::PhysicalDevice> phyDev(gpu_count);
        auto result = inst.enumeratePhysicalDevices(&gpu_count, phyDev.get());
        VERIFY(result == vk::Result::eSuccess);
        gpu = phyDev[0];
    }

    return gpu_count > 0;
}

bool
vkUtil::find_swapchain_in_device_extensions(
    vk::PhysicalDevice &_phyDev,
    uint32_t &_enabled_extension_count,
    const char *_extension_names[64])
{
    bool swapChainExtFound = false;
    uint32_t device_extension_count = 0;

    _enabled_extension_count = 0;
    memset(_extension_names, 0, sizeof(_extension_names));

    auto result = _phyDev.enumerateDeviceExtensionProperties(
        nullptr, &device_extension_count, nullptr);
    VERIFY(result == vk::Result::eSuccess);


    if (device_extension_count > 0) {
        TempArray<vk::ExtensionProperties> dev_extensions(device_extension_count);

        result = _phyDev.enumerateDeviceExtensionProperties(
            nullptr, &device_extension_count, dev_extensions.get());
        VERIFY(result == vk::Result::eSuccess);


        for (uint32_t i = 0; i < device_extension_count; i++) {

            printf("%d: %s\n", i, dev_extensions[i].extensionName);

            if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                dev_extensions[i].extensionName)) {
                swapChainExtFound = true;
                _extension_names[_enabled_extension_count++] =
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME;
            }
            assert(_enabled_extension_count < 64);
        }
    }

    return swapChainExtFound;
}
