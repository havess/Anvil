#include <Engine/Plane.h>

namespace Engine {
static int numPlanes = 0;
Plane::Plane(int32_t width, int32_t height, const vec3 &normal, const vec3 &p0,
             const vec3 &p1)
    : Mesh("Plane" + std::to_string(numPlanes++), GL_TRIANGLES), mWidth(width),
      mHeight(height), mNormal(normal), mP0(p0), mP1(p0) {
  computeVertices();
}

void Plane::computeVertices() {
  vec3 normalizedForward = glm::normalize(mP1 - mP0);
  vec3 lowerLeft(-mWidth / 2, 0, -mHeight / 2);
  vec3 lowerRight(mWidth / 2, 0, -mHeight / 2);
  vec3 upperRight(mWidth / 2, 0, mHeight / 2);
  vec3 upperLeft(-mWidth / 2, 0, mHeight / 2);

  std::vector<Point> points{
      Point{lowerLeft, vec2(0, 0)}, Point{lowerRight, vec2(0, 1)},
      Point{upperRight, vec2(1, 1)}, Point{upperLeft, vec2(1, 0)}};
  setPoints(points);
  std::vector<uint32_t> indices{0, 1, 3, 3, 1, 2};
  setIndices(indices);
  finalize();
}

} // namespace Engine
