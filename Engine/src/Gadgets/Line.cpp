#include <Line.h>

namespace Engine {
namespace Gadgets{

static int numLines = 0;
Line::Line() : Mesh("Line" + std::to_string(numLines++), GL_TRIANGLES) {}

void Line::startLine() {
    myStrokePoints.clear();
}

void Line::addPoint(const vec3 &p) {
    // Just store the position for now, the normal/uv/prev/next will
    // be computed once the line is finished. 
    auto data = LineVertexData{p};
    myStrokePoints.push_back(data);
    myStrokePoints.push_back(data);
}

void endLine() {
    // Go through line and setup next/prev points.
    for (auto i = 0, end = myStrokePoints.size(); i < end; i++) {
        if (i != 0) 
            myStrokePoints[i].prev = myStrokePoints[i-1].pos;

        if (i != end - 1)
            myStrokePoints[i].next = myStrokePoints[i+1].pos;
    }
    setPoints(myStrokePoints);
}
} // namespace Gadgets
} // namespace Engine