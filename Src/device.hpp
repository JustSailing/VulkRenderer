#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace mini_engine {
class Vulkan_Device {
public:
  explicit Vulkan_Device(std::string app_name, bool debug);
  ~Vulkan_Device();

private:
  /// Member functions
  auto create_instance(std::string app_name) -> void;

  /// Member variables
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
  vk::DebugUtilsMessengerEXT m_debugger;
  // physical devices
  std::vector<vk::PhysicalDevice> m_physical_devs;
};
} // namespace mini_engine