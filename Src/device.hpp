#include <vulkan/vulkan.hpp>

namespace mini_engine {
class Vulkan_Device {
public:
  explicit Vulkan_Device();
  ~Vulkan_Device();
  auto create_instance(std::string app_name) -> void;
private:
VkInstance m_vk_instace;
};
} // namespace mini_engine