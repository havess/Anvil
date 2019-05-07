#pragma once

#include "Camera.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "Types.h"
#include "UIManager.h"
#include <GLFW/glfw3.h>

namespace Anvil {

class Window {
public:
  Window();
  virtual ~Window() = default;
  void run();

  inline void setRenderer(sptr<Renderer> renderer) { mRenderer = renderer; }
  inline uint getWidth() const { return mWidth; }
  inline uint getHeight() const { return mHeight; }
  inline mat4 getViewMatrix() const { return mCamera->GetViewMatrix(); }
  inline float getZoom() const { return mCamera->Zoom; }

private:
  uint mWidth, mHeight;
  GLFWwindow *mWindow;
  uptr<InputHandler> mInputHandler;
  sptr<Camera> mCamera;
  sptr<UIManager> mUIManager;
  sptr<Renderer> mRenderer;
  float mLastFrame = 0.0f;
};

} // namespace Anvil
