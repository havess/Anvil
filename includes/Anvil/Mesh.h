#pragma once
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Geometry.h"
#include "Shader.h"
#include "Texture.h"
#include "Types.h"

namespace Anvil {

class Window;

/// Err I'm not really sure if this is the best way to do this
/// We want points to be the stuff handled from a geometric perspective
/// and vertices to be what contains attribute information for OpenGL
/// VertexData will be the barebones struct that holds all the data needed
/// for OpenGL
struct VertexData {
  vec3 mPos;
  vec3 mNormal;
};

/// Vertex is what will be interfaced with to update attributes in VertexData
class Vertex : public Geometry::PointListener {
public:
  Vertex(const Geometry::Point &p, VertexData &vertexData,
         const Geometry::Face &f, bool isSmooth)
      : mPoint(p), mFace(f), mData(vertexData), mIsSmooth(isSmooth) {
    update();
  }
  virtual ~Vertex() = default;

  void update() override {
    mData.mPos = mPoint.getPos();
    mData.mNormal = mIsSmooth ? mPoint.getNormal() : mFace.getNormal();
  }
  inline const VertexData &getData() { return mData; }

private:
  const Geometry::Point &mPoint;
  const Geometry::Face &mFace;
  VertexData &mData;
  bool mIsSmooth;
};

struct Attribute {
  uint32_t mSize;
  std::string mDesc;
};

class Mesh : public Geometry::BaseGeometry {
public:
  Mesh(const std::string &texPath = "", bool smooth = false);
  virtual ~Mesh() = default;
  void addAttribute(Attribute &att);
  void draw(const Window &window, Shader &shader);
  void addPoint(const vec3 &pos);
  void addFace(uint32_t a, uint32_t b, uint32_t c);
  void addFace(std::tuple<uint32_t, uint32_t, uint32_t>);
  void removeFaces(size_t low, size_t high);

  // TODO: this is just to see if textures work, obviously bad
  inline void bindTexture() {
    if (nullptr == mTexture) {
      return;
    }
    mTexture->bind();
  }

  inline void setVAO(GLuint vao) { mVAO = vao; }
  inline GLuint getVAO() const { return mVAO; }
  inline void setVBO(GLuint vbo) { mVBO = vbo; }
  inline GLuint getVBO() const { return mVBO; }
  inline void setEBO(GLuint ebo) { mEBO = ebo; }
  inline GLuint getEBO() const { return mEBO; }
  inline void setTexCoordBO(GLuint tcbo) { mTexCoordBO = tcbo; }
  inline void setNormalBO(GLuint nbo) { mNormalBO = nbo; }
  inline void setIndexBO(GLuint ibo) { mIndexBO = ibo; }
  inline mat4 getModelMat() { return mModelMat; }
  inline void rotate(float degrees, const vec3 &axis) {
    mModelMat = glm::rotate(mModelMat, glm::radians(degrees), axis);
  }
  inline void translate(const vec3 &vec) {
    mModelMat = glm::translate(mModelMat, vec);
  }
  inline void scale(const vec3 &vec) { mModelMat = glm::scale(mModelMat, vec); }

  inline void startBatchUpdate() { mBatchUpdate = true; }
  inline void stopBatchUpdate() { mBatchUpdate = false; }

protected:
  std::vector<Vertex> mVertices;
  std::vector<uint32_t> mIndices;

private:
  void createVertex(uint32_t pointIndex, uint32_t faceIndex);
  void refreshData();

  GLuint mVAO, mVBO, mEBO;
  GLuint mTexCoordBO;
  GLuint mNormalBO;
  GLuint mIndexBO;

  uint32_t mVertexBlockSize;

  mat4 mModelMat;

  sptr<Texture> mTexture;

  std::vector<Attribute> mAttributes;
  std::vector<VertexData> mVertexData;
  std::unordered_map<uint32_t, uint32_t> mPointToVertexData;

  bool mBatchUpdate;
  bool mIsSmooth;
};
} // namespace Anvil
