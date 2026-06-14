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

#include "GFVL.hpp"

VkDeviceSize GFVL_GetDeviceVRAM(VkPhysicalDevice device) {
  VkPhysicalDeviceMemoryProperties memoryProperties{};
  vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);

  VkDeviceSize totalDedicatedMemory = 0;

  for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++) {
    const VkMemoryHeap &heap = memoryProperties.memoryHeaps[i];

    if ((heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0)
      totalDedicatedMemory += heap.size;
  }

  return totalDedicatedMemory;
}
bool GFVL_EnumerateQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, uint32_t &graphicsFamilyIndex, uint32_t &presentFamilyIndex) {
  graphicsFamilyIndex = UINT32_MAX;
  presentFamilyIndex = UINT32_MAX;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  if (queueFamilyCount == 0)
    return false;

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    const bool graphicsSupport = (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
    VkBool32 presentationSupport = VK_FALSE;
    CheckVkResult(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport));

    if (graphicsSupport && graphicsFamilyIndex == UINT32_MAX)
      graphicsFamilyIndex = i;

    if (presentationSupport == VK_TRUE && presentFamilyIndex == UINT32_MAX)
      presentFamilyIndex = i;

    /*if (GFVL_DEBUG_MODE) {
      std::cout << "[GFVL] Queue Family " << i << '\n';
      std::cout << "  Graphics: " << (graphicsSupport ? "Yes" : "No") << '\n';
      std::cout << "  Present: " << (presentationSupport ? "Yes" : "No") << '\n';
      std::cout << "  Queue Count: " << queueFamilies[i].queueCount << '\n';
    }*/
  }

  return graphicsFamilyIndex != UINT32_MAX && presentFamilyIndex != UINT32_MAX;
}
bool GFVL_HasRequiredDeviceExtensions(VkPhysicalDevice device) {
  uint32_t extensionCount = 0;

  CheckVkResult(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr));

  std::vector<VkExtensionProperties> extensions(extensionCount);

  CheckVkResult(vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data()));

  for (const VkExtensionProperties &extension : extensions) {
    if (strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
      return true;
  }

  return false;
}
int GFVL_CalculateDeviceScore(VkPhysicalDevice device, GFVL_PREFERRED_GPU preference) {
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(device, &properties);

  int score = 0;

  switch (properties.deviceType) {
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    score += preference == GFVL_PREFERRED_GPU_PERFORMANCE ? 1000 : 100;
    break;

  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    score += preference == GFVL_PREFERRED_GPU_POWER_SAVING ? 1000 : 500;
    break;

  case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    score += 250;
    break;

  case VK_PHYSICAL_DEVICE_TYPE_CPU:
    score += 1;
    break;

  default:
    break;
  }

  score += static_cast<int>(
      GFVL_GetDeviceVRAM(device) / (1024ull * 1024ull * 1024ull));

  return score;
}
GFVL_PHYSICAL_DEVICE GFVL_EvaluatePhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface, GFVL_PREFERRED_GPU preference) {
  uint32_t graphicsFamilyIndex = UINT32_MAX;
  uint32_t presentFamilyIndex = UINT32_MAX;

  if (!GFVL_EnumerateQueueFamilies(
          device,
          surface,
          graphicsFamilyIndex,
          presentFamilyIndex))
    return {};

  if (!GFVL_HasRequiredDeviceExtensions(device))
    return {};

  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(device, &properties);

  const VkDeviceSize dedicatedVideoMemory =
      GFVL_GetDeviceVRAM(device);

  if (GFVL_DEBUG_MODE) {
    std::cout << "[GFVL] Device\n";
    std::cout << "  Name: " << properties.deviceName << '\n';
    // std::cout << "  API Version: " << VK_VERSION_MAJOR(properties.apiVersion) << '.' << VK_VERSION_MINOR(properties.apiVersion) << '.' << VK_VERSION_PATCH(properties.apiVersion) << '\n';
    // std::cout << "  Driver Version: " << properties.driverVersion << '\n';
    // std::cout << "  Vendor ID: " << properties.vendorID << '\n';
    // std::cout << "  Device ID: " << properties.deviceID << '\n';
    std::cout << "  Dedicated VRAM: " << dedicatedVideoMemory / (1024ull * 1024ull) << " MB\n";

    switch (properties.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
      std::cout << "  Type: Discrete GPU\n";
      break;

    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
      std::cout << "  Type: Integrated GPU\n";
      break;

    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
      std::cout << "  Type: Virtual GPU\n";
      break;

    case VK_PHYSICAL_DEVICE_TYPE_CPU:
      std::cout << "  Type: CPU\n";
      break;

    default:
      std::cout << "  Type: Other\n";
      break;
    }

    std::cout << "  Score: "
              << GFVL_CalculateDeviceScore(device, preference)
              << '\n';
  }

  return {
      .device = device,
      .videoMemory = dedicatedVideoMemory,
      .graphicsFamilyIndex = graphicsFamilyIndex,
      .presentFamilyIndex = presentFamilyIndex,
  };
}
GFVL_PHYSICAL_DEVICE GFVL_InitializePhysicalDevice(VkInstance instance, VkSurfaceKHR surface, GFVL_PREFERRED_GPU preference) {
  uint32_t physicalDeviceCount = 0;

  CheckVkResult(
      vkEnumeratePhysicalDevices(
          instance,
          &physicalDeviceCount,
          nullptr));

  if (physicalDeviceCount == 0)
    throw std::runtime_error(
        "[GFVL] No Vulkan-compatible GPUs found.");

  std::vector<VkPhysicalDevice> physicalDevices(
      physicalDeviceCount);

  CheckVkResult(
      vkEnumeratePhysicalDevices(
          instance,
          &physicalDeviceCount,
          physicalDevices.data()));

  if (GFVL_DEBUG_MODE)
    std::cout << "[GFVL] Found "
              << physicalDeviceCount
              << " Vulkan-compatible devices.\n";

  GFVL_PHYSICAL_DEVICE bestDevice{};
  int bestScore = -1;

  for (const VkPhysicalDevice device : physicalDevices) {
    const GFVL_PHYSICAL_DEVICE candidate =
        GFVL_EvaluatePhysicalDevice(
            device,
            surface,
            preference);

    if (candidate.device == VK_NULL_HANDLE)
      continue;

    const int candidateScore =
        GFVL_CalculateDeviceScore(
            device,
            preference);

    if (candidateScore > bestScore) {
      bestScore = candidateScore;
      bestDevice = candidate;
    }
  }

  if (bestDevice.device == VK_NULL_HANDLE)
    throw std::runtime_error(
        "[GFVL] No suitable Vulkan device found.");

  if (GFVL_DEBUG_MODE) {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(
        bestDevice.device,
        &properties);

    std::cout << "[GFVL] Selected Device: "
              << properties.deviceName
              << '\n';

    std::cout << "[GFVL] Graphics Queue Family: "
              << bestDevice.graphicsFamilyIndex
              << '\n';

    std::cout << "[GFVL] Present Queue Family: "
              << bestDevice.presentFamilyIndex
              << '\n';

    std::cout << "[GFVL] Dedicated VRAM: "
              << bestDevice.videoMemory / (1024ull * 1024ull)
              << " MB\n";

    std::cout << "[GFVL] Final Score: "
              << bestScore
              << '\n';
  }

  return bestDevice;
}
VkDeviceQueueCreateInfo GFVL_InitializeQueueCreation(uint32_t graphicsFamilyIndex, VkSurfaceKHR surface, VkPhysicalDevice device, uint32_t *familyIndex) {
  constexpr float queuePriority = 1.0f;

  VkDeviceQueueCreateInfo queueCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = graphicsFamilyIndex,
      .queueCount = 1,
      .pQueuePriorities = &queuePriority};

  if (GFVL_DEBUG_MODE) {
    std::cout << "[GFVL] Queue creation info initialized.\n";
    std::cout << "[GFVL] Family Index: " << queueCreateInfo.queueFamilyIndex << '\n';
  }

  return queueCreateInfo;
}
std::vector<const char *> GFVL_EnumerateDeviceExtensions(VkPhysicalDevice device) {
  uint32_t deviceExtensionCount = 0;
  CheckVkResult(vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, nullptr));

  std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
  CheckVkResult(vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, deviceExtensions.data()));
  /* this prints too damn much
  if (GFVL_DEBUG_MODE) {
    std::cout << "[GFVL] Available device extensions:\n";
    for (const auto &ext : deviceExtensions)
      std::cout << "  " << ext.extensionName << '\n';
  }
  */
  std::vector<const char *> enabledDeviceExtensions;
  for (const VkExtensionProperties &ext : deviceExtensions) {
    if (strcmp(ext.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
      enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
      if (GFVL_DEBUG_MODE)
        std::cout << "[GFVL] Enabling extension: " << VK_KHR_SWAPCHAIN_EXTENSION_NAME << '\n';
    }
  }

  if (enabledDeviceExtensions.empty())
    throw std::runtime_error("[GFVL] Required extension VK_KHR_swapchain not found.");
  if (GFVL_DEBUG_MODE)
    std::cout << "[GFVL] Enabled device extension count: " << enabledDeviceExtensions.size() << '\n';

  return enabledDeviceExtensions;
}