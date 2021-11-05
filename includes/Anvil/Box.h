#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

#include "Mesh.h"
#include "Types.h"

namespace Engine {

/// Basic axis-aligned rectangular prism implemenation.
class Box : public Mesh {
public:
  /// The position of the box relates to the front right corner.
  Box(const vec3 &position, float length, float width, float height);

  inline float getLength() const { return mLength; };
  inline void setLength(float length) {
    mLength = length;
    generatePoints();
  };
  inline float getWidth() const { return mWidth; };
  inline void setWidth(float width) {
    mWidth = width;
    generatePoints();
  };
  inline float getHeight() const { return mHeight; };
  inline void setHeight(float height) {
    mHeight = height;
    generatePoints();
  };

private:
  float mLength;
  float mWidth;
  float mHeight;
  vec3 mPosition;

  void generatePoints();
};

} // namespace Engine
