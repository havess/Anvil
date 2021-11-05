#include <Anvil/Sphere.h>
#include <stdio.h>
#include <iostream>

namespace Engine {
static int numSpheres = 0;
Sphere::Sphere(const vec3 &centre, float radius, uint8_t iter)
    : Mesh("Sphere" + std::to_string(numSpheres++), GL_TRIANGLES),
      mRadius(radius), mPosition(centre), mIterations(iter) {
  computeVertices();
}

void Sphere::computeVertices() {
  float t = (1.0f + sqrt(5.0f)) / 2.0f;

  // Base points of icosahedron.
  std::vector<uint32_t> indices;
  std::vector<Point> points{
      Point(-1, t, 0), Point(1, t, 0), Point(-1, -t, 0), Point(1, -t, 0),
      Point(0, -1, t), Point(0, 1, t), Point(0, -1, -t), Point(0, 1, -t),
      Point(t, 0, -1), Point(t, 0, 1), Point(-t, 0, -1), Point(-t, 0, 1)};

  // Normalize all the points onto the sphere surface.
  for (auto &v : points) {
    v.setPos(mPosition + mRadius * glm::normalize(v.getPos()));
  }

  auto addFace = [&indices](std::tuple<uint32_t, uint32_t, uint32_t> tuple) {
    auto a = std::get<0>(tuple);
    auto b = std::get<1>(tuple);
    auto c = std::get<2>(tuple);
    indices.push_back(a);
    indices.push_back(b);
    indices.push_back(c);
  };

  // Generate icosahedron faces.
  addFace(std::make_tuple(0, 11, 5));
  addFace(std::make_tuple(0, 5, 1));
  addFace(std::make_tuple(0, 1, 7));
  addFace(std::make_tuple(0, 7, 10));
  addFace(std::make_tuple(0, 10, 11));
  // 5 adjacent faces
  addFace(std::make_tuple(1, 5, 9));
  addFace(std::make_tuple(5, 11, 4));
  addFace(std::make_tuple(11, 10, 2));
  addFace(std::make_tuple(10, 7, 6));
  addFace(std::make_tuple(7, 1, 8));
  // 5 faces around point 3
  addFace(std::make_tuple(3, 9, 4));
  addFace(std::make_tuple(3, 4, 2));
  addFace(std::make_tuple(3, 2, 6));
  addFace(std::make_tuple(3, 6, 8));
  addFace(std::make_tuple(3, 8, 9));
  // 5 adjacent faces
  addFace(std::make_tuple(4, 9, 5));
  addFace(std::make_tuple(2, 4, 11));
  addFace(std::make_tuple(6, 2, 10));
  addFace(std::make_tuple(8, 6, 7));
  addFace(std::make_tuple(9, 8, 1));

  // For each triangle, split it into 4 equal area subtriangles and normalize
  // the new points.
  for (int i = 0; i < mIterations; i++) {
    size_t n = indices.size() / 3;
    for (int j = 0; j < n; j++) {
      int k = j * 3;

      // Since we share midpoints fairly often, cache them using the indices of
      // the points on the ends of the line being divided.
      uint32_t v1 = getMidpoint(points, indices[k], indices[k + 1]),
               v2 = getMidpoint(points, indices[k], indices[k + 2]),
               v3 = getMidpoint(points, indices[k + 1], indices[k + 2]);

      if (i == mIterations - 1) {
        // On the last iteration, make sure all our faces have proper winding so
        // that the normals are outwards facing.
        addFace(getCCWOrdering(points, v1, v2, v3));
        addFace(getCCWOrdering(points, v1, v2, indices[k]));
        addFace(getCCWOrdering(points, v1, v3, indices[k + 1]));
        addFace(getCCWOrdering(points, v2, v3, indices[k + 2]));
      } else {
        using std::make_tuple;
        addFace(make_tuple(v1, v2, v3));
        addFace(make_tuple(v1, v2, indices[k]));
        addFace(make_tuple(v1, v3, indices[k + 1]));
        addFace(make_tuple(v2, v3, indices[k + 2]));
      }
    }
    // Delete faces that got subdivided.
    indices.erase(indices.begin(), indices.begin() + n * 3);
  }
  setPoints(points);
  setIndices(indices);
  finalize();
}

std::tuple<uint32_t, uint32_t, uint32_t>
Sphere::getCCWOrdering(const std::vector<Point> &points, uint32_t a, uint32_t b,
                       uint32_t c) const {
  const auto &posA = points[a].getPos();
  const auto &posB = points[b].getPos();
  const auto &posC = points[c].getPos();

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

size_t Sphere::getMidpoint(std::vector<Point> &points, uint32_t index1,
                           uint32_t index2) {
  // get the smaller index since smaller is computed firsts
  bool smaller = index1 < index2;
  uint64_t smallIndex = smaller ? index1 : index2;
  uint64_t bigIndex = smaller ? index2 : index1;

  // midpoints are stored with long int with both end point indices
  uint64_t key = (smallIndex << 32) + bigIndex;

  if (mMiddlePointCache.find(key) != mMiddlePointCache.end()) {
    return mMiddlePointCache.at(key);
  }

  vec3 posA = points[index1].getPos() - mPosition,
       posB = points[index2].getPos() - mPosition;

  vec3 mid =
      vec3((posA.x + posB.x) / 2, (posA.y + posB.y) / 2, (posA.z + posB.z) / 2);

  mid = mPosition + mRadius * glm::normalize(mid);
  points.emplace_back(mid);

  mMiddlePointCache.insert(std::pair<uint64_t, size_t>(key, points.size() - 1));

  return points.size() - 1;
}

} // namespace Engine
