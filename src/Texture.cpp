#include <GL/glew.h>
#include <iostream>

//#include "stb/stb_image.h"
#include "Anvil/Texture.h"

namespace Anvil {

Texture::Texture(const std::string &path) {
  /*  glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    // set the texture wrapping/filtering options (on the currently bound
  texture
    // object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
  //  stbi_set_flip_vertically_on_load(true);
  //  mData = stbi_load(path.c_str(), &mWidth, &mHeight, &mNumChannels, 0);
    if (mData) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mWidth, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, mData);
      glGenerateMipmap(GL_TEXTURE_2D);
    } else {
      std::cout << "Failed to load texture" << std::endl;
    }
  //  stbi_image_free(mData);*/
}

void Texture::bind() { glBindTexture(GL_TEXTURE_2D, mTexture); }

} // namespace Anvil
