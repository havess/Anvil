#include <Engine/Grid.h>

namespace Engine {

Grid::Grid(uint8_t dim, uint8_t spacing, vec3 startingCorner)
    : Mesh("Grid", GL_LINES) {
  std::vector<Point> points;
  auto sX = startingCorner.x, sY = startingCorner.y, sZ = startingCorner.z;
  // Generate points, remember that y-axis is up.
  for (int i = 0; i <= dim; i++) {
    points.emplace_back(sX + i * spacing, sY, sZ);
    points.emplace_back(sX + i * spacing, sY, sZ + dim * spacing);
    points.emplace_back(sX, sY, sZ + i * spacing);
    points.emplace_back(sX + dim * spacing, sY, sZ + i * spacing);
  }
  setPoints(points);
  finalize();
}

} // namespace Engine
