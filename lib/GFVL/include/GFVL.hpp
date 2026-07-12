/*
GoofyVulkanLibrary. A vulkan wrapper, designed to allow users to code Vulkan applications without high boilerplate.
Copyright (C) 2026 acoolmanboon

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
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