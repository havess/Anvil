#pragma once
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Log.h"
#include "Shader.h"
#include "Types.h"

namespace {
  void bindGLAttrib(int index, int numElements, int type, int stride, int offset) {
    //glVertexAttribPointer(index, numElements, type, GL_FALSE, stride,
    //                    (void *)(offset));
    //glEnableVertexAttribArray(index);
    LOG_ERROR("bindGLAttrib is not implemented");
  }

  template<typename T>
  void bindType(int index, int stride, int& offset) {
    throw std::runtime_error("Unsupported vertex attribute.");
  }

  template<>
  void bindType<int>(int index, int stride, int& offset) {
    bindGLAttrib(index, 1, 0 /* GL_INT */, stride, offset);
    offset += sizeof(int);
  }

  template<>
  void bindType<float>(int index, int stride, int& offset) {
    bindGLAttrib(index, 1, 0 /* GL_FLOAT */, stride, offset);
    offset += sizeof(float);
  }

  template<>
  void bindType<vec2>(int index, int stride, int& offset) {
    bindGLAttrib(index, 2, 0 /* GL_FLOAT */, stride, offset);
    offset += 2*sizeof(float);
  }

  template<>
  void bindType<vec3>(int index, int stride, int& offset) {
    bindGLAttrib(index, 3, 0 /* GL_FLOAT */, stride, offset);
    offset += 3*sizeof(float);
  }

  template<typename First>
  void bindAttributes(int stride, int& offset, int index) {
    bindType<std::tuple_element_t<0, std::tuple<First>>>(index, stride, offset);
  }

  template<typename First, typename Second, typename... Types>
  void bindAttributes(int stride, int& offset, int index) {
    bindAttributes<First>(stride, offset, index);
    bindAttributes<Second, Types...>(stride, offset, index + 1);
  }
}

namespace Engine {

class Application;
template <typename, typename...>
class Mesh;

/// Face is the representation of a triangle in 3 space.
/// faces might have a normal for geometric computation
/// Each face has a list of point IDs which should be
/// provide in CCW order according to the normal
class Face {
public:
  template <typename D, typename... Types>
  Face(const Mesh<D, Types...> *geo, unsigned int p1, unsigned int p2, unsigned int p3);
  virtual ~Face() = default;

  inline const vec3 &getNormal() const { return mNormal; }
  inline void flipNormal() { mNormal = -mNormal; }
  inline const std::tuple<uint32_t, uint32_t, uint32_t> getPoints() const {
    return std::make_tuple(mPointIDs[0], mPointIDs[1], mPointIDs[2]);
  }

private:
  vec3 mNormal;
  uint32_t mPointIDs[3];
};

/// A point is a simple representation of a position in 3 space and should
/// be what we use when manipulating geometry
class Point {
public:
  Point(const vec3 &pos);
  Point(const vec3 &pos, const vec2 &uv);
  Point(float x, float y, float z);

  void setPos(const vec3 &pos);
  void setNormal(const vec3 &normal);
  void setUV(const vec2 *uv);

  inline const vec3 &getPos() const { return mPos; }
  inline const vec3 &getNormal() const { return mNormal; }
  inline const vec2 &getUV() const { return mUV; }

  Point operator*(const Point &that) const {
    return Point{this->mPos * that.mPos};
  }

  Point operator/(const Point &that) const {
    return Point{this->mPos / that.mPos};
  }

  Point operator+(const Point &that) const {
    return Point{this->mPos + that.mPos};
  }

  Point operator-(const Point &that) const {
    return Point{this->mPos - that.mPos};
  }

private:
  vec3 mPos;
  vec3 mNormal;
  vec2 mUV;
};

struct VertexData {
  vec3 mPos;
  vec3 mNormal;
  vec2 mTextureCoord;
};

template <typename Data, typename... Types>
class Mesh {
public:
  Mesh(const std::string &name, GLenum mode) : mName(name), mMode(mode) {
      mModelMat = mat4(1.0f);
    //glGenVertexArrays(1, &mVAO);
    //glBindVertexArray(mVAO);
    //glGenBuffers(1, &mVBO);
    //glGenBuffers(1, &mEBO);
  }
  virtual ~Mesh() = default;
  void draw(const Application &app) {
    //glBindVertexArray(mVAO);

    // if we provided indices, do an indexed draw.
    if (!mIndices.empty()) {
      //glDrawElements(mMode, mIndices.size(), GL_UNSIGNED_INT, 0);
    } else {
      //glDrawArrays(mMode, 0, mVertexData.size());
    }

    //glBindVertexArray(0);
  }
  void finalize(bool updateVertexData = true) {
    //glBindVertexArray(mVAO);
    //glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    //glBufferData(GL_ARRAY_BUFFER, mVertexData.size() * sizeof(Data),
    //            &mVertexData[0], GL_STATIC_DRAW);

    if (!mIndices.empty()) {
      //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
      //glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t),
      //            &mIndices[0], GL_STATIC_DRAW);
    }

    int offset = 0;
    ::bindAttributes<Types...>(sizeof(Data), offset, 0);
  }

  inline void setVertexData(const std::vector<Data> &data) { mVertexData = data; }
  inline void setIndices(const std::vector<uint32_t> &indices) {
    mIndices = indices;
  }
  inline void flipNormals() {
    for (auto &f : mFaces)
      f.flipNormal();
    finalize();
  }
  inline size_t getNumFaces() const { return mFaces.size(); }
  inline const Face &getFace(uint32_t index) const { return mFaces[index]; }
  inline const mat4 &getModelMat() const { return mModelMat; }
  inline const mat4 &getScaleMat() const { return mScaleMat; }
  inline void setModelMat(const mat4 &mat) {
    mModelMat = mat * mTranslateMat * mRotateMat * mScaleMat;
  }
  inline mat4 getUnscaledMat() const { return mTranslateMat * mRotateMat; }
  inline const std::string &name() const { return mName; }

  inline mat4 getModelMat() { return mModelMat; }
  inline void resetModelMat() { mModelMat = mat4(1.0f); }
  inline void rotate(float degrees, const vec3 &axis) {
    mRotateMat = glm::rotate(mRotateMat, glm::radians(degrees), axis);
    mModelMat = mTranslateMat * mRotateMat * mScaleMat;
  }
  inline void translate(const vec3 &vec) {
    mTranslateMat = glm::translate(mTranslateMat, vec);
    mModelMat = mTranslateMat * mRotateMat * mScaleMat;
  }
  
  inline void scale(const vec3 &vec) {
    mScaleMat = glm::scale(mScaleMat, vec);
    mModelMat = mTranslateMat * mRotateMat * mScaleMat;
  }
  inline void setScale(const vec3 &vec) {
    mScaleMat = glm::scale(mat4(1.0), vec);
    mModelMat = mTranslateMat * mRotateMat * mScaleMat;
  }

  bool mAreNormalsFlipped = false;
  static constexpr std::tuple<Types...> layout;

protected:
  std::vector<uint32_t> mIndices;
  std::vector<Face> mFaces;
  mat4 mTranslateMat;
  mat4 mScaleMat;
  mat4 mRotateMat;
  mat4 mModelMat;

private:
  std::string mName;
  int mMode;
  int mVAO, mVBO, mEBO;
  int mNormalBO;
  int mIndexBO;

  std::vector<Data> mVertexData;
};

using StandardMeshData = VertexData;
using StandardMesh = Mesh<StandardMeshData, vec3, vec3, vec2>;
} // namespace Engine
