// Winter 2019

#pragma once

#include "Box.h"
#include "Types.h"

class Maze {
public:
  Maze(size_t dim);
  ~Maze();

  void reset();

  size_t getDim() const;

  int getValue(int x, int y) const;

  void setValue(int x, int y, int h);

  void digMaze();
  void printMaze(); // for debugging
  inline std::pair<int, int> getStart() const { return mStart; }

private:
  size_t mDim;
  int *mValues;
  void recDigMaze(int r, int c);
  int numNeighbors(int r, int c);
  std::pair<int, int> mStart;
};
