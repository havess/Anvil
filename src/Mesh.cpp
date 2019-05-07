#include "Anvil/Log.h"
#include "Anvil/Mesh.h"
#include "Anvil/Window.h"

#include <glm/glm.hpp>
#include <iostream>
#include <stdio.h>

namespace Anvil {

Mesh::Mesh(const std::string &texPath, bool smooth)
    : mVertexBlockSize(0), mBatchUpdate(false), mIsSmooth(smooth) {
  LOG_INFO("Mesh constructor");
  mModelMat = mat4(1.0f);
  mTexture = nullptr;
  if (!texPath.empty()) {
    mTexture = std::make_shared<Texture>(texPath.c_str());
  }
  glGenVertexArrays(1, &mVAO);
  glBindVertexArray(mVAO);
  glGenBuffers(1, &mVBO);
  glGenBuffers(1, &mEBO);
  refreshData();
}

void Mesh::addAttribute(Attribute &att) {
  mAttributes.push_back(att);
  mVertexBlockSize += att.mSize;
  refreshData();
}

void Mesh::createVertex(uint32_t point, uint32_t face) {
  mVertices.emplace_back(Vertex{getPoint(point),
                                mVertexData[mPointToVertexData[point]],
                                mFaces[face], mIsSmooth});
}

void Mesh::addPoint(const vec3 &pos) {
  mPointToVertexData[mPoints.size()] = mVertexData.size();
  BaseGeometry::addPoint(pos);
  mVertexData.push_back(VertexData{pos, vec3{0, 0, 0}});
}

void Mesh::addFace(uint32_t a, uint32_t b, uint32_t c) {
  // Add index data
  mIndices.push_back(a);
  mIndices.push_back(b);
  mIndices.push_back(c);
  // Create face on geometry
  Geometry::BaseGeometry::addFace(a, b, c);
  // Create a vertex on every corner of the face
  createVertex(a, mFaces.size() - 1);
  createVertex(b, mFaces.size() - 1);
  createVertex(c, mFaces.size() - 1);
}

void Mesh::addFace(std::tuple<uint32_t, uint32_t, uint32_t> tuple) {
  auto [a, b, c] = tuple;
  addFace(a, b, c);
}

/// This is crude and expensive, use sparingly :)
void Mesh::removeFaces(size_t low, size_t high) {
  mFaces.erase(mFaces.begin() + low, mFaces.begin() + high);
  mIndices.erase(mIndices.begin() + 3 * low, mIndices.begin() + 3 * high);
  // mVertexData.erase(mVertexData.begin() + low, mVertexData.begin() + high);
}

void Mesh::refreshData() {
  // if we are currently batch updating, return early
  if (mBatchUpdate) {
    return;
  }
  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  glBufferData(GL_ARRAY_BUFFER, mVertexData.size() * sizeof(VertexData),
               &mVertexData[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(uint32_t),
               &mIndices[0], GL_STATIC_DRAW);
  uint offset = 0;
  for (int i = 0; i < mAttributes.size(); i++) {
    auto a = mAttributes[i];
    glVertexAttribPointer(i, a.mSize, GL_FLOAT, GL_FALSE,
                          mVertexBlockSize * sizeof(float),
                          (void *)(offset * sizeof(float)));
    glEnableVertexAttribArray(i);
    offset += a.mSize;
  }
}

void Mesh::draw(const Window &window, Shader &shader) {
  shader.use();
  shader.setMatrix("model", mModelMat);
  shader.setMatrix("view", window.getViewMatrix());
  shader.setMatrix("proj", glm::perspective(glm::radians(window.getZoom()),
                                            (float)window.getWidth() /
                                                (float)window.getHeight(),
                                            0.1f, 100.0f));
  glBindVertexArray(mVAO);
  if (mIndices.size()) {
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, mVertexData.size());
  }
  glBindVertexArray(0);
}
} // namespace Anvil
