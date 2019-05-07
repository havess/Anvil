//
//  Sphere.h
//  Anvil
//
//  Copyright © 2018 Sam Haves. All rights reserved.
//

#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

#include "Geometry.h"
#include "Anvil/Mesh.h"
#include "Anvil/Types.h"

namespace Anvil {

class Box : public Mesh {
public:
  Box(const vec3 &position, float length, float width, float height);

  inline float getLength() const { return mLength; };
  inline void setLength(float length) { mLength = length; };
  inline float getWidth() const { return mWidth; };
  inline void setWidth(float width) { mWidth = width; };
  inline float getHeight() const { return mHeight; };
  inline void setHeight(float height) { mHeight = height; };

private:
  float mLength;
  float mWidth;
  float mHeight;
  vec3 mPosition;
}; // Sphere

} // namespace Anvil
