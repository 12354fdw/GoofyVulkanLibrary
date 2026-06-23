#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <cstring>
#include <vulkan/vulkan.h>

#include "GFVL.hpp"
using namespace GFVL;

// USER-DEFINED STUFF
namespace GFVL {
UNIFORM_BUFFER::UNIFORM_BUFFER(DEVICE &device, size_t uboSize, void *ubo) : device(device), size(uboSize) {
  VkBufferCreateInfo uniformBufferInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = uboSize,
      .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE};
  CheckVkResult(vkCreateBuffer(device.logicalDevice, &uniformBufferInfo, nullptr, &this->uniformBuffer));

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device.logicalDevice, this->uniformBuffer, &memRequirements);

  VkMemoryAllocateInfo uniformBufferMemoryAllocatonInfo{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .allocationSize = memRequirements.size,
      .memoryTypeIndex = GFVL::findMemoryType(device.physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
  };
  CheckVkResult(vkAllocateMemory(device.logicalDevice, &uniformBufferMemoryAllocatonInfo, nullptr, &this->uniformBufferMemory));
  CheckVkResult(vkBindBufferMemory(device.logicalDevice, this->uniformBuffer, this->uniformBufferMemory, 0));

  VkDescriptorSetLayoutBinding binding{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS};

  VkDescriptorSetLayoutCreateInfo descriptorInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = 1,
      .pBindings = &binding};

  CheckVkResult(vkCreateDescriptorSetLayout(device.logicalDevice, &descriptorInfo, nullptr, &this->descriptorSetLayout));

  vkMapMemory(device.logicalDevice, this->uniformBufferMemory, 0, uboSize, 0, &this->data);

  memcpy(this->data, ubo, uboSize);

  vkUnmapMemory(
      device.logicalDevice,
      this->uniformBufferMemory);

  VkDescriptorPoolSize poolSize{
      .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1};

  VkDescriptorPoolCreateInfo poolInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .maxSets = 1,
      .poolSizeCount = 1,
      .pPoolSizes = &poolSize};

  CheckVkResult(vkCreateDescriptorPool(
      device.logicalDevice,
      &poolInfo,
      nullptr,
      &this->descriptorPool));

  VkDescriptorSetAllocateInfo descriptorSetAllocationInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = this->descriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts = &this->descriptorSetLayout};

  CheckVkResult(vkAllocateDescriptorSets(
      device.logicalDevice,
      &descriptorSetAllocationInfo,
      &this->descriptorSet));

  VkDescriptorBufferInfo descriptorBufferInfo{
      .buffer = this->uniformBuffer,
      .offset = 0,
      .range = uboSize};

  VkWriteDescriptorSet descriptorWrite{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = this->descriptorSet,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pBufferInfo = &descriptorBufferInfo};

  vkUpdateDescriptorSets(
      device.logicalDevice,
      1,
      &descriptorWrite,
      0,
      nullptr);
}
void UNIFORM_BUFFER::bind(VkCommandBuffer &commandBuffer, PIPELINE &pipeline, uint32_t set) {
  vkCmdBindDescriptorSets(
      commandBuffer,
      VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline.pipelineLayout,
      set,
      1,
      &descriptorSet,
      0,
      nullptr);
}
void UNIFORM_BUFFER::update(void *data) {
  void *mapped;
  vkMapMemory(device.logicalDevice, uniformBufferMemory, 0, size, 0, &mapped);
  memcpy(mapped, data, size);
  vkUnmapMemory(device.logicalDevice, uniformBufferMemory);
}
UNIFORM_BUFFER::~UNIFORM_BUFFER() {
  vkDestroyBuffer(device.logicalDevice, this->uniformBuffer, nullptr);
  vkFreeMemory(device.logicalDevice, this->uniformBufferMemory, nullptr);
vkDestroyDescriptorSetLayout(device.logicalDevice, this->descriptorSetLayout, nullptr);

  vkDestroyDescriptorPool(device.logicalDevice, this->descriptorPool, nullptr);
}
} // namespace GFVL
