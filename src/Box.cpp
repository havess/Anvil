#include "Anvil/Box.h"
namespace Anvil {

Box::Box(const vec3 &position, float length, float width, float height)
    : Mesh(), mPosition(position), mLength(length), mWidth(width),
      mHeight(height) {
  auto lW = width / 2.0f, hW = width / 2.0f, hH = height / 2.0f;

  addPoint(position + vec3(-lW, -hW, -hH));
  addPoint(position + vec3(-lW, -hW, hH));
  addPoint(position + vec3(-lW, hW, -hH));
  addPoint(position + vec3(-lW, hW, hH));
  addPoint(position + vec3(lW, -hW, -hH));
  addPoint(position + vec3(lW, -hW, hH));
  addPoint(position + vec3(lW, hW, -hH));
  addPoint(position + vec3(lW, hW, hH));

  addFace(0, 1, 2);
  addFace(0, 2, 3);
  addFace(0, 4, 7);
  addFace(0, 7, 3);
  addFace(1, 5, 6);
  addFace(1, 6, 2);
  addFace(4, 5, 6);
  addFace(4, 6, 7);
  addFace(6, 7, 2);
  addFace(6, 2, 3);
  addFace(4, 5, 1);
  addFace(4, 1, 0);
}
} // namespace Anvil
