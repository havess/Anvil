#pragma once
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>

#include <GL/gl3w.h>
#include <glfw/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Types.h"

namespace Engine {

class Application;
class Mesh;

/// Face is the representation of a triangle in 3 space.
/// faces might have a normal for geometric computation
/// Each face has a list of point IDs which should be
/// provide in CCW order according to the normal
class Face {
public:
  Face(const Mesh *geo, unsigned int p1, unsigned int p2, unsigned int p3);
  virtual ~Face() = default;

  inline const vec3 &getNormal() const { return mNormal; }
  inline void flipNormal() { mNormal = -mNormal; }
  inline const std::tuple<uint32_t, uint32_t, uint32_t> getPoints() const {
    return std::make_tuple(mPointIDs[0], mPointIDs[1], mPointIDs[2]);
  }

private:
  const Mesh *mGeo;
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

/// Vertex is what will be interfaced with to update attributes in VertexData
class Vertex {
public:
  Vertex(const Mesh *mesh, size_t point, VertexData vertexData, size_t face);
  virtual ~Vertex() = default;

  inline const VertexData &getData() { return mData; }

private:
  size_t mPoint;
  size_t mFace;
  VertexData mData;
};

class Mesh {
  friend Vertex;

public:
  Mesh(const std::string &name, GLenum mode);
  virtual ~Mesh() = default;
  void draw(const Application &app);
  void finalize(bool updateVertexData = true);

  inline void setPoints(const std::vector<Point> &points) { mPoints = points; }
  inline void setIndices(const std::vector<uint32_t> &indices) {
    mIndices = indices;
  }
  inline void flipNormals() {
    for (auto &f : mFaces)
      f.flipNormal();
    finalize();
  }
  inline size_t getNumPoints() const { return mPoints.size(); }
  inline size_t getNumFaces() const { return mFaces.size(); }
  inline const Point &getPoint(uint32_t index) const { return mPoints[index]; }
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

protected:
  std::vector<Vertex> mVertices;
  std::vector<uint32_t> mIndices;
  std::vector<Point> mPoints;
  std::vector<Face> mFaces;
  mat4 mTranslateMat;
  mat4 mScaleMat;
  mat4 mRotateMat;
  mat4 mModelMat;

private:
  std::string mName;
  GLenum mMode;
  GLuint mVAO, mVBO, mEBO;
  GLuint mNormalBO;
  GLuint mIndexBO;

  std::vector<VertexData> mVertexData;
};
} // namespace Engine
