#pragma once

// Helper file for typedefs for consistency, honestly
// I don't like this but for now this is a thing
#include <glm/glm.hpp>
#include <memory>

using uint = unsigned int;
template <typename T> using sptr = std::shared_ptr<T>;
template <typename T> using uptr = std::unique_ptr<T>;

using mat4 = glm::mat4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;
