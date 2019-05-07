#include "Anvil/Geometry.h"
#include <iostream>

namespace Anvil {
namespace Geometry {

Face::Face(const BaseGeometry *geo, uint32_t p1, uint32_t p2, uint32_t p3)
    : mGeo(geo) {
  mPointIDs[0] = p1;
  mPointIDs[1] = p2;
  mPointIDs[2] = p3;
}

void Face::update() {
  const vec3 &a = mGeo->getPoint(mPointIDs[0]).getPos(),
             &b = mGeo->getPoint(mPointIDs[1]).getPos(),
             &c = mGeo->getPoint(mPointIDs[2]).getPos();

  mNormal = glm::normalize(glm::cross(c - a, b - a));
}

Point::Point(const vec3 &pos) : mPos(pos) {}

void Point::subscribe(PointListener *pl) { mSubscribers.insert(pl); }

void Point::unsubscribe(PointListener *pl) { mSubscribers.erase(pl); }

void Point::setPos(const vec3 &pos) {
  mPos = pos;
  broadcast();
}

void Point::setNormal(const vec3 &normal) {
  mNormal = normal;
  broadcast();
}

void Point::broadcast() const {
  for (auto sub : mSubscribers) {
    sub->update();
  }
}

// This function assumes that the indices were given in counter clockwise
// order and will construct the normal to reflect that
void BaseGeometry::addFace(uint32_t index1, uint32_t index2, uint32_t index3) {
  Face f{this, index1, index2, index3};
  mFaces.push_back(f);
  mFaces.back().update();

  mPoints[index1].subscribe(&f);
  mPoints[index2].subscribe(&f);
  mPoints[index3].subscribe(&f);
  mPointFaces[index1].push_back(mFaces.size() - 1);
  mPointFaces[index2].push_back(mFaces.size() - 1);
  mPointFaces[index3].push_back(mFaces.size() - 1);
  computeNormal(index1);
  computeNormal(index2);
  computeNormal(index3);
}

void BaseGeometry::addFace(std::tuple<uint32_t, uint32_t, uint32_t> tuple) {
  auto [a, b, c] = tuple;
  addFace(a, b, c);
}

void BaseGeometry::computeNormal(uint32_t point) {
  auto faces = mPointFaces.at(point);
  vec3 newNormal;
  for (auto f : faces) {
    newNormal += mFaces[f].getNormal();
  }
  newNormal = glm::normalize(newNormal);
  mPoints[point].setNormal(newNormal);
}

} // namespace Geometry
} // namespace Anvil
