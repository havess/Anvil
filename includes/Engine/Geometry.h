#pragma once

#include "Types.h"

#include <set>
#include <unordered_map>
#include <vector>

namespace Engine {
namespace Geometry {

class PointListener {
public:
  virtual void update() = 0;
};

class BaseGeometry;
/// GeoFace is the representation of a triangle in 3 space
/// faces might have a normal for geometric computation
/// Each face has a list of point IDs which should be
/// provide in CCW order according to the normal
/// !!! NOTE: the normal must not be normalized !!!
class Face : public PointListener {
public:
  Face(const BaseGeometry *geo, uint32_t p1, uint32_t p2, uint32_t p3);
  virtual ~Face() = default;
  void update();

  inline const vec3 &getNormal() const { return mNormal; }

private:
  const BaseGeometry *mGeo;
  vec3 mNormal;
  uint32_t mPointIDs[3];
};

/// A point is a simple representation of a position in 3 space and should
/// be what we use when manipulating geometry
class Point {
public:
  Point(const vec3 &pos);

  void subscribe(PointListener *pl);
  void unsubscribe(PointListener *pl);

  bool hasSubscribers() const { return !mSubscribers.empty(); }

  void setPos(const vec3 &pos);
  void setNormal(const vec3 &normal);

  inline const vec3 &getPos() const { return mPos; }
  inline const vec3 &getNormal() const { return mNormal; }

  Point operator+(const Point &that) const {
    return Point{this->mPos + that.mPos};
  }

  Point operator-(const Point &that) const {
    return Point{this->mPos - that.mPos};
  }

private:
  void broadcast() const;

  std::set<PointListener *> mSubscribers;

  vec3 mPos;
  vec3 mNormal;
};

class BaseGeometry {
  friend Face;

protected:
  inline size_t getNumPoints() const { return mPoints.size(); }

  virtual void addPoint(const vec3 &v) { mPoints.emplace_back(Point{v}); }

  const Point &getPoint(uint32_t index) const { return mPoints[index]; }

  // This function assumes that the indices were given in counter clockwise
  // order and will construct the normal to reflect that
  void addFace(uint32_t index1, uint32_t index2, uint32_t index3);
  void addFace(std::tuple<uint32_t, uint32_t, uint32_t>);

  std::vector<Point> mPoints;
  std::vector<Face> mFaces;

private:
  void computeNormal(uint32_t point);
  std::unordered_map<uint32_t, std::vector<uint32_t>> mPointFaces;
};
} // namespace Geometry
} // namespace Engine
