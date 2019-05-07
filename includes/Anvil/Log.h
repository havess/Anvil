#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "imgui/imgui.h"
#include <GL/glew.h>
namespace Anvil {
class Log {
public:
  enum class Category { Debug, Error, Info };
  static Log &getInstance() {
    static Log instance;
    return instance;
  }
  Log(Log const &) = delete;
  void operator=(Log const &) = delete;
  void clear();

  void addLog(Category cat, const char *TAG, const char *fmt, ...);
  void draw(bool *p_open = nullptr);

private:
  Log();
  ImGuiTextBuffer mBuf;
  ImGuiTextFilter mFilter;
  // Index to lines offset. We maintain this with AddLog()
  // calls, allowing us to have a random access on lines
  std::vector<int> mLineOffsets;
  bool mAutoScroll;
  bool mScrollToBottom;
};
} // namespace Anvil

#define LOG_ERROR(fmt, ...)                                                    \
  Anvil::Log::getInstance().addLog(Anvil::Log::Category::Error, __FILE__, fmt, \
                                   ##__VA_ARGS__);

#define LOG_INFO(fmt, ...)                                                     \
  Anvil::Log::getInstance().addLog(Anvil::Log::Category::Info, __FILE__, fmt,  \
                                   ##__VA_ARGS__);

#define LOG_DEBUG(fmt, ...)                                                    \
  Anvil::Log::getInstance().addLog(Anvil::Log::Category::Debug, __FILE__, fmt, \
                                   ##__VA_ARGS__);

namespace Anvil {
inline void _check_gl_error() {
  GLenum err(glGetError());

  while (err != GL_NO_ERROR) {
    std::string error;

    switch (err) {
    case GL_INVALID_OPERATION:
      error = "GL_INVALID_OPERATION";
      break;
    case GL_INVALID_ENUM:
      error = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "GL_INVALID_VALUE";
      break;
    case GL_OUT_OF_MEMORY:
      error = "GL_OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "GL_INVALID_FRAMEBUFFER_OPERATION";
      break;
    }
    LOG_ERROR("Test %d\n", 1);
    err = glGetError();
  }
}
} // namespace Anvil
