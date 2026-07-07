/**
 * @file GFVL.hpp
 * @brief Exposes GFVL.
 * @details This is probably the file you wanna include.
 */
#ifndef GFVL_CPP
#define GFVL_CPP
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "../lib/GFVL_core.hpp"
#include "../lib/GFVL_definition.hpp"
namespace GFVL {

class INSTANCE {
public:
  VkInstance instance;
  SDL_Window *window;
  VkSurfaceKHR surface;
  DEVICE device;
  SWAPCHAIN swapchain;
  RENDERPASS renderPass;
  UNIFORM_BUFFER uniformBuffer;
  std::vector<SHADER> shaderStages;
  PIPELINE pipeline;
  Framebuffer framebuffer;
  CommandPool commandPool;

  INSTANCE(APPLICATION_INFO applicationInfo, VERTEX_LAYOUT &layout, std::vector<UNIFORM_BUFFER_BINDING> &bindings, std::vector<SHADER_STAGE> &stages);
  ~INSTANCE();

  INSTANCE(const INSTANCE &) = delete;
  INSTANCE &operator=(const INSTANCE &) = delete;

  INSTANCE(const INSTANCE &&) = delete;
  INSTANCE &operator=(const INSTANCE &&) = delete;
};

} // namespace GFVL

#endif