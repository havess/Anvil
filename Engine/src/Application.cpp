#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include <Engine/Application.h>

namespace Engine {

void Application::createWindow() {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "glfwInit failed!" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);

  auto *monitor = glfwGetPrimaryMonitor();
  if (!monitor) {
    glfwTerminate();
    std::cerr << "Could not retrive primary monitor" << std::endl;
    std::abort();
  }

  // Create window
  mWindow = glfwCreateWindow(mWidth, mHeight, "Engine", NULL, NULL);
  if (mWindow == NULL) {
    std::cerr << "Could not create window" << std::endl;
    glfwTerminate();
    std::abort();
  }
  // Set framebuffer size to support all screen resolutions.
  glfwGetFramebufferSize(mWindow, &mFramebufferWidth, &mFramebufferHeight);
  glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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

Application::Application(int width, int height, int argc, char **argv)
    : mWidth(width),
    mHeight(height),
    mCamera(Engine::Camera{vec3(0, 3, 3), vec3(0, 1, 0), vec3(1, -3, -3)}) {

  /************ CREATE WINDOW AND CONTEXT ***********/
  createWindow();
  centerWindow();

  // Create context
  glfwMakeContextCurrent(mWindow);
  gl3wInit();

  mRenderer = std::make_unique<Renderer>();

  // Establish initial position for camera
  mWorldTranslation = glm::translate(
      mat4(1.0f), float(mScale) * -glm::normalize(mCamera.getPos()));
  mWorldTransform = mWorldTranslation * mWorldRotation;

  /************ INITIALIZE INPUT HANDLER ***********/
  mInputHandler = std::make_unique<InputHandler>(mWindow);

  /************ SET INPUT CALLBACKS *************/
  mInputHandler->setScrollCallback([this](double xoff, double yoff) {
    mScale += yoff;
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
  mUIManager.init(mWindow, /* glsl version */ "#version 330 core");
}

void Application::run() {
  glfwSwapInterval(1);

  do {
    // Update framebuffer size.
    glfwGetFramebufferSize(mWindow, &mFramebufferWidth, &mFramebufferHeight);
    glViewport(0, 0, mFramebufferWidth, mFramebufferHeight);

    // Per frame implementation specific update.
    float currentFrame = (float) glfwGetTime();
    tick(currentFrame);

    // Render all renderable objects.
    mRenderer->renderFrame(*this, mWorldTransform);

    // Check for input.
    mInputHandler->poll();

    // Draw UI.
    mUIManager.drawFrame();

    // Swap buffers
    glfwSwapBuffers(mWindow);
  } while (glfwWindowShouldClose(mWindow) == 0);

  mUIManager.shutdown();
  glfwTerminate();
}
} // namespace Engine
