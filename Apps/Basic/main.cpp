#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <map>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <Engine/Application.h>
#include <Engine/Box.h>
#include <Engine/Mesh.h>
#include <Engine/Renderer.h>
#include <Engine/Sphere.h>
#include <Engine/Texture.h>

class Example : public Engine::Application {
public:
  Example(int argc, char **argv) : Engine::Application(1800, 1000, argc, argv) {
    // Get the renderer for this application.
    auto &renderer = getRenderer();

    // ----------- Author Shaders -------------
    auto basic_lighting_cb = [this](Engine::Shader &shader) {
        shader.setMatrix("view", getViewMatrix());
        shader.setMatrix("proj", getProjMatrix());
        shader.setVec3("lightColour", vec3(0.8f));
    };

    auto shader_id = renderer.createShader({
      "cell_shaded.vs", // vertex shader
      "cell_shaded.fs", // fragment shader
      "", // no geometry shader
      basic_lighting_cb
    });

    // -------------- Create Renderables -----------------
    auto default_mat = Engine::Material{};
    auto sphere = renderer.createRenderable<Engine::Sphere>(
      default_mat,
      shader_id,
      vec3(0, 0, 0), 1.0f, 3);
 
    auto cb = [this](Engine::Shader &shader, const Engine::Sphere &m) {
          shader.setMatrix("model", mWorldTransform * m.getModelMat());
        };
    sphere->bindCallback(cb);

    auto box = renderer.createRenderable<Engine::Box>(
      default_mat,
      shader_id,
      vec3{-15, -15, -15},
      30, 30, 30
    );

    auto bcb = [this](Engine::Shader &shader, const Engine::Box &m) {
          shader.setMatrix("model", m.getModelMat());
        };
    box->mesh().flipNormals();
    box->bindCallback(bcb);

    // -------------- Setup Callbacks -----------------
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::mem_fn;
    using std::bind;
    std::function<void(int, int)> key_cb = bind(mem_fn(&Example::keyCB), this, _1, _2);
    std::function<void(int, int)> button_cb = bind(mem_fn(&Example::mouseButtonCB), this, _1, _2);
    std::function<void(double, double)> mouse_cb = bind(mem_fn(&Example::mouseCB), this, _1, _2);
    
    mInputHandler->addKeyCallback(key_cb);
    mInputHandler->setMouseButtonCallback(button_cb);
    mInputHandler->setMouseCallback(mouse_cb);
  }

  void tick(float deltaTime) override {
    // If we have let go of the mouse and we were moving as we did so, slowly
    // slow down the rotation.
    if (!mMouseDown && mRotVel != 0.0f) {
      auto rads = glm::radians(mRotVel);
      mWorldRotation = glm::rotate(mWorldRotation, rads, vec3(0, 1, 0));
      mWorldTransform = mWorldTranslation * mWorldRotation;
      mRotVel += mRotVel < 0 ? 0.2f : -0.2f;
      if (abs(mRotVel) < 0.05)
        mRotVel = 0;
    }
  }

private:
  void mouseCB(double xpos, double ypos) {
    if (mMouseDown) {
      if (mFirstMouse) {
        mLastMouseXPos = int(xpos);
        mFirstMouse = false;
      }
      auto delta = xpos - mLastMouseXPos;
      mRotVel = float(delta);
      mLastMouseXPos = int(xpos);
      auto rads = glm::radians(mRotVel);
      mWorldRotation = glm::rotate(mWorldRotation, rads, vec3(0, 1, 0));
      mWorldTransform = mWorldTranslation * mWorldRotation;
    }
  }

  void mouseButtonCB(int key, int action) {
    if (key == GLFW_MOUSE_BUTTON_LEFT) {
      mMouseDown = action == GLFW_PRESS;
      if (mMouseDown) {
        mFirstMouse = true;
        mRotVel = 0;
      }
    }
  }

  void keyCB(int key, int action) {
    if (action == GLFW_PRESS) {
      switch (key) {
      case GLFW_KEY_Q:
        setShouldCloseWindow();
        break;
      }
    }
  }

  // Input state members.
  int   mLastMouseXPos;
  bool  mMouseDown = false;
  bool  mFirstMouse = true;
  float mRotVel = 0.0f;
};

int main(int argc, char **argv) {
  Example app(argc, argv);
  app.run();
  return 0;
}