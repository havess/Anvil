#pragma once
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "Types.h"
//#include "UIManager.h"

struct GLFWwindow;
namespace Engine {

/// This is the basis for all applications that will use the 3D
/// framework/engine.
class Application {
public:
  Application(int width, int height, int argc, char **argv);
  virtual ~Application() = default;
  virtual void run();
  /// Applications implement this for per frame updates.
  virtual void tick(float deltaTime) {}

  Renderer &getRenderer() { return *mRenderer; }
  //UIManager &getUI() { return mUIManager; }
  inline void attachCamera(Camera &cam) { mCamera = cam; }
  inline uint getWidth() const { return mWidth; }
  inline uint getHeight() const { return mHeight; }
  inline uint getFramebufferWidth() const { return mFramebufferWidth; }
  inline uint getFramebufferHeight() const { return mFramebufferHeight; }
  inline mat4 getViewMatrix() const { return mCamera.getViewMatrix(); }
  inline mat4 getProjMatrix() const { return mCamera.getProjMatrix(*this); }

protected:
  int mWidth, mHeight;
  int mFramebufferWidth, mFramebufferHeight;
  uptr<InputHandler> mInputHandler;
  Camera &mCamera;
  //UIManager mUIManager;
  uptr<Renderer> mRenderer;

  float mScale = 25.0f;
  mat4 mWorldTransform{1.0f};
  mat4 mWorldTranslation{1.0f};
  mat4 mWorldRotation{1.0f};

  inline void setShouldCloseWindow() {
    glfwSetWindowShouldClose(mWindow, true);
  }

private:
  GLFWwindow *mWindow;
  VkInstance mInstance;
  VkDebugUtilsMessengerEXT mDebugMessenger;
  VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
  VkDevice mDevice;
  VkQueue mGraphicsQueue;

  #ifdef NDEBUG
    const bool mEnableValidationLayers = false;
  #else
    const bool mEnableValidationLayers = true;
  #endif

  void initWindow();
  void initVulkan();
  void initEngine();
  void cleanup();

  void createInstance();
  /// When validation layers are supported this function
  /// will setup the debug messenger callback function that
  /// will relay the messages to our application instead of
  /// straight to stdout.
  void setupDebugMessenger();
  /// This function will pick the graphics card on the system
  /// to use.
  void pickPhysicalDevice();
  /// This function creates a logical device for the physical
  /// device to interface with.
  void createLogicalDevice();
  void centerWindow();
};

} // namespace Engine
