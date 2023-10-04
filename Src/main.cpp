#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float4.hpp"
#include "vulkan/vulkan_core.h"
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <glm/mat4x4.hpp>
#include "window.hpp"

auto main(int argc, char** argv) -> int {
    std::cout << "Hello World" << '\n';
    if (!glfwInit()) {
        std::cout << "glfw failed to initialize" << '\n';
        glfwTerminate();
        return 1;
    }
    

    mini_engine::Window window(800, 600, "Vulkan Window", true, nullptr, nullptr);
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " Extensions supported\n";
    
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while (!glfwWindowShouldClose(window.get_window())) {
        glfwPollEvents();
    }

    return 0;
}