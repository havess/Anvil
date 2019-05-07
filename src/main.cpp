#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Anvil/Box.h"
#include "Anvil/Mesh.h"
#include "Anvil/Renderer.h"
#include "Anvil/Sphere.h"
#include "Anvil/Window.h"

#define GL_ERROR_CHECK

int main() {
  printf("Starting up Anvil...\n");
  Anvil::Window window;
  sptr<Anvil::Renderer> renderer = std::make_shared<Anvil::Renderer>();

  Anvil::Sphere s0{vec3{-2, 0, 0}, 0.4f, 1};
  Anvil::Sphere s1{vec3{-1, 0, 0}, 0.4f, 2};
  Anvil::Sphere s2{vec3{0, 0, 0}, 0.4f, 3};
  Anvil::Sphere s3{vec3{1, 0, 0}, 0.4f, 4};
  Anvil::Sphere s4{vec3{2, 0, 0}, 0.4f, 5};
  Anvil::Box b0{vec3{0, 0, 0}, 1.0f, 1.0f, 1.0f};

  Anvil::Attribute a0{3, "pos"};
  Anvil::Attribute a1{3, "normal"};

  s0.addAttribute(a0);
  s0.addAttribute(a1);
  s1.addAttribute(a0);
  s1.addAttribute(a1);
  s2.addAttribute(a0);
  s2.addAttribute(a1);
  s3.addAttribute(a0);
  s3.addAttribute(a1);
  s4.addAttribute(a0);
  s4.addAttribute(a1);
  b0.addAttribute(a0);
  b0.addAttribute(a1);

  renderer->addMesh(s0);
  renderer->addMesh(s1);
  // renderer->addMesh(b0);
  renderer->addLight(s2);
  renderer->addMesh(s3);
  renderer->addMesh(s4);

  window.setRenderer(renderer);
  std::cout << "done setting renderer" << std::endl;
  window.run();

  printf("Exiting Anvil...\n");
  return 0;
}
