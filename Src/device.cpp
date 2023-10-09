// System
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>
// Lib
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
// User Defined
#include "colorIO.hpp"
#include "device.hpp"

namespace mini_engine {

Vulkan_Device::Vulkan_Device(std::string app_name, bool debug)
    : m_debug{debug} {
  enumerate_props();
  create_instance(app_name);
}

auto Vulkan_Device::enumerate_props() -> void {
  // check which version the system can use
  uint32_t m_version{0};
  auto result = vk::enumerateInstanceVersion(&m_version);
  if (result == vk::Result::eSuccess && m_debug) {
    fprintf(stderr, "%s\n\tVariant: %d, Major: %d, Minor: %d, Patch: %d\n",
            CGRN("[Version Support] "), VK_API_VERSION_VARIANT(m_version),
            VK_API_VERSION_MAJOR(m_version), VK_API_VERSION_MINOR(m_version),
            VK_API_VERSION_PATCH(m_version));
  } else if (result != vk::Result::eSuccess) {
    fprintf(stderr, "%scould not get vulkan version\n", CRED("[FAILURE] "));
    exit(2);
  }

  // get layer properties and count
  uint32_t layer_count{0};
  result = vk::enumerateInstanceLayerProperties(&layer_count, nullptr);
  if (result == vk::Result::eSuccess && m_debug) {
    fprintf(stderr, "%sinstance layer count: %d\n", CGRN("[SUCCESS] "),
            layer_count);
  } else if (result != vk::Result::eSuccess) {
    fprintf(stderr, "%scould not get instance layer count: %d\n",
            CRED("[FAILURE] "), layer_count);
    exit(2);
  }
  m_layer_props.resize(layer_count);
  result =
      vk::enumerateInstanceLayerProperties(&layer_count, m_layer_props.data());
  if (result == vk::Result::eSuccess && m_debug) {
    fprintf(stderr, "%s\n", CGRN("[Layer Properties Supported]"));
    for (auto const &prop : m_layer_props) {
      fprintf(stderr, "\t%s\n", prop.layerName.data());
    }
  } else if (result != vk::Result::eSuccess) {
    fprintf(stderr, "%spopulating layers into vector failed\n",
            CRED("[FAILURE] "));
    exit(2);
  }

  // get instance extension properties
  uint32_t extension_count{0};
  result = vk::enumerateInstanceExtensionProperties(nullptr, &extension_count,
                                                    nullptr);
  m_ext_props.resize(extension_count);
  result = vk::enumerateInstanceExtensionProperties(nullptr, &extension_count,
                                                    m_ext_props.data());
  if (result == vk::Result::eSuccess && m_debug) {
    fprintf(stderr, "%s\n", CGRN("[Extension Properties Supported]"));
    for (auto const &prop : m_ext_props) {
      fprintf(stderr, "\t%s\n", prop.extensionName.data());
    }
  } else if (result != vk::Result::eSuccess) {
    fprintf(stderr, "%spopulating extension into vector failed\n",
            CRED("[FAILURE] "));
    exit(2); 
  }
}

auto Vulkan_Device::create_instance(const std::string &app_name) -> void {
  vk::ApplicationInfo app_info(app_name.c_str(), VK_MAKE_VERSION(0, 1, 0),
                               "mini_engine", VK_MAKE_VERSION(0, 1, 0),
                               m_version, nullptr);

  vk::InstanceCreateInfo instance_create_info({}, &app_info);
  std::vector<const char *> ext;
  std::vector<const char *> lay;
  if (m_debug) {
    ext = {"VK_EXT_debug_utils", "VK_EXT_debug_report", "VK_KHR_surface",
           "VK_KHR_xlib_surface"};
    lay = {"VK_LAYER_KHRONOS_validation"};
  } else {
    ext.push_back("VK_KHR_surface");ext.push_back("VK_KHR_xlib_surface");
  }
  instance_create_info.setEnabledExtensionCount(ext.size());
  instance_create_info.setPpEnabledExtensionNames(ext.data());
  instance_create_info.setEnabledLayerCount(lay.size());
  instance_create_info.setPpEnabledLayerNames(lay.data());
  auto result =
      vk::createInstance(&instance_create_info, nullptr, &m_vk_instance);
  if (result == vk::Result::eSuccess && m_debug) {
    fprintf(stderr, "%svulkan instance created\n", CGRN("[SUCCESS] "));
  } else if (result != vk::Result::eSuccess) {
    fprintf(stderr, "%svulkan instance not created\n", CGRN("[FAILURE] "));
    exit(2);
  }
  if (m_debug) {
    m_dyn_loader =
        vk::DispatchLoaderDynamic(m_vk_instance, vkGetInstanceProcAddr);
    vk::DebugUtilsMessengerCreateInfoEXT createInfo =
        vk::DebugUtilsMessengerCreateInfoEXT(
            vk::DebugUtilsMessengerCreateFlagsEXT(),
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            &debugCallback, nullptr);
    m_debug_messenger = m_vk_instance.createDebugUtilsMessengerEXT(
        createInfo, nullptr, m_dyn_loader);
  }
  m_physical_devs = m_vk_instance.enumeratePhysicalDevices();
  if (m_debug) {
    fprintf(stderr, "%s\n", CGRN("[Physical Device Available]"));
    // NOTE: Move the following for loop to its own function
    for (auto const &dev : m_physical_devs) {
      vk::PhysicalDeviceProperties props = dev.getProperties();

      fprintf(stderr, "\t%s\n", props.deviceName.data());
      auto exts = dev.enumerateDeviceExtensionProperties();
      auto layers = dev.enumerateDeviceLayerProperties();
      fprintf(stderr, "\t\t%s\n", CGRN("[Physical Device Extension Support]"));
      for (auto const &e : exts) {
        fprintf(stderr, "\t\t\t%s\n", e.extensionName.data());
      }
      fprintf(stderr, "\t\t%s\n", CGRN("[Physical Device Layer Support]"));
      for (auto const &e : layers) {
        fprintf(stderr, "\t\t\t%s: %s\n", e.layerName.data(),
                e.description.data());
      }
      auto some = dev.getQueueFamilyProperties();
      // auto next = dev.get;
      auto type = [](const vk::PhysicalDeviceType &ty) {
        switch (ty) {
        case vk::PhysicalDeviceType::eOther:
          return "Physical Device Type: Other";
        case vk::PhysicalDeviceType::eIntegratedGpu:
          return "Physical Device Type: Integrated GPU";
        case vk::PhysicalDeviceType::eDiscreteGpu:
          return "Physical Device Type: Discrete GPU";
        case vk::PhysicalDeviceType::eVirtualGpu:
          return "Physical Device Type: Virtual GPU";
        case vk::PhysicalDeviceType::eCpu:
          return "Physical Device Type: Other";
          break;
        }
        return "Physical Device Type: Unknown";
      };
      fprintf(stderr, "\t\t%s\n", type(props.deviceType));
      for (auto const &q : some) {
        if (q.queueFlags | vk::QueueFlagBits::eGraphics)
          fprintf(stderr, "\t\t\t%s\n", CGRN("GRAPHICS SUPPORTED"));
        else
          fprintf(stderr, "\t\t%s\n", CRED("GRAPHICS  NOT SUPPORTED"));
        if (q.queueFlags | vk::QueueFlagBits::eCompute)
          fprintf(stderr, "\t\t\t%s\n", CGRN("COMPUTE SUPPORTED"));
        else
          fprintf(stderr, "\t\t%s", CRED("COMPUTE NOT SUPPORTED"));
        if (q.queueFlags | vk::QueueFlagBits::eOpticalFlowNV)
          fprintf(stderr, "\t\t\t%s\n", CGRN("OPTICAL FLOW SUPPORTED"));
        else
          fprintf(stderr, "\t\t\t%s\n", CRED("OPTICAL FLOW NOT SUPPORTED"));
        if (q.queueFlags | vk::QueueFlagBits::eSparseBinding)
          fprintf(stderr, "\t\t\t%s\n", CGRN("SPARSE BINDING SUPPORTED"));
        else
          fprintf(stderr, "\t\t\t%s\n", CRED("SPARSE BINDING NOT SUPPORTED"));
        if (q.queueFlags | vk::QueueFlagBits::eVideoDecodeKHR)
          fprintf(stderr, "\t\t\t%s\n", CGRN("VIDEO DECODE SUPPORTED"));
        else
          fprintf(stderr, "\t\t\t%s\n", CRED("VIDEO DECODE NOT SUPPORTED"));
        if (q.queueFlags | vk::QueueFlagBits::eTransfer)
          fprintf(stderr, "\t\t\t%s\n", CGRN("TRANSFER SUPPORTED"));
        else
          fprintf(stderr, "\t\t\t%s\n", CRED("TRANSFER NOT SUPPORTED"));
      }
    }
  }
}

Vulkan_Device::~Vulkan_Device() {
  if (m_debug) {
    m_vk_instance.destroyDebugUtilsMessengerEXT(m_debug_messenger, nullptr,
                                                m_dyn_loader);
  }
  m_vk_instance.destroy();
}
} // namespace mini_engine