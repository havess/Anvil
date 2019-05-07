#pragma once
#include <GLFW/glfw3.h>
#include <functional>
#include <vector>
#include "Types.h"

namespace Anvil {

class UIManager {
public:
  UIManager();

  /// Startup and shutdown.
  void init(GLFWwindow *window, const std::string &glsl_version);
  void shutdown();
  void drawFrame();
  void registerWidget(std::function<void()> widgetRender);

private:
  std::vector<std::function<void()>> mWidgets;
};

} // namespace Anvil
