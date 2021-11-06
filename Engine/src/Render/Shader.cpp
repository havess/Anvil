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
    : mPerBind(info.bindCB), mID(nextID++) {

  std::string vertexPath = std::move(info.vsPath);
  std::string fragmentPath = std::move(info.fsPath);
  std::string geometryPath = std::move(info.gsPath);

  std::string vertexCode;
  std::string fragmentCode;
  std::string geometryCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;
  std::ifstream gShaderFile;

  // ensure ifstream objects can throw exceptions:
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    // open files
    vShaderFile.open(std::filesystem::current_path() / std::filesystem::path(vertexPath));
    fShaderFile.open(fragmentPath.c_str());
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
    if (!geometryPath.empty()) {
      gShaderFile.open(geometryPath.c_str());
      std::stringstream gShaderStream;
      gShaderStream << gShaderFile.rdbuf();
      gShaderFile.close();
      geometryCode = gShaderStream.str();
    }
  } catch (std::ifstream::failure e) {
    std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << e.what() << std::endl;
  }

  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");
  // if geometry shader is given, compile geometry shader
  unsigned int geometry;
  if (!geometryPath.empty()) {
    const char *gShaderCode = geometryCode.c_str();
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderCode, NULL);
    glCompileShader(geometry);
    checkCompileErrors(geometry, "GEOMETRY");
  }

  mProgramID = glCreateProgram();
  glAttachShader(mProgramID, vertex);
  glAttachShader(mProgramID, fragment);
  if (!geometryPath.empty())
    glAttachShader(mProgramID, geometry);
  glLinkProgram(mProgramID);
  checkCompileErrors(mProgramID, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);  
  if (!geometryPath.empty())
    glDeleteShader(geometry);
}

void Shader::use() {
  glUseProgram(mProgramID);
  // Call function that is user defined at every bind.
  mPerBind(*this);
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

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
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
}
} // namespace Engine
