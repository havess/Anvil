#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <map>
#include <cmath>
#include <chrono>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <Engine/Application.h>
#include <Engine/Log.h>
#include <Engine/Mesh.h>
#include <Engine/Renderer.h>

class SSPlane : public Engine::StandardMesh {
  public:
    SSPlane() : Engine::StandardMesh("SSPlane", GL_TRIANGLES){
      using Engine::Point;
      std::vector<Point> points{
        Point{vec3{-1, -1, 0}, vec2(0, 0)},
        Point{vec3{-1, 1, 0}, vec2(0, 1)},
        Point{vec3{1, 1, 0}, vec2(1, 1)},
        Point{vec3{1, -1, 0}, vec2(1, 0)}};
      std::vector<Engine::StandardMeshData> vertexData;
      for (const auto &p : points) {
        auto d = Engine::StandardMeshData{p.getPos(), p.getNormal(), p.getUV()};
        vertexData.push_back(d);
      }
      setVertexData(vertexData);
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
    using std::placeholders::_2;
    using std::mem_fn;
    using std::bind;
    mShader = renderer.createShader({
      "passthrough.vs", // vertex shader
      "circle.fs", // fragment shader
      "", // no geometry shader
      bind(mem_fn(&Example::updateUniforms), this, _1)
    });

    // -------------- Create Renderables -----------------
    auto default_mat = Engine::Material{};
    auto ss_plane = renderer.createRenderable<SSPlane>(
      default_mat,
      mShader);
 
    ss_plane->bindCallback([this](Engine::Shader &shader, const SSPlane &m) {});

    std::function<void(int, int)> key_cb = bind(mem_fn(&Example::keyCB), this, _1, _2);
    mInputHandler->addKeyCallback(key_cb);
    LOG_INFO("Initialized Shader Editor.");

    std::function<void(bool *)> overlay_draw = bind(mem_fn(&Example::drawOverlay), this, _1);
    mUIManager.registerWidget("Overlay", overlay_draw);
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

private:
  void keyCB(int key, int action) {
    if (action == GLFW_PRESS) {
      switch (key) {
      case GLFW_KEY_R:
        auto &r = getRenderer();
        r.getShader(mShader).reload();
        LOG_INFO("Shader reloaded.")
        break;
      }
    }
  }

  void drawOverlay(bool *p_open) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    constexpr float PAD = 10.0f;
    ImVec2 window_pos, window_pos_pivot;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    window_pos.x = work_pos.x + PAD;
    window_pos.y = work_pos.y + PAD;
    window_pos_pivot.x = 0.0f;
    window_pos_pivot.y = 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    window_flags |= ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Help", p_open, window_flags))
        ImGui::Text("R - Reload Shader");
    ImGui::End();
  }

  int mShader = -1;
};

int main(int argc, char **argv) {
  Example app(argc, argv);
  app.run();
  return 0;
}