#pragma once
#include <GL/gl3w.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "Types.h"
#include "UIManager.h"

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
  inline sptr<UIManager> getUI() { return mUIManager; }
  inline const sptr<Camera> getCamera() const { return mCamera; }
  inline uint getWidth() const { return mWidth; }
  inline uint getHeight() const { return mHeight; }
  inline uint getFramebufferWidth() const { return mFramebufferWidth; }
  inline uint getFramebufferHeight() const { return mFramebufferHeight; }
  inline mat4 getViewMatrix() const { return mCamera->getViewMatrix(); }
  inline mat4 getProjMatrix() const {
    return glm::perspective(
        glm::radians(45.0f),
        (float)mFramebufferWidth / (float)mFramebufferHeight, 0.1f, 1000.0f);
  }

  inline std::string getAssetPath(std::string asset) const {
    return std::string{asset};
  }

protected:
  int mWidth, mHeight;
  int mFramebufferWidth, mFramebufferHeight;
  uptr<InputHandler> mInputHandler;
  sptr<Camera> mCamera;
  sptr<UIManager> mUIManager;
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
  void createWindow();
  void centerWindow();
};

} // namespace Engine
