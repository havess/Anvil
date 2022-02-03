#pragma once

#include <Engine/Shader.h>

namespace Engine {
enum class ShaderPreset {
    Line,
};

int generateShaderPreset(Renderer &renderer, ShaderPreset preset, std::function<void(Shader &)> &bindCB) {
    int ret = -1;
    if (preset == ShaderPreset::Line) {
        auto shader_info = Shader::Info{
            "screenspace_lines.vs",
            "fill.fs",
            "",
            bindCB
        };
        ret = renderer.createShader(shader_info);
    }
    return ret;
}
}