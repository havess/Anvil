#pragma once
#include "Types.h"

namespace Engine {

/// Basic Camera implementation.
class Application;
class Camera {
public:
  Camera(vec3 position, vec3 up, vec3 front);

  void setPos(vec3 pos) noexcept { mPos = pos; }
  inline const vec3 &getPos() const noexcept { return mPos; }

  virtual mat4 getViewMatrix() const;
  virtual mat4 getProjMatrix(const Application &app) const;

protected:
  vec3 mPos;
  vec3 mFront;
  vec3 mUp;
  vec3 mRight;
};

class OrthoCamera : public Camera {
public:
  OrthoCamera(vec3 position, vec3 up, vec3 front, float width, float height);

  mat4 getViewMatrix() const override;
  mat4 getProjMatrix(const Application &app) const override;

private:
  float mWidth;
  float mHeight;
};

} // namespace Engine
