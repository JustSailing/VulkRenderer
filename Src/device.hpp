// System
#include <iostream>
#include <string>
#include <vector>
// Lib
#include <vulkan/vulkan.hpp>

namespace mini_engine {
class Vulkan_Device {
public:
  explicit Vulkan_Device(std::string app_name,
                         [[maybe_unused]] bool debug = false);
  ~Vulkan_Device();

private:
  /// Member functions
  auto enumerate_props(void) -> void;
  auto create_instance(const std::string &app_name) -> void;

  /// Member variables
  bool m_debug;
  vk::Instance m_vk_instance{};
  // supported vulkan api version
  uint32_t m_version;
  // supported extension properties
  std::vector<vk::ExtensionProperties> m_ext_props;
  // supported layer properties
  std::vector<vk::LayerProperties> m_layer_props;
  // dynamic loader to run function not present statically
  vk::DispatchLoaderDynamic m_dyn_loader;
  // debugger
  vk::DebugUtilsMessengerEXT m_debug_messenger;
  // physical devices
  std::vector<vk::PhysicalDevice> m_physical_devs;
};

[[maybe_unused]] static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              [[maybe_unused]] void *pUserData) {

  auto type = [](VkDebugUtilsMessageTypeFlagsEXT ty) {
    switch (ty) {
    case VkDebugUtilsMessageTypeFlagBitsEXT::
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
      return "[Validation]";
    case VkDebugUtilsMessageTypeFlagBitsEXT::
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
      return "[General]";
    case VkDebugUtilsMessageTypeFlagBitsEXT::
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
      return "[Performance]";
      break;
    }
    return "[Unknown]";
  };

  auto sever = [](VkDebugUtilsMessageSeverityFlagBitsEXT s) {
    switch (s) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      return "[Verbose]";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      return "[Info]";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      return "[Warning]";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      return "[Error]";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
      return "[Unknown]";
      break;
    }
    return "[Unknown]";
  };

  std::cerr << type(messageType) << " " << sever(messageSeverity)
            << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}
} // namespace mini_engine