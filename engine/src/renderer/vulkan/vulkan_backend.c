#include "renderer/vulkan/vulkan_backend.h"
#include "renderer/vulkan/vulkan_types.inl"
#include "renderer/vulkan/vulkan_platform.h"
#include "core/logger.h"
#include "core/kstring.h"
#include "containers/darray.h"
#include "platform/platform.h"

// forward declrations
VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data);

// global variables
static vulkan_context context;

b8 vulkan_renderer_backend_initialize(struct renderer_backend *backend, const char *application_name, struct platform_state *plt_state) {
    // TODO: write a custom allocator
    context.allocator = 0;

    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};  // stype field for the VkApplicationInfo
    app_info.apiVersion = VK_API_VERSION_1_3;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    app_info.pEngineName = "ARGOSTA Engine";
    app_info.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    VkInstanceCreateInfo inst_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};  // again, stype field
    inst_info.pApplicationInfo = &app_info;

    // Extensions
    const char **required_extensions = darray_create(const char *);
    darray_push(required_extensions, &VK_KHR_SURFACE_EXTENSION_NAME);  // generic surface extension
    platform_get_required_extension_names(&required_extensions);       // platform-specific extention(s)

#if defined(_DEBUG)
    darray_push(required_extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  // debug utils

    KDEBUG("Required Vulkan Extensions:");
    u32 n_ext = darray_length(required_extensions);
    for (u32 i = 0; i < n_ext; ++i) {
        KDEBUG(required_extensions[i]);
    }
#endif

    inst_info.enabledExtensionCount = darray_length(required_extensions);
    inst_info.ppEnabledExtensionNames = required_extensions;

    // Validation Layers
    const char **required_validation_layer_names = 0;
    u32 required_validation_layers_count = 0;

// If validation should be done, get a list of the required validation layert names
// and make sure they exist. Validation layers should only be enabled on non-release builds.
#if defined(_DEBUG)
    KINFO("Validations layers enabled. Verifying ...");

    required_validation_layer_names = darray_create(const char *);
    darray_push(required_validation_layer_names, &"VK_LAYER_KHRONOS_validation");
    required_validation_layers_count = darray_length(required_validation_layer_names);

    // obtain a list of all available layers
    u32 available_layer_count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
    VkLayerProperties *availble_layers = darray_reserve(VkLayerProperties, available_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, availble_layers));
    KDEBUG("Found %d supported layers.", available_layer_count);

    for (u32 i = 0; i < required_validation_layers_count; ++i) {
        b8 found = FALSE;
        for (u32 j = 0; j < available_layer_count; ++j) {
            if (strings_equal(required_validation_layer_names[i], availble_layers[j].layerName)) {
                found = TRUE;
                KINFO("Found layer: %s;", required_validation_layer_names[i]);
                break;
            }
        }
        if (!found) {
            KINFO("Could not find layer: %s; Required validation layer is missing. Shutting down the engine.", required_validation_layer_names[i]);
            return FALSE;
        }
    }
    KINFO("All required validation layer are found successfully.")

#endif

    inst_info.ppEnabledLayerNames = required_validation_layer_names;
    inst_info.enabledLayerCount = required_validation_layers_count;

    VK_CHECK(vkCreateInstance(&inst_info, context.allocator, &context.instance));
    KINFO("Vulkan instance created.");

// Debugger
#if defined(_DEBUG)
    KDEBUG("Creating Vulkan debugger...");
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;  //|
                                                                      //    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug_callback;

    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
    KASSERT_MSG(func, "Failed to create debug messenger!");
    VK_CHECK(func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));
    KDEBUG("Vulkan debugger created.");
#endif

    KINFO("Vulkan renderer initialized successfully.");
    return TRUE;
}

void vulkan_renderer_backend_shutdown(struct renderer_backend *backend) {
    KDEBUG("Destroying Vulkan debugger...");
    if (context.debug_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(context.instance, context.debug_messenger, context.allocator);
    }

    KDEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_renderer_backend_on_resize(struct renderer_backend *backend, u16 new_width, u16 new_height) {
}

b8 vulkan_renderer_backend_begin_frame(struct renderer_backend *backend, f32 delta_time) {
    return TRUE;
}
b8 vulkan_renderer_backend_end_frame(struct renderer_backend *backend, f32 delta_time) {
    return TRUE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data) {
    switch (message_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            KERROR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            KWARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            KINFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            KTRACE(callback_data->pMessage);
            break;
    }
    return VK_FALSE;
}
