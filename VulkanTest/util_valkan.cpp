#include "util_valkan.h"
#include "DebugConsole.h"


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


//Extensionを(3つ)調べる
//[out] _enabled_extension_count 有効なExtensionの数0-3を返す
//[out] _extension_names 有効なExtension名(文字列)へのポインタを返す
//[out] _surfaceExtFound VK_KHR_SURFACE_EXTENSION_NAMEが有効ならTrue
//[out] _platformSurfaceExtFound VK_KHR_WIN32_SURFACE_EXTENSION_NAMEが有効ならTrue
//[out] _debugReportExtFound VK_EXT_DEBUG_REPORT_EXTENSION_NAMEが有効ならTrue
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
    DC_PRINT("Error! MyDebugReportCallback : %s\n", pMessage);

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

            DC_PRINT("%d: %s\n", i, dev_extensions[i].extensionName);

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

//TODO:インスタンスクラス行き？
void vkUtil::createWin32Surface(vk::Instance &inst, HINSTANCE hinst, HWND hwnd, vk::SurfaceKHR &out)
{
    // Create a WSI surface for the window:
    //VK_USE_PLATFORM_WIN32_KHR)
    auto const createInfo = vk::Win32SurfaceCreateInfoKHR()
        .setHinstance(hinst)
        .setHwnd(hwnd);

    auto result = inst.createWin32SurfaceKHR(&createInfo, nullptr, &out);
    VERIFY(result == vk::Result::eSuccess);
}

//[out] graphic_qfi, present_qfi
void vkUtil::findQueueFamilyIndeciesForGraphicsAndPresent(
    vk::PhysicalDevice &gpu,
    vk::SurfaceKHR &surface,
    int qfc,
    const vk::QueueFamilyProperties *qfProps,
    uint32_t &graphic_qfi,
    uint32_t &preset_qfi,
    bool &separate_present_queue
    )
{

    // Iterate over each queue to learn whether it supports presenting:
    std::unique_ptr<vk::Bool32[]> supportsPresent(new vk::Bool32[qfc]);

    for (int i = 0; i < qfc; i++) {
        gpu.getSurfaceSupportKHR(i, surface, &supportsPresent[i]);
    }

    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t presentQueueFamilyIndex = UINT32_MAX;
    for (int i = 0; i < qfc; i++) {
        if (qfProps[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            if (graphicsQueueFamilyIndex == UINT32_MAX) {
                graphicsQueueFamilyIndex = i;
            }

            if (supportsPresent[i] == VK_TRUE) {
                graphicsQueueFamilyIndex = i;
                presentQueueFamilyIndex = i;
                break;
            }
        }
    }

    if (presentQueueFamilyIndex == UINT32_MAX) {
        // If didn't find a queue that supports both graphics and present,
        // then find a separate present queue.
        for (int i = 0; i < qfc; ++i) {
            if (supportsPresent[i] == VK_TRUE) {
                presentQueueFamilyIndex = i;
                break;
            }
        }
    }

    // Generate error if could not find both a graphics and a present queue
    if (graphicsQueueFamilyIndex == UINT32_MAX ||
        presentQueueFamilyIndex == UINT32_MAX) {

        DC_PRINT("Could not find both graphics and present queues\n"
            "Swapchain Initialization Failure");
    }

    graphic_qfi = graphicsQueueFamilyIndex;
    preset_qfi = presentQueueFamilyIndex;
    separate_present_queue = graphicsQueueFamilyIndex != presentQueueFamilyIndex;
}


void vkUtil::createDevice(
    vk::PhysicalDevice &in_gpu,
    uint32_t in_graphic_queue_family_index,
    uint32_t in_present_queue_family_index,
    bool in_separate_present_queue,
    uint32_t in_enabled_extension_count,
    const char *in_extension_names[64],
    vk::Device &out_device)
{
    float const priorities[1] = { 0.0 };

    vk::DeviceQueueCreateInfo queues[2];
    queues[0].setQueueFamilyIndex(in_graphic_queue_family_index);
    queues[0].setQueueCount(1);
    queues[0].setPQueuePriorities(priorities);

    auto deviceInfo = vk::DeviceCreateInfo()
        .setQueueCreateInfoCount(1)
        .setPQueueCreateInfos(queues)
        .setEnabledLayerCount(0)
        .setPpEnabledLayerNames(nullptr)
        .setEnabledExtensionCount(in_enabled_extension_count)
        .setPpEnabledExtensionNames(
            (const char *const *)in_extension_names)
        .setPEnabledFeatures(nullptr);

#ifdef _DEBUG
    static char const *const device_layer_standard_validation[] = { "VK_LAYER_LUNARG_standard_validation" };
    deviceInfo.setEnabledLayerCount(1).setPpEnabledLayerNames(device_layer_standard_validation);
#endif

    if (in_separate_present_queue) {
        queues[1].setQueueFamilyIndex(in_present_queue_family_index);
        queues[1].setQueueCount(1);
        queues[1].setPQueuePriorities(priorities);
        deviceInfo.setQueueCreateInfoCount(2);
    }

    auto result = in_gpu.createDevice(&deviceInfo, nullptr, &out_device);
    VERIFY(result == vk::Result::eSuccess);
}


// デバイスからキューを取得。
void vkUtil::getQueue(
    vk::Device &in_device,
    uint32_t in_graphic_queue_family_index,
    uint32_t in_present_queue_family_index,
    bool in_separate_present_queue,
    vk::Queue &out_graphics_queue,
    vk::Queue &out_present_queue)
{
    in_device.getQueue(in_graphic_queue_family_index, 0, &out_graphics_queue);
    if (!in_separate_present_queue) {
        out_present_queue = out_graphics_queue;
    }
    else {
        in_device.getQueue(in_present_queue_family_index, 0, &out_present_queue);
    }
}

//
void vkUtil::getFormatAndColorSpace(
    vk::PhysicalDevice &in_gpu,
    vk::SurfaceKHR &in_surface,
    vk::Format &out_format,
    vk::ColorSpaceKHR &out_color_space)
{
    // Get the list of VkFormat's that are supported:
    uint32_t formatCount;
    auto result = in_gpu.getSurfaceFormatsKHR(in_surface, &formatCount, nullptr);
    VERIFY(result == vk::Result::eSuccess);

    std::unique_ptr<vk::SurfaceFormatKHR[]> surfFormats(
        new vk::SurfaceFormatKHR[formatCount]);
    result = in_gpu.getSurfaceFormatsKHR(in_surface, &formatCount, surfFormats.get());
    VERIFY(result == vk::Result::eSuccess);

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if (formatCount == 1 && surfFormats[0].format == vk::Format::eUndefined) {
        out_format = vk::Format::eB8G8R8A8Unorm;
    }
    else {
        assert(formatCount >= 1);
        out_format = surfFormats[0].format;
    }
    out_color_space = surfFormats[0].colorSpace;
}


void vkUtil::createFencesAndSemaphores(
    vk::Device &in_device,
    int in_frame_lag,
    bool in_separate_present_queue,
    vk::Fence *out_fences,
    vk::Semaphore *out_semaphoresImageAcquired,
    vk::Semaphore *out_semaphoresDrawComplete,
    vk::Semaphore *out_semaphoresImageOwnershop
    )
{
    // Create semaphores to synchronize acquiring presentable buffers before
    // rendering and waiting for drawing to be complete before presenting
    auto const semaphoreCreateInfo = vk::SemaphoreCreateInfo();

    // Create fences that we can use to throttle if we get too far
    // ahead of the image presents
    auto const fenceCreateInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

    for (uint32_t i = 0; i < in_frame_lag; i++) {
        
        in_device.createFence(&fenceCreateInfo, nullptr, &out_fences[i]);

        auto result = in_device.createSemaphore(&semaphoreCreateInfo, nullptr,
            &out_semaphoresImageAcquired[i]);
        VERIFY(result == vk::Result::eSuccess);

        result = in_device.createSemaphore(&semaphoreCreateInfo, nullptr,
            &out_semaphoresDrawComplete[i]);
        VERIFY(result == vk::Result::eSuccess);

        if (in_separate_present_queue) {
            result = in_device.createSemaphore(&semaphoreCreateInfo, nullptr,
                &out_semaphoresImageOwnershop[i]);
            VERIFY(result == vk::Result::eSuccess);
        }
    }
}


void vkUtil::createCommandPool(
    vk::Device &in_device,
    uint32_t in_queue_family_index,
    vk::CommandPool &out_cmd_pool
    )
{
    auto const cmd_pool_info = vk::CommandPoolCreateInfo().setQueueFamilyIndex(in_queue_family_index);
    auto result = in_device.createCommandPool(&cmd_pool_info, nullptr, &out_cmd_pool);
    VERIFY(result == vk::Result::eSuccess);
}

void vkUtil::allocateCommandBuffers(
    vk::Device &in_device,
    vk::CommandPool &in_cmd_pool,
    int in_swap_chain_image_count,
    SwapchainBuffers *out_swap_chain_buffers
    )
{
    vk::Result result;

    auto const cbAllocInfo = vk::CommandBufferAllocateInfo()
        .setCommandPool(in_cmd_pool)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);

    for (uint32_t i = 0; i < in_swap_chain_image_count; ++i) {
        result = in_device.allocateCommandBuffers(&cbAllocInfo, &out_swap_chain_buffers[i].cmd);
        VERIFY(result == vk::Result::eSuccess);
    }
}


void vkUtil::build_image_ownership_cmd(
    uint32_t in_graphic_queue_family_index,
    uint32_t in_present_queue_family_index,
    SwapchainBuffers &out_swapchain_buffer
    )
{
    auto const cmbuf_begin_info = vk::CommandBufferBeginInfo().setFlags(
        vk::CommandBufferUsageFlagBits::eSimultaneousUse);

    auto result = out_swapchain_buffer.graphics_to_present_cmd.begin(&cmbuf_begin_info);
    VERIFY(result == vk::Result::eSuccess);

    auto const image_ownership_barrier =
        vk::ImageMemoryBarrier()
        .setSrcAccessMask(vk::AccessFlags())
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setOldLayout(vk::ImageLayout::ePresentSrcKHR)
        .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
        .setSrcQueueFamilyIndex(in_graphic_queue_family_index)
        .setDstQueueFamilyIndex(in_present_queue_family_index)
        .setImage(out_swapchain_buffer.image)
        .setSubresourceRange(vk::ImageSubresourceRange(
            vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

    out_swapchain_buffer.graphics_to_present_cmd.pipelineBarrier(
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::DependencyFlagBits(), 0, nullptr, 0, nullptr, 1,
        &image_ownership_barrier);

    result = out_swapchain_buffer.graphics_to_present_cmd.end();
    VERIFY(result == vk::Result::eSuccess);

}


void vkUtil::allocateCommandBuffersForPresent(
    vk::Device &in_device,
    vk::CommandPool &in_cmd_pool,
    int in_swap_chain_image_count,
    uint32_t in_graphic_queue_family_index,
    uint32_t in_present_queue_family_index,
    SwapchainBuffers *out_swap_chain_buffers
    )
{
    vk::Result result;

    auto const cbAllocInfo = vk::CommandBufferAllocateInfo()
        .setCommandPool(in_cmd_pool)
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);

    for (uint32_t i = 0; i < in_swap_chain_image_count; ++i) {
        result = in_device.allocateCommandBuffers(&cbAllocInfo, &out_swap_chain_buffers[i].graphics_to_present_cmd);
        VERIFY(result == vk::Result::eSuccess);

        vkUtil::build_image_ownership_cmd(
            in_graphic_queue_family_index,
            in_present_queue_family_index,
            out_swap_chain_buffers[i]);
    }
}


void vkUtil::getSurfacePresentModesKHR(
    vk::PhysicalDevice &in_gpu,
    vk::SurfaceKHR &in_surface,
    std::vector<vk::PresentModeKHR> &out_presentModes)
{
    uint32_t presentModeCount;
    auto result = in_gpu.getSurfacePresentModesKHR(in_surface, &presentModeCount, nullptr);
    VERIFY(result == vk::Result::eSuccess);

    out_presentModes.resize(presentModeCount);
    result = in_gpu.getSurfacePresentModesKHR(in_surface, &presentModeCount, out_presentModes.data());
    VERIFY(result == vk::Result::eSuccess);
}

//TODO: Surfaceクラス行き？
//GPU,サーフェイスからケイパビリティ情報を取得。
void vkUtil::getSurfaceCapabilitiesKHR(
    vk::PhysicalDevice &in_gpu,
    vk::SurfaceKHR &in_surface,
    vk::SurfaceCapabilitiesKHR &out)
{
    auto result = in_gpu.getSurfaceCapabilitiesKHR(in_surface, &out);
    VERIFY(result == vk::Result::eSuccess);
}


//GPU,サーフェイスからプレゼントモードを選択。
//４種類あるが、現状はデフォルトはFIFOモード。
vk::PresentModeKHR vkUtil::selectPresentMode(
    vk::PhysicalDevice &in_gpu,
    vk::SurfaceKHR &in_surface)
{
    //GPUとサーフェイスからプレゼントモードのリストを取得。
    std::vector<vk::PresentModeKHR> presentModes;
    vkUtil::getSurfacePresentModesKHR(in_gpu, in_surface, presentModes);

    // The FIFO present mode is guaranteed by the spec to be supported
    // and to have no tearing.  It's a great default present mode to use.
    vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;

    //  There are times when you may wish to use another present mode.  The
    //  following code shows how to select them, and the comments provide some
    //  reasons you may wish to use them.
    //
    // It should be noted that Vulkan 1.0 doesn't provide a method for
    // synchronizing rendering with the presentation engine's display.  There
    // is a method provided for throttling rendering with the display, but
    // there are some presentation engines for which this method will not work.
    // If an application doesn't throttle its rendering, and if it renders much
    // faster than the refresh rate of the display, this can waste power on
    // mobile devices.  That is because power is being spent rendering images
    // that may never be seen.
    //#define DESIRE_VK_PRESENT_MODE_IMMEDIATE_KHR
    //#define DESIRE_VK_PRESENT_MODE_MAILBOX_KHR
    //#define DESIRE_VK_PRESENT_MODE_FIFO_RELAXED_KHR
#if defined(DESIRE_VK_PRESENT_MODE_IMMEDIATE_KHR)
    // VK_PRESENT_MODE_IMMEDIATE_KHR is for applications that don't care
    // about
    // tearing, or have some way of synchronizing their rendering with the
    // display.
    for (size_t i = 0; i < presentModes.size(); ++i) {
        if (presentModes[i] == vk::PresentModeKHR::eImmediate) {
            swapchainPresentMode = vk::PresentModeKHR::eImmediate;
            break;
        }
    }
#elif defined(DESIRE_VK_PRESENT_MODE_MAILBOX_KHR)
    // VK_PRESENT_MODE_MAILBOX_KHR may be useful for applications that
    // generally render a new presentable image every refresh cycle, but are
    // occasionally early.  In this case, the application wants the new
    // image
    // to be displayed instead of the previously-queued-for-presentation
    // image
    // that has not yet been displayed.
    for (size_t i = 0; i < presentModes.size(); ++i) {
        if (presentModes[i] == vk::PresentModeKHR::eMailbox) {
            swapchainPresentMode = vk::PresentModeKHR::eMailbox;
            break;
        }
    }
#elif defined(DESIRE_VK_PRESENT_MODE_FIFO_RELAXED_KHR)
    // VK_PRESENT_MODE_FIFO_RELAXED_KHR is for applications that generally
    // render a new presentable image every refresh cycle, but are
    // occasionally
    // late.  In this case (perhaps because of stuttering/latency concerns),
    // the application wants the late image to be immediately displayed,
    // even
    // though that may mean some tearing.
    for (size_t i = 0; i < presentModes.size(); ++i) {
        if (presentModes[i] == vk::PresentModeKHR::eFifoRelaxed) {
            swapchainPresentMode = vk::PresentModeKHR::eFifoRelaxed;
            break;
        }
    }
#endif

    return swapchainPresentMode;
}


void vkUtil::determineSwapChainExtent(
    vk::SurfaceCapabilitiesKHR &in_surfCapabilities,
    uint32_t &io_current_width,
    uint32_t &io_current_height,
    vk::Extent2D &out_swapchainExtent)
{
    //ケイパビリティのwidth と height は両方-1か、両方-1以外である。
    //NOTE:現在の環境では、surfCapabilities.currentExtent.width/heightにはウインドウサイズの512,512が返ってくる。(2017.03.26 Geforce1060/Win10)

    if (in_surfCapabilities.currentExtent.width == (uint32_t)-1) {
        // もしケイパビリティのサーフェスサイズが未定義(-1)であれば要求されたサイズ(width_,height_)にする。
        out_swapchainExtent.width = io_current_width;
        out_swapchainExtent.height = io_current_height;
    }
    else {
        // ケイパビリティのサーフェスサイズが定義されていれば、スワップチェインサイズを合わせる。
        out_swapchainExtent = in_surfCapabilities.currentExtent;
        io_current_width = in_surfCapabilities.currentExtent.width;
        io_current_height = in_surfCapabilities.currentExtent.height;
    }
}


uint32_t vkUtil::determineDesiredNumberOfSwapchainImages(
    vk::SurfaceCapabilitiesKHR &in_surfCapabilities)
{
    //Swap chainのVkImageの数を決定する。
    //表示用と表示向けにキューされる画像の他に、自身で１つ要求する。
    //NOTE: 現環境ではminImageCountには２が入っている。(2017.03.26 Geforce1060/Win10)
    uint32_t desiredNumberOfSwapchainImages = in_surfCapabilities.minImageCount + 1;

    //もしmaxImageCountが0ならdesiredNumberOfSwapchainImagesをそのまま要求する。
    //さもなければ、maxImageCountでdesiredNumberOfSwapchainImagesを制限する。
    //NOTE: 現環境ではmaxImageCountは8が入っている。(2017.03.26 Geforce1060/Win10)
    if ((in_surfCapabilities.maxImageCount > 0) &&
        (desiredNumberOfSwapchainImages > in_surfCapabilities.maxImageCount)) {
        desiredNumberOfSwapchainImages = in_surfCapabilities.maxImageCount;
    }

    return desiredNumberOfSwapchainImages;
}


void vkUtil::createSwapchainKHR(
    vk::Device &in_device,
    vk::SurfaceCapabilitiesKHR &surfCapabilities,
    vk::PresentModeKHR swapchainPresentMode,
    vk::SurfaceKHR &in_surface,
    vk::Extent2D swapchainExtent,
    vk::Format format,
    vk::ColorSpaceKHR color_space,
    vk::SwapchainKHR &io_swapchain)
{

    vk::SwapchainKHR oldSwapchain = io_swapchain;

    //SwapChainイメージの数を決定する。
    uint32_t desiredNumberOfSwapchainImages =
        vkUtil::determineDesiredNumberOfSwapchainImages(surfCapabilities);

    //PretransformFlagBits
    vk::SurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms &
        vk::SurfaceTransformFlagBitsKHR::eIdentity) {
        preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    }
    else {
        preTransform = surfCapabilities.currentTransform;
    }

    auto const swapchain_ci =
        vk::SwapchainCreateInfoKHR()
        .setSurface(in_surface)
        .setMinImageCount(desiredNumberOfSwapchainImages)
        .setImageFormat(format)
        .setImageColorSpace(color_space)
        .setImageExtent({ swapchainExtent.width, swapchainExtent.height })
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setImageSharingMode(vk::SharingMode::eExclusive)
        .setQueueFamilyIndexCount(0)
        .setPQueueFamilyIndices(nullptr)
        .setPreTransform(preTransform)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(swapchainPresentMode)
        .setClipped(true)
        .setOldSwapchain(oldSwapchain);

    auto result = in_device.createSwapchainKHR(&swapchain_ci, nullptr, &io_swapchain);
    VERIFY(result == vk::Result::eSuccess);

    // If we just re-created an existing swapchain, we should destroy the
    // old swapchain at this point.
    // Note: destroying the swapchain also cleans up all its associated
    // presentable images once the platform is done with them.
    if (oldSwapchain) {
        in_device.destroySwapchainKHR(oldSwapchain, nullptr);
    }
}


void vkUtil::createSwapchainBuffersAndImages(
    vk::Device &in_device,
    vk::Format in_format,
    uint32_t in_swapchainImageCount,
    vk::SwapchainKHR &in_swapchain,
    std::unique_ptr<SwapchainBuffers[]> &out_swapChainBuffers)
{
    //スワップチェインイメージ取得。
    std::unique_ptr<vk::Image[]> swapchainImages(new vk::Image[in_swapchainImageCount]);
    auto result = in_device.getSwapchainImagesKHR(in_swapchain, &in_swapchainImageCount, swapchainImages.get());
    VERIFY(result == vk::Result::eSuccess);

    //スワップチェインバッファを新しく作り直す。
    out_swapChainBuffers.reset(new SwapchainBuffers[in_swapchainImageCount]);

    //スワップチェインイメージを新しく生成したイメージビューに渡しバッファに接続する。
    for (uint32_t i = 0; i < in_swapchainImageCount; ++i) {
    
        auto const color_image_view =
            vk::ImageViewCreateInfo()
            .setImage(swapchainImages[i])
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(in_format)
            .setSubresourceRange(vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

        out_swapChainBuffers[i].image = swapchainImages[i];

        auto result = in_device.createImageView(
            &color_image_view, 
            nullptr,
            &out_swapChainBuffers[i].view);

        VERIFY(result == vk::Result::eSuccess);
    }



}