#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <map>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#define PI 3.14159265

#include <Engine/Application.h>
#include <Engine/Gadgets.h>
#include <Engine/Sphere.h>
#include <Engine/Mesh.h>
#include <Engine/Renderer.h>

class Example : public Engine::Application {
public:
  Example(int argc, char **argv) : 
    Engine::Application(1800, 1000, argc, argv),
    mOrthoCamera{vec3{0, 0, 0}, vec3{0, 1, 0}, vec3{0, 0, -1}, getFramebufferWidth(), getFramebufferHeight()}{
  
    // Setup camera
    attachCamera(mOrthoCamera);

    // Get the renderer for this application.
    auto &renderer = getRenderer();

    // -------------- Create Renderables -----------------
    
    using Engine::Gadgets::Line;
    auto shader_cb = [this](Engine::Shader &shader) {
        shader.setMatrix("view", mOrthoCamera.getViewMatrix());
        shader.setMatrix("proj", mOrthoCamera.getProjMatrix(*this));
        shader.setFloat("thickness", 0.2f);
        shader.setFloat("aspect", getFramebufferWidth()/getFramebufferHeight());
        shader.setVec3("color", vec3{1, 1, 1});
    };
    auto line_renderable = std::make_unique<Line>(getRenderer(), shader_cb);
    auto id = line_renderable->shaderID();
    mLine = dynamic_cast<Line*>(renderer.addRenderable<Line::Mesh>(id, std::move(line_renderable)));
    auto cb = [this](Engine::Shader &shader, const Line::Mesh &m) {
      shader.setMatrix("model", m.getModelMat());
    };
    mLine->bindCallback(cb);
    

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
    float period = 4.0f;
    float time_passed = deltaTime - int(deltaTime/period) * period;
    mLine->startLine();
    mLine->addPoint(vec3{0,25,0});
    mLine->addPoint(vec3{25,0,0});
    /*for(int i = 0; i < 100; i++) {
      double x = float(i)/100.0 * 14 * PI * (time_passed/period);
      mLine->addPoint(vec3{x, sin(x) + sin(x/2.0) + sin(x/1.6), 0});
    }*/
    mLine->endLine();
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

  // Camera
  Engine::OrthoCamera mOrthoCamera;

  // Input state members.
  int   mLastMouseXPos;
  bool  mMouseDown = false;
  bool  mFirstMouse = true;
  float mRotVel = 0.0f;
  Engine::Gadgets::Line *mLine = nullptr;
};

int main(int argc, char **argv) {
  Example app(argc, argv);
  app.run();
  return 0;
}