#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>
#include <functional>

#include "Camera.h"
#include "Types.h"

namespace {
std::function<void(double, double)> mouseCallback;
std::function<void(double, double)> scrollCallback;
bool isFirstMouse = true;
double lastX;
double lastY;
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (isFirstMouse) {
    lastX = xpos;
    lastY = ypos;
    isFirstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;
  mouseCallback(xoffset, yoffset);
}
void scroll_callback(GLFWwindow *window, double x, double y) {
  scrollCallback(x, y);
}
} // namespace

class InputHandler {
public:
  InputHandler(GLFWwindow *window) : mWindow(window) {
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE);
  };
  inline void poll() { glfwPollEvents(); }
  inline void processInput(float deltaTime, sptr<Camera> camera) {
    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(mWindow, true);

    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
      camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
      camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
      camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
      camera->ProcessKeyboard(RIGHT, deltaTime);
  }
  inline bool getState(uint key) { return glfwGetKey(mWindow, key); }
  inline void setMouseCallback(std::function<void(double, double)> callback) {
    mouseCallback = callback;
    glfwSetCursorPosCallback(mWindow, mouse_callback);
  }
  inline void setScrollCallback(std::function<void(double, double)> callback) {
    scrollCallback = callback;
    glfwSetScrollCallback(mWindow, scroll_callback);
  }

private:
  GLFWwindow *mWindow;
};
