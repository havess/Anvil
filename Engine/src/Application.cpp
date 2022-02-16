#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <optional>
#include <set>

#include <Engine/Application.h>

namespace {
  constexpr std::array validationLayers{"VK_LAYER_KHRONOS_validation"};
  constexpr std::array deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  auto checkValidationLayers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (auto *layer : validationLayers) {
      bool found = false;
      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layer, layerProperties.layerName) == 0){
          found = true;
          break;
        }
      }

      if (!found)
        return false;
    }

    return true;
  }

  auto getRequiredExtensions(bool enableValidationLayers) {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }

  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
      createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
      createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
      createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
      createInfo.pfnUserCallback = debugCallback;
  }

  // Proxy functions to load in the debug messenger create/delete functions since they
  // are an extension.
  VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto fn = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (fn != nullptr)
      return fn(instance, pCreateInfo, pAllocator, pDebugMessenger);
    
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (fn != nullptr)
        fn(instance, debugMessenger, pAllocator);
  }

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    auto isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());

    for (auto i = 0; i < families.size(); i++) {
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

      if (presentSupport)
        indices.presentFamily = i;
      
      auto family = families[i];

      if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        indices.graphicsFamily = i;

      if (indices.isComplete())
        break;
    }

    return indices;
  }

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto &availableFormat: availableFormats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8_SRGB &&
          availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
          return availableFormat;
    }

    // Just settle for the first format if we don't have our
    // preferred format.
    return availableFormats[0];
  }

  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes) {
    for (const auto &mode: availableModes) {
      if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        return mode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D chooseSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX)
      return capabilities.currentExtent;
    
    // Screen coordinates don't always correspond to pixel
    // coordinates due to screens with high DPI like a retina screen.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    VkExtent2D actualExtent = {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, 
      capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height,
      capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

namespace Engine {

Application::Application(int width, int height, int argc, char **argv)
    : mWidth(width),
    mHeight(height),
    mCamera(Engine::Camera{vec3(0, 3, 3), vec3(0, 1, 0), vec3(1, -3, -3)}) {}

bool Application::isDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = findQueueFamilies(device, mSurface);

  bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    auto swapChainSupport = querySwapChainSupport(device);
    bool has_formats = !swapChainSupport.formats.empty();
    bool has_present_modes = !swapChainSupport.presentModes.empty();
    swapChainAdequate = has_formats && has_present_modes;
  }

  return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Application::checkDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

  for (const auto &extension: availableExtensions) {
    if (requiredExtensions.empty())
      return true;
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

Application::SwapChainSupportDetails
Application::querySwapChainSupport(VkPhysicalDevice device) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

  if (formatCount) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

  if (presentModeCount) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

void Application::initWindow() {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "glfwInit failed!" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
  /*glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);*/

  auto *monitor = glfwGetPrimaryMonitor();
  if (!monitor) {
    glfwTerminate();
    std::cerr << "Could not retrive primary monitor" << std::endl;
    std::abort();
  }

  // Create window
  mWindow = glfwCreateWindow(mWidth, mHeight, "Engine", nullptr, nullptr);
  if (mWindow == NULL) {
    std::cerr << "Could not create window" << std::endl;
    glfwTerminate();
    std::abort();
  }
  // Set framebuffer size to support all screen resolutions.
  glfwGetFramebufferSize(mWindow, &mFramebufferWidth, &mFramebufferHeight);
  glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  centerWindow();  
  // Create context
  glfwMakeContextCurrent(mWindow);
  glfwSwapInterval(1);
}

void Application::centerWindow() {
  glfwGetWindowSize(mWindow, &mWidth, &mHeight);
  auto *monitor = glfwGetPrimaryMonitor();
  if (!monitor)
    return;
  const auto *videoMode = glfwGetVideoMode(monitor);
  auto x = (videoMode->width - mWidth) / 2;
  auto y = (videoMode->height - mHeight) / 2;
  glfwSetWindowPos(mWindow, x, y);
}

void Application::createInstance() {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Anvil";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto glfwExtensions = getRequiredExtensions(mEnableValidationLayers);
  createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
  createInfo.ppEnabledExtensionNames = glfwExtensions.data();

  if (mEnableValidationLayers && !checkValidationLayers())
    throw std::runtime_error("Validation layers requested, but are not available.");

  // Create an additional debug utils messenger here that will pickup the instance
  // creation and be destroyed immediately following this.
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (mEnableValidationLayers) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
  }

  uint32_t extensionCount = 0;
  // Get number of extensions before we get extension details.
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions(extensionCount);
  // Get extension details.
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  std::cout << "available extensions:\n";

  for (const auto& extension : extensions)
      std::cout << '\t' << extension.extensionName << '\n';
  
  if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan instance!");
  }
}

void Application::setupDebugMessenger() {
  if (!mEnableValidationLayers)
    return;
  
  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  populateDebugMessengerCreateInfo(createInfo);

  if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}

void Application::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

  if (!deviceCount)
    throw std::runtime_error("Failed to find GPUs supporting Vulkan...");

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (isDeviceSuitable(device)) {
      mPhysicalDevice = device;
      break;
    }
  }

  if (mPhysicalDevice == VK_NULL_HANDLE)
    throw std::runtime_error("Could not find a suitable GPU device...");
}

void Application::createLogicalDevice() {
  auto indices = findQueueFamilies(mPhysicalDevice, mSurface);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {
    indices.graphicsFamily.value(), indices.presentFamily.value()
  };

  float queuePriority = 1.0f;
  for (auto family : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = family;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (mEnableValidationLayers) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else
    createInfo.enabledLayerCount = 0;

  if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS)
    throw std::runtime_error("Failed to create logical device.");

  vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
  vkGetDeviceQueue(mDevice, indices.presentFamily.value(), 0, &mPresentQueue);
}

void Application::createSurface() {
  if (glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface) != VK_SUCCESS)
    throw std::runtime_error("Failed to create window surface.");
}

void Application::createSwapChain() {
  auto swapChainSupport = querySwapChainSupport(mPhysicalDevice);
  auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
  auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
  auto extent = chooseSwapExtent(mWindow, swapChainSupport.capabilities);

  // Request one more image in our swapchain than the minimum to avoid
  // waiting on the driver to complete internal operations.
  uint32_t imageCnt = swapChainSupport.capabilities.minImageCount + 1;

  // 0 is a special value which means no maximum number of images.
  auto maxImageCount = swapChainSupport.capabilities.maxImageCount;
  if (maxImageCount > 0 && imageCnt > maxImageCount)
    imageCnt = maxImageCount;

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = mSurface;
  createInfo.minImageCount = imageCnt;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  auto indices = findQueueFamilies(mPhysicalDevice, mSurface);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

  // If we have separate queues we need to decide how to handle
  // swap chain images. 
  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  // No pre-transform just use the current.
  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;

  // Enable clipping pixels.
  createInfo.clipped = VK_TRUE;

  // When recreating the swapchain we need to provide the
  // last swapchain. For now let's imagine we won't be resizing
  // the window.
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS)
    throw std::runtime_error("Failed to create swap chain.");

  vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCnt, nullptr);
  mSwapChainImages.resize(imageCnt);
  vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCnt, mSwapChainImages.data());

  mSwapChainImageFormat = surfaceFormat.format;
  mSwapChainExtent = extent;
}

void Application::createImageViews() {
  mSwapChainImageViews.resize(mSwapChainImages.size());

  for (size_t i = 0; i < mSwapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = mSwapChainImages[i];
    // View type tells vulkan how to treat images (1D textures, 2D textures, 3D textures, etc.)
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = mSwapChainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    // Images will be used as color color targets
    // without mipmapping or multiple layers.
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(mDevice, &createInfo, nullptr, &mSwapChainImageViews[i]) != VK_SUCCESS)
      throw std::runtime_error("Failed to create image view.");
  }
}

void Application::initVulkan() {
  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createImageViews();
}

void Application::initEngine() {
  mRenderer = std::make_unique<Renderer>();

  // Establish initial position for camera
  mWorldTranslation = glm::translate(
      mat4(1.0f), float(mScale) * -glm::normalize(mCamera.getPos()));
  mWorldTransform = mWorldTranslation * mWorldRotation;

  /************ INITIALIZE INPUT HANDLER ***********/
  mInputHandler = std::make_unique<InputHandler>(mWindow);

  /************ SET INPUT CALLBACKS *************/
  mInputHandler->setScrollCallback([this](double xoff, double yoff) {
    mScale += (float) yoff;
    if (mScale < 1.0f)
      mScale = 1.0f;
    else if (mScale > 300.0f)
      mScale = 300.0f;

    mWorldTranslation = glm::translate(
        mat4(1.0f), float(mScale) * -glm::normalize(mCamera.getPos()));
    mWorldTransform = mWorldTranslation * mWorldRotation;
  });

  /************ INITIALIZE UI *************/
  // Init ImGui.
  //mUIManager.init(mWindow, /* glsl version */ "#version 330 core");
}

void Application::cleanup() {
  //mUIManager.shutdown();

  for (auto imageView : mSwapChainImageViews)
    vkDestroyImageView(mDevice, imageView, nullptr);

  vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
  vkDestroyDevice(mDevice, nullptr);

  if (mEnableValidationLayers)
    DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);

  // Ignore callback by passing nullptr.
  vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  vkDestroyInstance(mInstance, nullptr);
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}


void Application::run() {
  initWindow();
  initVulkan();
  /*initEngine();

  do {
    // Update framebuffer size.
    glfwGetFramebufferSize(mWindow, &mFramebufferWidth, &mFramebufferHeight);
    //glViewport(0, 0, mFramebufferWidth, mFramebufferHeight);

    // Per frame implementation specific update.
    float currentFrame = (float) glfwGetTime();
    tick(currentFrame);

    // Render all renderable objects.
    //mRenderer->renderFrame(*this, mWorldTransform);

    // Check for input.
    mInputHandler->poll();

    // Draw UI.
    //mUIManager.drawFrame();

    // Swap buffers
    glfwSwapBuffers(mWindow);
  } while (glfwWindowShouldClose(mWindow) == 0);*/

  cleanup();
}
} // namespace Engine
