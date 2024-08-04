#pragma once

// each renderer backend will implement the function signatures provided in the 
#include "renderer/renderer_backend.h"




b8 vulkan_renderer_backend_initialize(struct renderer_backend *backend, const char *application_name, struct platform_state *plt_state);
void vulkan_renderer_backend_shutdown(struct renderer_backend *backend);

void vulkan_renderer_backend_on_resize(struct renderer_backend *backend, u16 new_width, u16 new_height);

b8 vulkan_renderer_backend_begin_frame(struct renderer_backend *backend, f32 delta_time);
b8 vulkan_renderer_backend_end_frame(struct renderer_backend *backend, f32 delta_time);
