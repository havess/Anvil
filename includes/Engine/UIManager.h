#pragma once
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <vector>
#include "Types.h"

namespace Engine {

/// class that allows us to create a custom window/widget from anywhere
/// in the codebase.
class UIManager {
public:
  UIManager();

  void init(GLFWwindow *window, const std::string &glsl_version);
  void shutdown();
  void drawFrame();
  void registerWidget(std::string name,
                      std::function<void(bool *)> widgetRender);

private:
  std::vector<std::function<void(bool *)>> mWidgets;
  std::vector<std::pair<std::string, bool>> mShowWidget;
};

} // namespace Engine
