#pragma once

#include <glfw/glfw3.h>
#include "Types.h"

#include <vector>
namespace Engine {

/// Basic Camera implementation.
class Camera {
public:
  Camera(vec3 position, vec3 up, vec3 front)
      : mPos(position), mUp(up), mFront(front), mRight(glm::cross(front, up)) {}

  inline const vec3 &getPos() const { return mPos; }

  inline mat4 getViewMatrix() const { return glm::lookAt(mPos, mFront, mUp); }

protected:
  vec3 mPos;
  vec3 mFront;
  vec3 mUp;
  vec3 mRight;
};

} // namespace Engine
