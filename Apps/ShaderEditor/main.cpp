#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <map>
#include <cmath>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <Engine/Application.h>
#include <Engine/Box.h>
#include <Engine/Grid.h>
#include <Engine/Mesh.h>
#include <Engine/Renderer.h>
#include <Engine/Sphere.h>
#include <Engine/Texture.h>

class SSPlane : public Engine::Mesh {
  public:
    SSPlane() : Mesh("SSPlane", GL_TRIANGLES){
      using Engine::Point;
      std::vector<Point> points{
        Point{vec3{-1, -1, 0}, vec2(0, 0)},
        Point{vec3{-1, 1, 0}, vec2(0, 1)},
        Point{vec3{1, 1, 0}, vec2(1, 1)},
        Point{vec3{1, -1, 0}, vec2(1, 0)}};
      setPoints(points);
      std::vector<uint32_t> indices{0, 1, 3, 3, 1, 2};
      setIndices(indices);
      finalize();
    }
};

class Example : public Engine::Application {
public:
  Example(int argc, char **argv) : Engine::Application(1800, 1000, argc, argv) {
    // Get the renderer for this application.
    auto &renderer = getRenderer();

    using std::placeholders::_1;
    auto shader_id = renderer.createShader({
      "passthrough.vs", // vertex shader
      "basic_color.fs", // fragment shader
      "", // no geometry shader
      std::bind(std::mem_fn(&Example::updateUniforms), this, _1)
    });

    // -------------- Create Renderables -----------------
    auto default_mat = Engine::Material{};
    auto ss_plane = renderer.createRenderable<SSPlane>(
      default_mat,
      shader_id);
 
    ss_plane->bindCallback([this](Engine::Shader &shader, const Engine::Mesh &m) {});
  }

  void updateUniforms(Engine::Shader &shader) {
    using std::chrono::system_clock;
    using std::chrono::duration_cast;
    static auto begin = system_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(system_clock::now() - begin).count() / 1000.0f;
    shader.setFloat("time", duration);
    auto res = vec2{getFramebufferWidth(), getFramebufferHeight()};
    shader.setVec2("resolution", res);
  }
};

int main(int argc, char **argv) {
  Example app(argc, argv);
  app.run();
  return 0;
}