#pragma once

#include "Mesh.h"
#include "Types.h"

namespace Engine {

/// Implmentation of axis aligned grid.
class Grid : public Mesh {
public:
  Grid(uint8_t dim, uint8_t spacing, vec3 startingCorner);
};

} // namespace Engine
