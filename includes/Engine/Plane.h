#pragma once
#include "Mesh.h"
#include "Types.h"

namespace Engine {

class Plane : public StandardMesh {
public:
  /// Constructors build Plane with dimensions \p width x \p height.
  /// Create plane with \p normal and surface vector \p forward.
  Plane(int32_t width, int32_t height, const vec3 &normal, const vec3 &forward);
  /// Create plane with \p normal that passes through points \p p0 and \p p1.
  Plane(int32_t width, int32_t height, const vec3 &normal, const vec3 &p0,
        const vec3 &p1);
  virtual ~Plane() = default;

  inline const vec3 &getNormal() const { return mNormal; }

private:
  void computeVertices();
  int32_t mWidth;
  int32_t mHeight;
  vec3 mNormal;
  vec3 mP0, mP1;
};
} // namespace Engine
