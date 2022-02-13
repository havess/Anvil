#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <optional>

#include <Engine/Application.h>

namespace {
  constexpr std::array validationLayers{"VK_LAYER_KHRONOS_validation"};

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

    auto isComplete() {
      return graphicsFamily.has_value();
    }
  };

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());

    for (auto i = 0; i < families.size(); i++) {
      auto family = families[i];
      if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        indices.graphicsFamily = i;

      if (indices.isComplete())
        break;
    }

    return indices;
  }
}

namespace Engine {

Application::Application(int width, int height, int argc, char **argv)
    : mWidth(width),
    mHeight(height),
    mCamera(Engine::Camera{vec3(0, 3, 3), vec3(0, 1, 0), vec3(1, -3, -3)}) {}

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

  // TODO: Be smarter about device choice. We can either score devices
  // based off some heuristics or simply look for some devices with features
  // that we want. Maybe the Application can define a list of requirements?
  auto isDeviceSuitable = [](const VkPhysicalDevice &device) {
    auto indices = findQueueFamilies(device);
    return indices.isComplete();
  };

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
  auto indices = findQueueFamilies(mPhysicalDevice);

  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;

  auto queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pEnabledFeatures = &deviceFeatures;

  // Add validation layer data for compatibility with older
  // Vulkan implementations.
  createInfo.enabledExtensionCount = 0;

  if (mEnableValidationLayers) {
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else
    createInfo.enabledLayerCount = 0;

  if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS)
    throw std::runtime_error("Failed to create logical device.");

  vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
}

void Application::initVulkan() {
  createInstance();
  setupDebugMessenger();
  pickPhysicalDevice();
  createLogicalDevice();
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

  vkDestroyDevice(mDevice, nullptr);

  if (mEnableValidationLayers)
    DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);

  // Ignore callback by passing nullptr.
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
