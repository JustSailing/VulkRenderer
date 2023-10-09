#include "device.hpp"
// #include "vk_video/vulkan_video_codecs_common.h"
#include "colorIO.hpp"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <iostream>
#include <vector>

namespace mini_engine {

Vulkan_Device::Vulkan_Device(std::string app_name, bool debug) {
  // check which version the system can use
  uint32_t m_version{0};
  auto result = vk::enumerateInstanceVersion(&m_version);
  if (result == vk::Result::eSuccess) {
    fprintf(stderr, "%s\n\tVariant: %d, Major: %d, Minor: %d, Patch: %d\n",
            CGRN("[Version Support] "), VK_API_VERSION_VARIANT(m_version),
            VK_API_VERSION_MAJOR(m_version), VK_API_VERSION_MINOR(m_version),
            VK_API_VERSION_PATCH(m_version));
  }

  uint32_t layer_count{0};
  result = vk::enumerateInstanceLayerProperties(&layer_count, nullptr);
  if (result == vk::Result::eSuccess) {
    fprintf(stderr, "%sinstance layer count: %d\n", CGRN("[SUCCESS] "),
            layer_count);
  }
  m_layer_props.resize(layer_count);
  result =
      vk::enumerateInstanceLayerProperties(&layer_count, m_layer_props.data());
  if (result != vk::Result::eSuccess) {
    fprintf(stderr, "%spopulating layers into vector failed\n",
            CRED("[FAILURE] "));
  }
  fprintf(stderr, "%s\n", CGRN("[Layer Properties Supported]"));
  for (auto const &prop : m_layer_props) {
    fprintf(stderr, "\t%s\n", prop.layerName.data());
  }

  uint32_t extension_count{0};
  result = vk::enumerateInstanceExtensionProperties(nullptr, &extension_count,
                                                    nullptr);
  // std::cout << result << "\n";
  m_ext_props.resize(extension_count);
  result = vk::enumerateInstanceExtensionProperties(nullptr, &extension_count,
                                                    m_ext_props.data());
  // std::cout << result << "\n";
  fprintf(stderr, "%s\n", CGRN("[Extension Properties Supported]"));
  for (auto const &prop : m_ext_props) {
    fprintf(stderr, "\t%s\n", prop.extensionName.data());
  }

  vk::ApplicationInfo app_info(app_name.c_str(), VK_MAKE_VERSION(0, 1, 0),
                               "mini_engine", VK_MAKE_VERSION(0, 1, 0),
                               m_version, nullptr);

  vk::InstanceCreateInfo instance_create_info({}, &app_info);
  result = vk::createInstance(&instance_create_info, nullptr, &m_vk_instance);
  if (result == vk::Result::eSuccess) {
    fprintf(stderr, "%svulkan instance created\n", CGRN("[SUCCESS] "));
  }
  m_physical_devs = m_vk_instance.enumeratePhysicalDevices();
  fprintf(stderr, "%s\n", CGRN("[Physical Device Available]"));
  for (auto const &dev : m_physical_devs) {
    vk::PhysicalDeviceProperties props = dev.getProperties();
    auto some = dev.getQueueFamilyProperties();
    // auto next = dev.get;
    fprintf(stderr, "\t%s\n", props.deviceName.data());
    auto type = [](const vk::PhysicalDeviceType & ty) {
      switch (ty) {
      case vk::PhysicalDeviceType::eOther: return "Physical Device Type: Other";
      case vk::PhysicalDeviceType::eIntegratedGpu: return "Physical Device Type: Integrated GPU"; 
      case vk::PhysicalDeviceType::eDiscreteGpu: return "Physical Device Type: Discrete GPU";
      case vk::PhysicalDeviceType::eVirtualGpu: return "Physical Device Type: Virtual GPU";
      case vk::PhysicalDeviceType::eCpu: return "Physical Device Type: Other";
        break;
      }
    };
    fprintf(stderr, "\t%s\n", type(props.deviceType));
    for (auto const &q : some) {
      if (q.queueFlags | vk::QueueFlagBits::eGraphics)
      fprintf(stderr, "\t\t%s\n", CGRN("GRAPHICS SUPPORTED"));
      else fprintf(stderr, "\t\t%s\n", CRED("GRAPHICS  NOT SUPPORTED"));
      if (q.queueFlags | vk::QueueFlagBits::eCompute)
      fprintf(stderr, "\t\t%s\n", CGRN("COMPUTE SUPPORTED"));
      else fprintf(stderr, "\t\t%s", CRED("COMPUTE NOT SUPPORTED"));
      if (q.queueFlags | vk::QueueFlagBits::eOpticalFlowNV)
      fprintf(stderr, "\t\t%s\n", CGRN("OPTICAL FLOW SUPPORTED"));
      else fprintf(stderr, "\t\t%s\n", CRED("OPTICAL FLOW NOT SUPPORTED"));
      if (q.queueFlags | vk::QueueFlagBits::eSparseBinding)
      fprintf(stderr, "\t\t%s\n", CGRN("SPARSE BINDING SUPPORTED"));
      else fprintf(stderr, "\t\t%s\n", CRED("SPARSE BINDING NOT SUPPORTED"));
      if (q.queueFlags | vk::QueueFlagBits::eVideoDecodeKHR)
      fprintf(stderr, "\t\t%s\n", CGRN("VIDEO DECODE SUPPORTED"));
      else fprintf(stderr, "\t\t%s\n", CRED("VIDEO DECODE NOT SUPPORTED"));
      if (q.queueFlags | vk::QueueFlagBits::eTransfer)
      fprintf(stderr, "\t\t%s\n", CGRN("TRANSFER SUPPORTED"));
      else fprintf(stderr, "\t\t%s\n", CRED("TRANSFER NOT SUPPORTED"));
    }
  }

}

Vulkan_Device::~Vulkan_Device() { m_vk_instance.destroy(); }
} // namespace mini_engine