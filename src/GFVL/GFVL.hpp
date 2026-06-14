#ifndef GFVL_CPP
#define GFVL_CPP
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>

const bool GFVL_DEBUG_MODE = true;
enum GFVL_PREFERRED_GPU {
  GFVL_PREFERRED_GPU_POWER_SAVING,
  GFVL_PREFERRED_GPU_PERFORMANCE,
};


struct GFVL_PHYSICAL_DEVICE {
  VkPhysicalDevice device = VK_NULL_HANDLE;
  VkDeviceSize videoMemory = 0;
  uint32_t graphicsFamilyIndex = UINT32_MAX;
  uint32_t presentFamilyIndex = UINT32_MAX;
};

// defined in GFVL.cpp
std::vector<char> readFile(const std::string &filename);
const char *VkResultToString(VkResult result);
void PrintVkResult(VkResult result);
VkResult CheckVkResult(VkResult result);

// defined in physicalDevices.cpp
VkDeviceSize GFVL_GetDeviceVRAM(VkPhysicalDevice device);
GFVL_PHYSICAL_DEVICE GFVL_InitializePhysicalDevice(VkInstance instance, VkSurfaceKHR surface, GFVL_PREFERRED_GPU preference);
VkDeviceQueueCreateInfo GFVL_InitializeQueueCreation(uint32_t graphicsFamilyIndex, VkSurfaceKHR surface, VkPhysicalDevice device, uint32_t *familyIndex);
    std::vector<const char *> GFVL_EnumerateDeviceExtensions(VkPhysicalDevice device);

#endif