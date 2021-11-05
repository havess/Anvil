#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "Types.h"
#include "Texture.h"
#include "UIManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <map>
#include <vector>

namespace Engine {

class Application;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float sheen;
};

/// Encapsulation of shader, material, and mesh which allow us to show something
/// on the screen :).
class Renderable {
public:
  Renderable(sptr<Mesh> mesh, const Material &material, int shaderID,
             sptr<Texture> texture = nullptr)
      : mMesh(std::move(mesh)), mMaterial(material),
        mTexture(std::move(texture)), mShaderID(shaderID) {}

  inline sptr<Mesh> mesh() { return mMesh; }
  inline const Material &material() { return mMaterial; }
  inline int shaderID() { return mShaderID; }
  void bindCallback(std::function<void(Shader &)> cb) {
    mPerObject = cb;
  }
  void draw(const Application &app, Shader &shader) {
    if (mTexture) {
      glActiveTexture(GL_TEXTURE0);
      mTexture->bind();
    }
    mPerObject(shader);
    mMesh->draw(app);
    if (mTexture) {
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }

private:
  sptr<Mesh> mMesh;
  Material mMaterial;
  sptr<Texture> mTexture;
  int mShaderID;
  std::function<void(Shader &)> mPerObject;
};

class Renderer {
public:
  using RenderableHandle = std::weak_ptr<Renderable>;

  Renderer();
  virtual ~Renderer() = default;
  void renderFrame(const Application &app, const mat4 &worldTransform);
  void addLight(const Application &app, sptr<Mesh> mesh, vec3 &colour);

  void addRenderable(sptr<Renderable> renderable) {
    mRenderGroups[renderable->shaderID()].push_back(renderable);
  }

  template<typename M, typename ... Args>
  RenderableHandle createRenderable(
    const Material &material,
    int shaderID,
    Args&& ... args) {
    auto mesh = std::make_shared<M>(std::forward<Args>(args) ...);
    auto renderable = std::make_shared<Renderable>(mesh, material, shaderID);
    addRenderable(renderable);
    return renderable;
  }

  int createShader(const Shader::Info & shader_info) {
    auto shader = std::make_unique<Shader>(shader_info);
    auto id = shader->id();
    mShaders[id] = std::move(shader);
    return id;
  }

  inline Shader & getShader(int id) {
    if (mShaders.count(id) == 0)
      throw std::invalid_argument("ID does not map to an existing shader.");

    return *mShaders[id];
  }

  void clearRenderGroup(int shaderID) {
    mRenderGroups[shaderID].clear();
  }

  void removeRenderable(sptr<Renderable> renderable) {
    auto &group = mRenderGroups[renderable->shaderID()];
    auto iter = std::find(group.begin(), group.end(), renderable);
    if (iter != group.end())
      group.erase(iter);
  }

private:
  void renderGeometry(const Application &app, const mat4 &worldTransform,
                      sptr<Shader> overrideShader = nullptr);
  void renderLights(const Application &app, const mat4 &worldTransform);
  void renderText(const Application &app);

  uptr<Shader> mLightShader;
  uptr<Shader> mDepthShader;
  std::unordered_map<int, uptr<Shader>> mShaders;
  /// Group of renderables by shaderID.
  std::unordered_map<int, std::vector<sptr<Renderable>>> mRenderGroups;
  std::vector<sptr<Mesh>> mLights;
  std::vector<uint> mLightFBOs;
  std::vector<uint> mLightDepthCubeMaps;
  const uint mShadowWidth = 2048;
  const uint mShadowHeight = 2048;
};
} // namespace Engine
