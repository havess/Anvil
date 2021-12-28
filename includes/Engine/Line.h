#pragma once
#include <vector>

#include "Mesh.h"
#include "Types.h"

namespace Engine {
namespace Gadgets{

struct LineVertexData {
  vec3 pos;
  vec3 normal;
  vec2 uv;
  vec3 prev;
  vec3 next;
}

class Line : public Mesh<LineVertexData, vec3, vec3, vec2, vec3, vec3> {
public:
  Line();

  void startLine();
  void addPoint(const vec3 &p);
  void endLine();

private:
  vector<LineVertexData> myStrokePoints;
  Shader myShader;
};

} // namespace Gadgets
} // namespace Engine


