#include <iostream>

#include <Engine/Texture.h>
#define STB_IMAGE_IMPLEMENTATION
#include <Engine/stb_image.h>

namespace Engine {

Texture::Texture(const std::string &path) {
  /*glGenTextures(1, &mTexture);
  glBindTexture(GL_TEXTURE_2D, mTexture);
  // set the texture wrapping/filtering options (on the currently bound
  // texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load and generate the texture
  stbi_set_flip_vertically_on_load(true);
  mData = stbi_load(path.c_str(), &mWidth, &mHeight, &mNumChannels, 0);
  if (mData) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mWidth, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, mData);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load texture" << std::endl;
    std::cerr << stbi_failure_reason() << std::endl;
  }
  stbi_image_free(mData);*/
}

void Texture::bind() { 
  //glBindTexture(GL_TEXTURE_2D, mTexture);
}

} // namespace Engine
