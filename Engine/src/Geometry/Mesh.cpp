#include <Engine/Mesh.h>
#include <Engine/Application.h>

#include <stdio.h>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_set>



namespace Engine {

template <typename D, typename... Types>
Face::Face(const Mesh<D, Types...> *geo, uint32_t p1, uint32_t p2, uint32_t p3) : mGeo(geo) {
  mPointIDs[0] = p1;
  mPointIDs[1] = p2;
  mPointIDs[2] = p3;
  const vec3 &a = geo->getPoint(p1).getPos(), &b = geo->getPoint(p2).getPos(),
             &c = geo->getPoint(p3).getPos();

  mNormal = glm::normalize(glm::cross(c - a, b - a));
}

Point::Point(const vec3 &pos) : mPos(pos) {}
Point::Point(const vec3 &pos, const vec2 &uv) : mPos(pos), mUV(uv) {}
Point::Point(float x, float y, float z) : Point(vec3(x, y, z)) {}

void Point::setPos(const vec3 &pos) { mPos = pos; }

void Point::setNormal(const vec3 &normal) { mNormal = normal; }
} // namespace Engine
