#include <iostream>

#include <Engine/Application.h>
#include <Engine/Camera.h>

namespace Engine {
Camera::Camera(vec3 position, vec3 up, vec3 front)
      : mPos(position), mUp(up), mFront(front), mRight(glm::cross(front, up)) {}

mat4
Camera::getViewMatrix() const { 
    return glm::lookAt(mPos, mFront, mUp);
}

mat4
Camera::getProjMatrix(const Application &app) const { 
    auto fb_width = (float) app.getFramebufferWidth();
    auto fb_height = (float) app.getFramebufferHeight();
    return glm::perspective(
        glm::radians(45.0f), fb_width/fb_height, 0.1f, 1000.0f);
}

OrthoCamera::OrthoCamera(vec3 position, vec3 up, vec3 front, float width, float height)
    : Camera(position, up, front), mWidth(width), mHeight(height) {}

mat4
OrthoCamera::getViewMatrix() const { 
    return mat4{1.0f};
}

mat4
OrthoCamera::getProjMatrix(const Application &app) const {
    std::cout << "Get Proj Matrix" << std::endl;
    return glm::ortho(0.0f, mWidth, 0.0f, mHeight, -1000.0f, 1000.0f);
}

} // namespace Engine