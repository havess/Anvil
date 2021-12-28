#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>
#include <functional>

#include "Camera.h"
#include "Types.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

/********* HELPER FUNCTIONS *********/
namespace {

std::vector<std::function<void(int, int)>> keyCallbacks;
std::function<void(double, double)> mouseCallback;
std::function<void(double, double)> scrollCallback;
std::function<void(int, int)> buttonCallback;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  for (auto &cb : keyCallbacks)
    cb(key, action);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
    mouseCallback(xpos, ypos);
}

void scroll_callback(GLFWwindow *window, double x, double y) {
  if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
    scrollCallback(x, y);
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
    buttonCallback(button, action);
}

} // namespace

namespace Engine {

/// Input handeler/manager, allows callbacks to be added from anywhere in the
/// codebase. Eliminates need for one very large input handler function.
class InputHandler {
public:
  InputHandler(GLFWwindow *window) : mWindow(window) {
    glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, GL_TRUE);
  };

  inline void poll() { glfwPollEvents(); }

  inline void addKeyCallback(std::function<void(int, int)> callback) {
    keyCallbacks.push_back(callback);
    glfwSetKeyCallback(mWindow, key_callback);
  }

  inline bool getState(uint key) const { return glfwGetKey(mWindow, key); }

  inline void setMouseCallback(std::function<void(double, double)> callback) {
    mouseCallback = callback;
    glfwSetCursorPosCallback(mWindow, mouse_callback);
  }

  inline void setScrollCallback(std::function<void(double, double)> callback) {
    scrollCallback = callback;
    glfwSetScrollCallback(mWindow, scroll_callback);
  }

  inline void setMouseButtonCallback(std::function<void(int, int)> callback) {
    buttonCallback = callback;
    glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
  }

private:
  GLFWwindow *mWindow;
};

} // namespace Engine
