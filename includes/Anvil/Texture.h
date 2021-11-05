#pragma once

#include <string>

#include "Types.h"

namespace Engine {

class Texture {
public:
  Texture(const std::string &path);
  void bind();

private:
  int mWidth, mHeight, mNumChannels;
  unsigned char *mData;
  uint mTexture;
};
} // namespace Engine
