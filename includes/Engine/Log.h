#pragma once

#include <GL/gl3w.h>
#include <iostream>
#include <string>
#include <vector>
#include <imgui/imgui.h>
namespace Engine {
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
} // namespace Engine

#define LOG_ERROR(fmt, ...)                                                    \
  Engine::Log::getInstance().addLog(Engine::Log::Category::Error, __FILE__,    \
                                    fmt, ##__VA_ARGS__);

#define LOG_INFO(fmt, ...)                                                     \
  Engine::Log::getInstance().addLog(Engine::Log::Category::Info, __FILE__,     \
                                    fmt, ##__VA_ARGS__);

#define LOG_DEBUG(fmt, ...)                                                    \
  Engine::Log::getInstance().addLog(Engine::Log::Category::Debug, __FILE__,    \
                                    fmt, ##__VA_ARGS__);

namespace Engine {
inline bool _check_gl_error() {
  GLenum err(glGetError());

  bool err_occured = err != GL_NO_ERROR;
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
      default:
        LOG_ERROR("Unkown GL Error %d\n", err);
        continue;
    }
    LOG_ERROR("GL Error: %s\n", error.c_str());
    
    err = glGetError();
  }
  return err_occured;
}
} // namespace Engine

#define LOG_IF_GL_ERR(fmt, ...)  \
  if(Engine::_check_gl_error())  \
    LOG_ERROR("GL Error occured at line : %d", __LINE__);
