#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <Engine/Application.h>
#include <Engine/Renderer.h>
#include <Engine/Log.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
namespace Engine {

Renderer::Renderer() {
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
  // glFrontFace(GL_CW);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);
  glEnable(GL_DEPTH_CLAMP);

  glClearDepth(1.0f);
  glClearColor(0.8f, 0.25f, 0.5f, 1.0f);

  /*********** CONFIGURE DEPTH BUFFER ************/
  auto depth_shader_info = Shader::Info{
    "depth.vs", "depth.fs", "depth.gs", [](Shader &shader) {}
  };
  mDepthShader = std::make_unique<Shader>(depth_shader_info);
}

void Renderer::renderFrame(const Application &app, const mat4 &worldMat) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  // 0. create depth cubemap transformation matrices
  // -----------------------------------------------
  /*float near_plane = 1.0f;
  float far_plane = 500.0f;
  auto &l = mLights[0];
  auto lightPos = vec3(worldMat * l->getModelMat() * vec4(0, 0, 0, 1));
  glm::mat4 shadowProj = glm::perspective(
      glm::radians(90.0f), (float)mShadowWidth / (float)mShadowHeight,
      near_plane, far_plane);
  std::vector<glm::mat4> shadowTransforms;
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos,
                               lightPos + glm::vec3(-1.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos,
                               lightPos + glm::vec3(0.0f, -1.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, -1.0f)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos,
                               lightPos + glm::vec3(0.0f, 0.0f, -1.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f)));*/

  // 1. render scene to depth cubemap
  // --------------------------------
  //glViewport(0, 0, mShadowWidth, mShadowHeight);
  //glBindFramebuffer(GL_FRAMEBUFFER, mLightFBOs[0]);
  //glClear(GL_DEPTH_BUFFER_BIT);
  //mDepthShader->use();
  //for (unsigned int i = 0; i < 6; ++i)
  //  mDepthShader->setMatrix("shadowMatrices[" + std::to_string(i) + "]",
  //                          shadowTransforms[i]);
  //mDepthShader->setFloat("far_plane", far_plane);
  //mDepthShader->setVec3("lightPos", lightPos);
  //renderGeometry(app, worldMat, mDepthShader);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  _check_gl_error();
  glViewport(0, 0, app.getFramebufferWidth(), app.getFramebufferHeight());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glActiveTexture(GL_TEXTURE1);
  //glBindTexture(GL_TEXTURE_CUBE_MAP, mLightDepthCubeMaps[0]);
  renderGeometry(app, worldMat);
  //renderLights(app, worldMat);
  _check_gl_error();
} // namespace Engine

/*void Renderer::addLight(const Application &app, sptr<Mesh> mesh, vec3 &colour) {
  auto light_shader_info = Shader::Info{
    "basicLighting_VS.glsl",
    "basicLighting_FS.glsl",
    "",
    [colour](Shader &shader) { shader.setVec3("lightColour", colour); }
  };
  mLightShader = std::make_unique<Shader>(light_shader_info);
  mLights.push_back(mesh);

  /******** Generate depth map FBO for this light ********/

 /* unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  // create depth cubemap texture
  unsigned int depthCubemap;
  glGenTextures(1, &depthCubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
  for (unsigned int i = 0; i < 6; ++i)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 mShadowWidth, mShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  // attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  mLightFBOs.push_back(depthMapFBO);
  mLightDepthCubeMaps.push_back(depthCubemap);
}*/

void Renderer::renderGeometry(const Application &app,
                              const mat4 &worldTransform,
                              const sptr<Shader> overrideShader) {
  // For each list of renderables that share a shader program, draw them all at
  // once to minimize shader program switching.
  for (auto &renderGroup : mRenderGroups) {
    auto shaderID = renderGroup.first;
    auto &renderList = renderGroup.second;
    auto &shader = overrideShader ? *overrideShader : *mShaders[shaderID];
    shader.use();
    for (auto &renderable : renderList) {
      renderable->draw(app, shader);
    }
  }
}

/*void Renderer::renderLights(const Application &app,
                            const mat4 &worldTransform) {
  // Draw the lights mesh representations. For now we leave these as "hidden"
  // renderables. TODO
  for (auto lightPtr : mLights) {
    auto &light = *lightPtr;
    mLightShader->use();
    mLightShader->setMatrix("model", worldTransform * light.getModelMat());
    mLightShader->setMatrix("view", app.getViewMatrix());
    mLightShader->setMatrix("proj", app.getProjMatrix());
    mLightShader->setVec3("viewPos", app.getCamera().getPos());
    light.draw(app);
  }
}*/
} // namespace Engine
