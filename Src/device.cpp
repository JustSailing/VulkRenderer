#include "device.hpp"
// #include "vk_video/vulkan_video_codecs_common.h"
#include "colorIO.hpp"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_enums.hpp"
#include "vulkan/vulkan_handles.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <iostream>
#include <vector>

namespace mini_engine {

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {

  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}
PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger) {
  return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator,
                                           pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT messenger,
    VkAllocationCallbacks const *pAllocator) {
  return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

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
  std::vector<const char *> ext = {"VK_EXT_debug_utils", "VK_EXT_debug_report",
                                   "VK_KHR_surface", "VK_KHR_xlib_surface"};
  std::vector<const char *> lay = {"VK_LAYER_KHRONOS_validation"};
  instance_create_info.setEnabledExtensionCount(ext.size());
  instance_create_info.setPpEnabledExtensionNames(ext.data());
  instance_create_info.setEnabledLayerCount(1);
  instance_create_info.setPpEnabledLayerNames(lay.data());
  result = vk::createInstance(&instance_create_info, nullptr, &m_vk_instance);
  if (result == vk::Result::eSuccess) {
    fprintf(stderr, "%svulkan instance created\n", CGRN("[SUCCESS] "));
  }
  m_dyn_loader =
      vk::DispatchLoaderDynamic(m_vk_instance, vkGetInstanceProcAddr);
  vk::DebugUtilsMessengerCreateInfoEXT createInfo =
      vk::DebugUtilsMessengerCreateInfoEXT(
          vk::DebugUtilsMessengerCreateFlagsEXT(),
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
              vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
              vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
          vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
              vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
              vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
          debugCallback, nullptr);
  m_debugger = m_vk_instance.createDebugUtilsMessengerEXT(createInfo, nullptr,
                                                          m_dyn_loader);

  m_physical_devs = m_vk_instance.enumeratePhysicalDevices();
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

Vulkan_Device::~Vulkan_Device() {
  m_vk_instance.destroyDebugUtilsMessengerEXT(m_debugger, nullptr,
                                              m_dyn_loader);
  m_vk_instance.destroy();
}
} // namespace mini_engine