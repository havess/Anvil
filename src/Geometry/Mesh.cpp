#include <Anvil/Mesh.h>
#include <Anvil/Application.h>

#include <stdio.h>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_set>

namespace Engine {

Face::Face(const Mesh *geo, uint32_t p1, uint32_t p2, uint32_t p3) : mGeo(geo) {
  mPointIDs[0] = p1;
  mPointIDs[1] = p2;
  mPointIDs[2] = p3;
  const vec3 &a = mGeo->getPoint(p1).getPos(), &b = mGeo->getPoint(p2).getPos(),
             &c = mGeo->getPoint(p3).getPos();

  mNormal = glm::normalize(glm::cross(c - a, b - a));
}

Point::Point(const vec3 &pos) : mPos(pos) {}
Point::Point(const vec3 &pos, const vec2 &uv) : mPos(pos), mUV(uv) {}
Point::Point(float x, float y, float z) : Point(vec3(x, y, z)) {}

void Point::setPos(const vec3 &pos) { mPos = pos; }

void Point::setNormal(const vec3 &normal) { mNormal = normal; }

Vertex::Vertex(const Mesh *mesh, size_t point, VertexData vertexData,
               size_t face)
    : mPoint(point), mFace(face), mData(vertexData) {
  mData.mPos = mesh->getPoint(point).getPos();
  mData.mNormal = mesh->getFace(face).getNormal();
  mData.mTextureCoord = mesh->getPoint(point).getUV();
}

Mesh::Mesh(const std::string &name, GLenum mode) : mName(name), mMode(mode) {
  mModelMat = mat4(1.0f);
  glGenVertexArrays(1, &mVAO);
  glBindVertexArray(mVAO);
  glGenBuffers(1, &mVBO);
  glGenBuffers(1, &mEBO);
}

void Mesh::draw(const Application &app) {
  glBindVertexArray(mVAO);

  // if we provided indices, do an indexed draw.
  if (!mIndices.empty()) {
    glDrawElements(mMode, mIndices.size(), GL_UNSIGNED_INT, 0);
  } else {
    glDrawArrays(mMode, 0, mVertexData.size());
  }

  glBindVertexArray(0);
}

void Mesh::finalize(bool updateVertexData) {
  if (updateVertexData) {
    std::unordered_map<uint32_t, std::vector<uint32_t>> pointsToFaces;
    // Generate Faces and vertex data
    size_t size = mIndices.empty() ? mPoints.size() : mIndices.size();
    for (int i = 0; i < size; i += 3) {
      // Create face, it will compute its normal
      int32_t index0 = i, index1 = i + 1, index2 = i + 2;
      if (!mIndices.empty()) {
        index0 = mIndices[i];
        index1 = mIndices[i + 1];
        index2 = mIndices[i + 2];
      }
      mFaces.emplace_back(this, index0, index1, index2);
      pointsToFaces[index0].push_back(i / 3);
      pointsToFaces[index1].push_back(i / 3);
      pointsToFaces[index2].push_back(i / 3);
    }
    mVertexData.clear();
    mVertices.clear();
    for (size_t i = 0; i < mPoints.size(); i++) {
      // helper function to create vertex data and the vertex object
      vec3 normal(0.0f);
      for (auto &face : pointsToFaces[i]) {
        normal += mFaces[face].getNormal();
      }
      normal = glm::normalize(normal);
      mVertexData.push_back(
          VertexData{mPoints[i].getPos(), normal, mPoints[i].getUV()});
      mVertices.emplace_back(
          Vertex{this, i, mVertexData.back(), mFaces.size() - 1});
    }
  }

  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER, mVertexData.size() * sizeof(VertexData),
               &mVertexData[0], GL_STATIC_DRAW);

  if (!mIndices.empty()) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t),
                 &mIndices[0], GL_STATIC_DRAW);
  }

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}
} // namespace Engine
