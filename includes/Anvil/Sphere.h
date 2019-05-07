//
//  Sphere.h
//  Anvil
//
//  Copyright © 2019 Sam Haves. All rights reserved.
//

#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

#include "Geometry.h"
#include "Anvil/Mesh.h"
#include "Anvil/Types.h"

namespace Anvil {

class Sphere : public Mesh {
public:
  Sphere(const vec3 &position, float radius, uint8_t iter);

  inline float getRadius() const { return mRadius; };
  inline void setRadius(float radius) { mRadius = radius; };

private:
  void computeVertices();
  std::tuple<uint32_t, uint32_t, uint32_t> getCCWOrdering(uint32_t, uint32_t,
                                                          uint32_t) const;
  size_t getMidpoint(uint32_t iA, uint32_t iB);

  std::unordered_map<uint64_t, size_t> mMiddlePointCache;

  float mRadius;
  vec3 mPosition;
  uint8_t mIterations;
}; // Sphere

} // namespace Anvil

