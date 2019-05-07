#include <iostream>
#include <stdio.h>
#include "Anvil/Log.h"
#include "Anvil/Sphere.h"

namespace Anvil {

Sphere::Sphere(const vec3 &centre, float radius, uint8_t iter)
    : Mesh(), mRadius(radius), mPosition(centre), mIterations(iter) {
  computeVertices();
}

void Sphere::computeVertices() {
  double t = (1.0 + sqrt(5.0)) / 2.0;

  /* These are passed to the mesh so they will be deleted on mesh destruction */
  std::vector<vec3> points{vec3(-1, t, 0),  vec3(1, t, 0),   vec3(-1, -t, 0),
                           vec3(1, -t, 0),  vec3(0, -1, t),  vec3(0, 1, t),
                           vec3(0, -1, -t), vec3(0, 1, -t),  vec3(t, 0, -1),
                           vec3(t, 0, 1),   vec3(-t, 0, -1), vec3(-t, 0, 1)};

  for (auto &v : points) {
    v = mPosition + mRadius * glm::normalize(v);
    addPoint(v);
  }
  LOG_DEBUG("Creating initial faces");
  addFace(getCCWOrdering(0, 11, 5));
  addFace(getCCWOrdering(0, 5, 1));
  addFace(getCCWOrdering(0, 1, 7));
  addFace(getCCWOrdering(0, 7, 10));
  addFace(getCCWOrdering(0, 10, 11));
  // 5 adjacent faces
  addFace(getCCWOrdering(1, 5, 9));
  addFace(getCCWOrdering(5, 11, 4));
  addFace(getCCWOrdering(11, 10, 2));
  addFace(getCCWOrdering(10, 7, 6));
  addFace(getCCWOrdering(7, 1, 8));
  // 5 faces around point 3
  addFace(getCCWOrdering(3, 9, 4));
  addFace(getCCWOrdering(3, 4, 2));
  addFace(getCCWOrdering(3, 2, 6));
  addFace(getCCWOrdering(3, 6, 8));
  addFace(getCCWOrdering(3, 8, 9));
  // 5 adjacent faces
  addFace(getCCWOrdering(4, 9, 5));
  addFace(getCCWOrdering(2, 4, 11));
  addFace(getCCWOrdering(6, 2, 10));
  addFace(getCCWOrdering(8, 6, 7));
  addFace(getCCWOrdering(9, 8, 1));

  for (int i = 0; i < mIterations; i++) {
    size_t n = mFaces.size();
    for (int j = 0; j < n; j++) {
      int k = j * 3;
      uint32_t v1 = getMidpoint(mIndices[k], mIndices[k + 1]),
               v2 = getMidpoint(mIndices[k], mIndices[k + 2]),
               v3 = getMidpoint(mIndices[k + 1], mIndices[k + 2]);
      addFace(getCCWOrdering(v1, v2, v3));
      addFace(getCCWOrdering(v1, v2, mIndices[k]));
      addFace(getCCWOrdering(v1, v3, mIndices[k + 1]));
      addFace(getCCWOrdering(v2, v3, mIndices[k + 2]));
    }
    removeFaces(0, n);
  }
}

std::tuple<uint32_t, uint32_t, uint32_t>
Sphere::getCCWOrdering(uint32_t a, uint32_t b, uint32_t c) const {
  const auto &posA = mPoints[a].getPos();
  const auto &posB = mPoints[b].getPos();
  const auto &posC = mPoints[c].getPos();

  mat4 checkMatrix{1.0f};
  checkMatrix[0] = glm::vec4{posA, 1.0f};
  checkMatrix[1] = glm::vec4{posB, 1.0f};
  checkMatrix[2] = glm::vec4{posC, 1.0f};
  checkMatrix[3] = glm::vec4{mPosition, 1.0f};

  auto determinant = glm::determinant(checkMatrix);

  if (determinant > 0) {
    // The winding is backwards swap any 2 to fix.
    return std::make_tuple(a, c, b);
  }
  // Otherwise ordering was fine.
  return std::make_tuple(a, b, c);
}

size_t Sphere::getMidpoint(uint32_t index1, uint32_t index2) {
  // get the smaller index since smaller is computed firsts
  bool smaller = index1 < index2;
  uint64_t smallIndex = smaller ? index1 : index2;
  uint64_t bigIndex = smaller ? index2 : index1;

  // midpoints are stored with long int with both end point indices
  uint64_t key = (smallIndex << 32) + bigIndex;

  if (mMiddlePointCache.find(key) != mMiddlePointCache.end()) {
    return mMiddlePointCache.at(key);
  }

  vec3 posA = getPoint(index1).getPos() - mPosition,
       posB = getPoint(index2).getPos() - mPosition;

  vec3 mid =
      vec3((posA.x + posB.x) / 2, (posA.y + posB.y) / 2, (posA.z + posB.z) / 2);

  mid = mPosition + mRadius * glm::normalize(mid);
  addPoint(mid);

  mMiddlePointCache.insert(
      std::pair<uint64_t, size_t>(key, getNumPoints() - 1));

  return getNumPoints() - 1;
}

} // namespace Anvil
