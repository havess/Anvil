#pragma once

#include <functional>
#include <string>

#include "Types.h"

namespace Engine {

/* NOTE: Largely borrowed from learnopengl.com */

class Shader {
public:
  struct Info {
    std::string vsPath;
    std::string fsPath;
    std::string gsPath;
    std::function<void(Shader &)> bindCB;
  };

  Shader(Shader::Info info);
  void use();

  // Uniform functions.
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMatrix(const std::string &name, mat4 value) const;
  void setVec3(const std::string &name, const vec3 &value) const;
  void setVec3(const std::string &name, float x, float y, float z) const;
  inline int id() const { return mID; }
private:
  void checkCompileErrors(unsigned int shader, std::string type);
  uint mProgramID;
  std::function<void(Shader &)> mPerBind;
  // We use our own ID since we are not guaranteed monotonically increasing
  // program IDs from 0.
  int mID;
};
} // namespace Engine
