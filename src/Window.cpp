#include <GL/glew.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>

#include "Anvil/Window.h"

namespace Anvil {

Window::Window() {
  glewExperimental = true;
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return;
  }
  glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // We don't want the old OpenGL
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  mWidth = mode->width;
  mHeight = mode->height;
  mWindow =
      glfwCreateWindow(mWidth, mHeight, "Anvil", glfwGetPrimaryMonitor(), NULL);
  if (mWindow == NULL) {
    fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, "
                    "they are not 3.3 compatible.\n");
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(mWindow); // Initialize GLEW
  glewExperimental = true;         // Needed in core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return;
  }
  mCamera = std::make_unique<Camera>();
  mInputHandler = std::make_unique<InputHandler>(mWindow);
  mInputHandler->setScrollCallback(
      [this](double xoff, double yoff) { mCamera->ProcessMouseScroll(yoff); });
  mInputHandler->setMouseCallback([this](double xoffset, double yoffset) {
    mCamera->ProcessMouseMovement(xoffset, yoffset);
  });

  mUIManager = std::make_shared<UIManager>();
  // Init ImGui.
  mUIManager->init(mWindow, /* glsl version */ "#version 330 core");
}

void Window::run() {
  do {
    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - mLastFrame;
    mLastFrame = currentFrame;
    mRenderer->renderFrame(*this);
    mInputHandler->poll();
    mInputHandler->processInput(deltaTime, mCamera);
    mUIManager->drawFrame();
    // Swap buffers
    glfwSwapBuffers(mWindow);

  } while (glfwWindowShouldClose(mWindow) == 0);

  mUIManager->shutdown();
  glfwTerminate();
}
} // namespace Anvil

