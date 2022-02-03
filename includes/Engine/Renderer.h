#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Log.h"
#include "Shader.h"
#include "Types.h"
#include "Texture.h"
#include "UIManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <map>
#include <type_traits>
#include <vector>

namespace {
  template<typename T> struct is_mesh : public std::false_type {};

  template<typename D, typename... Types>
  struct is_mesh<Engine::Mesh<D, Types...>> : public std::true_type {};
}

namespace Engine {

class Application;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float sheen;
};

class RenderInterface {
  public:
    virtual void draw(const Application &app, Shader &shader) = 0;
};

/// Encapsulation of shader, material, and mesh which allow us to show something
/// on the screen :).
template<typename T>
class Renderable : public RenderInterface {
public:
  using Mesh = T;
  Renderable(uptr<T> mesh)
      : mMesh(std::move(mesh)) {}
  Renderable(uptr<T> mesh, const Material &material, int shaderID,
             sptr<Texture> texture = nullptr)
      : mMesh(std::move(mesh)), mMaterial(material),
        mTexture(std::move(texture)), mShaderID(shaderID) {}

  inline T &mesh() { return *mMesh; }
  inline const Material &material() { return mMaterial; }
  inline int shaderID() { return mShaderID; }
  void bindMaterial(const Material &material) {
    mMaterial = material;
  }
  void bindShader(int shaderID) {
    mShaderID = shaderID;
  }
  void bindCallback(std::function<void(Shader &, const T &)> cb) {
    mPerObject = cb;
  }
  void draw(const Application &app, Shader &shader) override {
    if (mTexture) {
      glActiveTexture(GL_TEXTURE0);
      mTexture->bind();
    }

    mPerObject(shader, *mMesh);
    LOG_IF_GL_ERR();
    mMesh->draw(app);
    LOG_IF_GL_ERR();

    if (mTexture) {
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }

private:
  uptr<T> mMesh;
  Material mMaterial;
  sptr<Texture> mTexture;
  int mShaderID;
  std::function<void(Shader &, const T &)> mPerObject;
};

class Renderer {
public:
  Renderer();
  virtual ~Renderer() = default;
  void renderFrame(const Application &app, const mat4 &worldTransform);
  //void addLight(const Application &app, sptr<Mesh> mesh, vec3 &colour);

  template<typename M>
  Renderable<M> *addRenderable(int renderGroup, uptr<RenderInterface> renderable) {
    mRenderGroups[renderGroup].push_back(std::move(renderable));
    return dynamic_cast<Renderable<M>*>(mRenderGroups[renderGroup].back().get());
  }

  template<typename M, typename ... Args>
  Renderable<M> *createRenderable(
    const Material &material,
    int shaderID,
    Args&& ... args) {
    auto mesh = std::make_unique<M>(std::forward<Args>(args) ...);
    uptr<Renderable<M>> renderable = std::make_unique<Renderable<M>>(std::move(mesh), material, shaderID);
    return addRenderable<M>(shaderID, std::move(renderable));
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

  template<typename T>
  void removeRenderable(uptr<Renderable<T>> renderable) {
    auto &group = mRenderGroups[renderable->shaderID()];
    auto iter = std::find(group.begin(), group.end(), renderable);
    if (iter != group.end())
      group.erase(iter);
  }

private:
  void renderGeometry(const Application &app, const mat4 &worldTransform,
                      sptr<Shader> overrideShader = nullptr);
  //void renderLights(const Application &app, const mat4 &worldTransform);
  void renderText(const Application &app);

  uptr<Shader> mLightShader;
  uptr<Shader> mDepthShader;
  std::unordered_map<int, uptr<Shader>> mShaders;
  /// Group of renderables by shaderID.
  std::unordered_map<int, std::vector<uptr<RenderInterface>>> mRenderGroups;
  //std::vector<sptr<Mesh>> mLights;
  std::vector<uint> mLightFBOs;
  std::vector<uint> mLightDepthCubeMaps;
  const uint mShadowWidth = 2048;
  const uint mShadowHeight = 2048;
};
} // namespace Engine
