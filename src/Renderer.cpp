#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Anvil/Renderer.h"

namespace Anvil {

Renderer::Renderer() {
  glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
  glEnable(GL_DEPTH_TEST);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  mShaderMap[ShaderType::Light] = std::make_shared<Shader>(
      "src/shaders/basicLighting_VS.glsl", "src/shaders/basicLighting_FS.glsl");

  mShaderMap[ShaderType::Flat] = std::make_shared<Shader>(
      "src/shaders/basicVS.glsl", "src/shaders/basicFS.glsl");
  auto flatShader = mShaderMap[ShaderType::Flat];

  flatShader->use();
  flatShader->setVec3("objectColour", 1.0f, 0.5f, 0.31f);
  flatShader->setVec3("lightColour", 1.0f, 1.0f, 1.0f);
  flatShader->setVec3("lightPos", 0.0f, 0.0f, 0.0f);
  flatShader->setVec3("viewPos", 0.0f, 2.0f, 4.0f);

  mShaderMap[ShaderType::Normals] =
      std::make_shared<Shader>("src/shaders/visualizeNormals_VS.glsl",
                               "src/shaders/visualizeNormals_FS.glsl",
                               "src/shaders/visualizeNormals_GS.glsl");
  auto normalsShader = mShaderMap[ShaderType::Normals];
}

void Renderer::renderFrame(const Anvil::Window &window) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (auto &mesh : mMeshes) {
    mesh.bindTexture();

    mesh.draw(window, *mShaderMap[ShaderType::Flat]);
    mesh.draw(window, *mShaderMap[ShaderType::Normals]);
  }

  for (auto &light : mLights) {
    light.draw(window, *mShaderMap[ShaderType::Light]);
  }
}

void Renderer::addMesh(Mesh &mesh) { mMeshes.push_back(mesh); }

void Renderer::addLight(Mesh &mesh) { mLights.push_back(mesh); }
} // namespace Anvil
