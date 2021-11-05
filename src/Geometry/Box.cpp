#include <Anvil/Box.h>

namespace Engine {

static int numBoxes = 0;
Box::Box(const vec3 &position, float length, float width, float height)
    : Mesh("Box" + std::to_string(numBoxes++), GL_TRIANGLES),
      mPosition(position), mLength(length), mWidth(width), mHeight(height) {
  generatePoints();
}

void Box::generatePoints() {
  // Generate points, since we want hard edges we don't use indexing. Remember
  // that y-axis is up.
  Point p0{mPosition + vec3(mLength, 0, 0)};
  Point p1{mPosition + vec3(mLength, mHeight, 0)};
  Point p2{mPosition + vec3(mLength, 0, mWidth)};
  Point p3{mPosition + vec3(mLength, mHeight, mWidth)};
  Point p4{mPosition + vec3(0, 0, 0)};
  Point p5{mPosition + vec3(0, mHeight, 0)};
  Point p6{mPosition + vec3(0, 0, mWidth)};
  Point p7{mPosition + vec3(0, mHeight, mWidth)};
  std::vector<Point> points{p0, p2, p1, p2, p3, p1, p0, p4, p6, p0, p6, p2,
                       p1, p5, p4, p1, p4, p0, p4, p5, p7, p4, p7, p6,
                       p2, p6, p7, p2, p7, p3, p3, p7, p5, p3, p5, p1};
  setPoints(points);
  finalize();
}

} // namespace Engine
