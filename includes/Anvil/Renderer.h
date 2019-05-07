#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Mesh.h"
#include "Shader.h"
#include "Types.h"

namespace Anvil {

class Renderer {
public:
  Renderer();
  void renderFrame(const Window &window);
  void addMesh(Mesh &mesh);
  void addLight(Mesh &mesh);

private:
  enum class ShaderType { Light, Flat, Normals };

  GLuint mMatrixID;

  std::map<ShaderType, sptr<Shader>> mShaderMap;

  std::vector<Mesh> mMeshes;
  std::vector<Mesh> mLights;
};
} // namespace Anvil
