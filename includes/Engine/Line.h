#pragma once
#include <vector>

#include "Application.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Types.h"

namespace Engine {
namespace Gadgets{

struct LineVertexData {
  vec3 pos;
  float sign;
  vec3 prev;
};

class LineMesh : public Mesh<LineVertexData, vec3, float, vec3> {
public:
  LineMesh();
  void startLine();
  void addPoint(const vec3 &p);
  void endLine();
private:
  std::vector<LineVertexData> mData;
};

class Line : public Renderable<LineMesh> {
public:
  Line(Renderer &r, std::function<void(Shader &)> bindCB);
  void startLine();
  void addPoint(const vec3 &p);
  void endLine();
private:
  inline LineMesh &line() { return dynamic_cast<LineMesh&>(mesh()); }
};

} // namespace Gadgets
} // namespace Engine


