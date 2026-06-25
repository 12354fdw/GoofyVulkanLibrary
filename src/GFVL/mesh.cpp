#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <cstring>
#include <vulkan/vulkan.h>

#include "GFVL.hpp"
using namespace GFVL;

// USER-DEFINED STUFF
namespace GFVL {
MESH::MESH(void *data, uint32_t size, DEVICE &device) : device(device), vertexBuffer(device, size, data) {
  this->mesh_size = size;
}
MESH::~MESH() {

}
}
