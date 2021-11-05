#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "Mesh.h"
#include "Types.h"

namespace Engine {

class Sphere : public Mesh {
public:
  Sphere(const vec3 &position, float radius, uint8_t iter);

  inline float getRadius() const { return mRadius; };
  inline void setRadius(float radius) {
    mRadius = radius;
    computeVertices();
  };
  inline vec3 getPos() const { return mPosition; }

private:
  void computeVertices();
  std::tuple<uint32_t, uint32_t, uint32_t>
  getCCWOrdering(const std::vector<Point> &, uint32_t, uint32_t,
                 uint32_t) const;
  size_t getMidpoint(std::vector<Point> &points, uint32_t iA, uint32_t iB);

  /// Since we share midpoints fairly often, cache them using the indices of
  /// the points on the ends of the line being divided.
  std::unordered_map<uint64_t, size_t> mMiddlePointCache;

  float mRadius;
  vec3 mPosition;
  uint8_t mIterations;
};

} // namespace Engine

