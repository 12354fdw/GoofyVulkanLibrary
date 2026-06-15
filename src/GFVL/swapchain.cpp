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
/*
using namespace GFVL;

GFVL_SWAPCHAIN_SUPPORT GFVL_QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
  GFVL_SWAPCHAIN_SUPPORT swapchainSupport{};

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapchainSupport.capabilities);

  uint32_t count = 0;

  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, nullptr);
  swapchainSupport.formats.resize(count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, swapchainSupport.formats.data());

  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, nullptr);
  swapchainSupport.presentModes.resize(count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, swapchainSupport.presentModes.data());

  return swapchainSupport;
}
GFVL_SWAPCHAIN_CONFIG GFVL_SelectSwapchainConfig(const GFVL_SWAPCHAIN_SUPPORT &swapchainSupport, SDL_Window *window) {
  GFVL_SWAPCHAIN_CONFIG swapchainConfiguration{};

  // pick random format at first
  swapchainConfiguration.format = swapchainSupport.formats[0].format;
  swapchainConfiguration.colorSpace = swapchainSupport.formats[0].colorSpace;

  // try to get ideal format
  for (const VkSurfaceFormatKHR &surfaceFormat : swapchainSupport.formats) {
    if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      swapchainConfiguration.format = surfaceFormat.format;
      swapchainConfiguration.colorSpace = surfaceFormat.colorSpace;
      break;
    }
  }

  swapchainConfiguration.presentMode = VK_PRESENT_MODE_FIFO_KHR; // immediate mode for vsync off

  if (swapchainSupport.capabilities.currentExtent.width != UINT32_MAX) {
    swapchainConfiguration.extent = swapchainSupport.capabilities.currentExtent;
  } else {
    int width, height;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    swapchainConfiguration.extent = {(uint32_t)width, (uint32_t)height};
  }

  uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

  if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
    imageCount = swapchainSupport.capabilities.maxImageCount;
  }

  swapchainConfiguration.imageCount = imageCount;

  return swapchainConfiguration;
}
void GFVL_BuildSwapchain(GFVL_SWAPCHAIN &swapchain, const GFVL_SWAPCHAIN_CONFIG &swapchainConfig, const GFVL_SWAPCHAIN_SUPPORT &swapchainSupport) {
  VkSwapchainCreateInfoKHR info{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = swapchain.surface,
      .minImageCount = swapchainConfig.imageCount,
      .imageFormat = swapchainConfig.format,
      .imageColorSpace = swapchainConfig.colorSpace,
      .imageExtent = swapchainConfig.extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .preTransform = swapchainSupport.capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = swapchainConfig.presentMode,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE};

  CheckVkResult(vkCreateSwapchainKHR(swapchain.device, &info, nullptr, &swapchain.swapchain));

  uint32_t count = 0;
  vkGetSwapchainImagesKHR(swapchain.device, swapchain.swapchain, &count, nullptr);

  swapchain.images.resize(count);
  vkGetSwapchainImagesKHR(swapchain.device, swapchain.swapchain, &count, swapchain.images.data());

  swapchain.imageViews.resize(count);

  for (size_t i = 0; i < count; i++) {
    VkImageViewCreateInfo vi{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = swapchain.images[i],
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = swapchainConfig.format,
        .components = {VK_COMPONENT_SWIZZLE_IDENTITY,
                       VK_COMPONENT_SWIZZLE_IDENTITY,
                       VK_COMPONENT_SWIZZLE_IDENTITY,
                       VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1}};

    CheckVkResult(vkCreateImageView(swapchain.device, &vi, nullptr, &swapchain.imageViews[i]));
  }

  swapchain.format = swapchainConfig.format;
  swapchain.extent = swapchainConfig.extent;
  swapchain.presentMode = swapchainConfig.presentMode;
  swapchain.imageCount = count;
}
void GFVL_DestroySwapchain(GFVL_SWAPCHAIN &sc) {
  vkDeviceWaitIdle(sc.device);

  for (VkImageView v : sc.imageViews)
    vkDestroyImageView(sc.device, v, nullptr);

  if (sc.swapchain)
    vkDestroySwapchainKHR(sc.device, sc.swapchain, nullptr);

  sc.imageViews.clear();
  sc.images.clear();
}
void GFVL_RecreateSwapchain(GFVL_SWAPCHAIN &sc) {
  GFVL_DestroySwapchain(sc);

  GFVL_SWAPCHAIN_SUPPORT support = GFVL_QuerySwapchainSupport(sc.physicalDevice, sc.surface);
  GFVL_SWAPCHAIN_CONFIG config = GFVL_SelectSwapchainConfig(support, sc.window);

  GFVL_BuildSwapchain(sc, config, support);
}

namespace GFVL {
SWAPCHAIN::SWAPCHAIN(DEVICE &device, SDL_Window *window) {
  
}
void SWAPCHAIN::Destroy(){

}
} 
*/