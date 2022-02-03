#include <Engine/Line.h>
#include <Engine/ShaderPresets.h>
#include <Engine/Log.h>

namespace Engine {
namespace Gadgets{

static int numLines = 0;
LineMesh::LineMesh() : Mesh("Line" + std::to_string(numLines++), GL_TRIANGLES) {}

void LineMesh::startLine() {
    mData.clear();
}

void LineMesh::addPoint(const vec3 &p) {
    // Just store the position for now, the normal/uv/prev/next will
    // be computed once the line is finished.
    auto data = LineVertexData{p};
    mData.push_back(data);
    mData.push_back(data);
}

void LineMesh::endLine() {
    // Go through line and setup next/prev points.
    for (auto i = 0; i < mData.size(); i += 2) {
        if (i != 0) {
            mData[i].prev = mData[i-1].pos;
            mData[i+1].prev = mData[i-1].pos;
        } else {
            auto prev = mData[0].pos - (mData[i+2].pos - mData[0].pos);
            mData[0].prev = prev;
            mData[1].prev = prev;
        }

        /*if (i != mData.size() - 2) {
            mData[i].next = mData[i+2].pos;
            mData[i].next = mData[i+2].pos;
        }*/

        mData[i].sign = 1.0f;
        mData[i+1].sign = -1.0f;
    }
    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < mData.size() - 2; i++) {
        indices.insert(indices.end(), {i, i+1, i+2});
    }
    setIndices(indices);
    setVertexData(mData);
    finalize();
}

Line::Line(Renderer &renderer, std::function<void(Shader &)> bindCB) : Renderable<LineMesh>(std::make_unique<LineMesh>()) {
    int shaderID = generateShaderPreset(renderer, ShaderPreset::Line, bindCB);
    bindShader(shaderID);
    LOG_DEBUG("Line Created...");
}

void Line::startLine() {
    line().startLine();
}

void Line::addPoint(const vec3 &p) {
    line().addPoint(p);
}

void Line::endLine() {
    line().endLine();
}
} // namespace Gadgets
} // namespace Engine