#include "renderer/vulkan/vulkan_backend.h"
#include "renderer/vulkan/vulkan_types.inl"
#include "core/logger.h"

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
    inst_info.enabledExtensionCount = 0;
    inst_info.enabledLayerCount = 0;
    inst_info.ppEnabledLayerNames = 0;
    inst_info.ppEnabledExtensionNames = 0;

    VkResult result = vkCreateInstance(&inst_info, context.allocator, &context.instance);
    if(result != VK_SUCCESS){
        KERROR("vkCreateInstance() failed with result: %u", result);
        return FALSE;
    }

    KINFO("Vulkan renderer initialized successfully.");
    return TRUE;
}

void vulkan_renderer_backend_shutdown(struct renderer_backend *backend) {
}

void vulkan_renderer_backend_on_resize(struct renderer_backend *backend, u16 new_width, u16 new_height) {
}

b8 vulkan_renderer_backend_begin_frame(struct renderer_backend *backend, f32 delta_time) {
    return TRUE;
}
b8 vulkan_renderer_backend_end_frame(struct renderer_backend *backend, f32 delta_time) {
    return TRUE;
}
