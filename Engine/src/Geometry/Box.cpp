#include <Engine/Box.h>

namespace Engine {
namespace {
  vec3 computeNormal(const Point &a, const Point & b, const Point & c) {
    return glm::normalize(glm::cross(b.getPos()-a.getPos(), c.getPos()-a.getPos()));
  }
}

static int numBoxes = 0;
Box::Box(const vec3 &position, float length, float width, float height)
    : StandardMesh("Box" + std::to_string(numBoxes++), GL_TRIANGLES),
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
  std::vector<StandardMeshData> vertexData;
  for (int i = 0, end = points.size(); i < end; i += 6) {
    auto normal = computeNormal(points[i], points[i+1], points[i+2]);
    auto data1 = StandardMeshData{points[i].getPos(), normal, vec2(0,0)};
    auto data2 = StandardMeshData{points[i+1].getPos(), normal, vec2(1,0)};
    auto data3 = StandardMeshData{points[i+2].getPos(), normal, vec2(0,1)};
    auto data4 = StandardMeshData{points[i+3].getPos(), normal, vec2(1,0)};
    auto data5 = StandardMeshData{points[i+4].getPos(), normal, vec2(1,1)};
    auto data6 = StandardMeshData{points[i+5].getPos(), normal, vec2(0,1)};
    vertexData.insert(vertexData.end(), {data1, data2, data3, data4, data5, data6});
  }
  setVertexData(vertexData);
  finalize();
}

} // namespace Engine
