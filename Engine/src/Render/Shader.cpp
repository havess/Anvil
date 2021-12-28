#include <GL/gl3w.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>

#include <Engine/Shader.h>

namespace Engine {

// Shader ID
static int nextID = 0;

/* NOTE: Largely borrowed from learnopengl.com */

Shader::Shader(Shader::Info info)
    : mPerBind(info.bindCB),
      mID(nextID++),
      mVertexPath(std::move(info.vsPath)),
      mFragmentPath(std::move(info.fsPath)),
      mGeometryPath(std::move(info.gsPath)) {
  compile();
}

bool Shader::compile() {
  std::string vertexCode, fragmentCode, geometryCode;
  std::ifstream vShaderFile, fShaderFile, gShaderFile;

  // ensure ifstream objects can throw exceptions:
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    // open files
    vShaderFile.open(std::filesystem::current_path() / std::filesystem::path(mVertexPath));
    fShaderFile.open(mFragmentPath.c_str());
    std::stringstream vShaderStream, fShaderStream;
    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    // if provided geometry shader
    if (!mGeometryPath.empty()) {
      gShaderFile.open(mGeometryPath.c_str());
      std::stringstream gShaderStream;
      gShaderStream << gShaderFile.rdbuf();
      gShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  } catch (std::ifstream::failure e) {
    std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << e.what() << std::endl;
  }

  unsigned int vertex, fragment, geometry;
  bool success = true;

  vertex = glCreateShader(GL_VERTEX_SHADER);
  auto v_code = vertexCode.c_str();
  glShaderSource(vertex, 1, &v_code, NULL);
  glCompileShader(vertex);
  success &= checkCompileErrors(vertex, "VERTEX");

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  auto f_code = fragmentCode.c_str();
  glShaderSource(fragment, 1, &f_code, NULL);
  glCompileShader(fragment);
  success &= checkCompileErrors(fragment, "FRAGMENT");
  
  // if geometry shader is given, compile geometry shader
  if (!mGeometryPath.empty()) {
    const char *gShaderCode = geometryCode.c_str();
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    success &= checkCompileErrors(geometry, "GEOMETRY");
  }

  if (success) {
    mProgramID = glCreateProgram();
    glAttachShader(mProgramID, vertex);
    glAttachShader(mProgramID, fragment);
    if (!mGeometryPath.empty())
      glAttachShader(mProgramID, geometry);
    glLinkProgram(mProgramID);
    success &= checkCompileErrors(mProgramID, "PROGRAM");
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);  
  if (!mGeometryPath.empty())
    glDeleteShader(geometry);

  return success;
}

void Shader::use() {
  glUseProgram(mProgramID);
  // Call function that is user defined at every bind.
  mPerBind(*this);
}

bool Shader::reload() {
  return compile();
}

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(mProgramID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(mProgramID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(mProgramID, name.c_str()), value);
}

void Shader::setMatrix(const std::string &name, mat4 value) const {
  glUniformMatrix4fv(glGetUniformLocation(mProgramID, name.c_str()), 1,
                     GL_FALSE, &value[0][0]);
}

void Shader::setVec2(const std::string &name, const vec2 &value) const {
  glUniform2fv(glGetUniformLocation(mProgramID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const {
  glUniform2f(glGetUniformLocation(mProgramID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const vec3 &value) const {
  glUniform3fv(glGetUniformLocation(mProgramID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(mProgramID, name.c_str()), x, y, z);
}

bool Shader::checkCompileErrors(unsigned int shader, std::string type) {
  int success;
  char infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cout
          << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cout
          << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  }
  return success;
}
} // namespace Engine
