#pragma once

#include <string>

#include "Types.h"

namespace Anvil {

class Shader {
public:
  // constructor reads and builds the shader
  Shader(const char *vertexPath, const char *fragmentPath,
         const char *geometryPath = nullptr);
  // use/activate the shader
  void use();
  // utility uniform functions
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMatrix(const std::string &name, mat4 value) const;
  void setVec3(const std::string &name, const vec3 &value) const;
  void setVec3(const std::string &name, float x, float y, float z) const;

private:
  void checkCompileErrors(unsigned int shader, std::string type);
  uint mProgramID;
};
} // namespace Anvil
