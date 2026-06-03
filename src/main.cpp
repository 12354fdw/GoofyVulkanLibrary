#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
// Remove-Item build -Recurse -Force; cmake -B build -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64 -G Ninja
// cmake --build build --verbose
int main() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window = SDL_CreateWindow("Test", 800, 600, SDL_WINDOW_VULKAN);
  VkApplicationInfo appInfo{ // structure specifying application information
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, // honestly dont know why it exists its just the type of the struct
    .pNext = NULL, // can be a pointer to a structure extending this structure
    .pApplicationName = "goofyVLib", // name
    .applicationVersion = 1, // version of application
    .apiVersion = VK_API_VERSION_1_4\
  };
  SDL_Delay(3000);

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
