#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

// Represents a simple point light.
struct Light {
  Light();
  
  glm::vec3 colour;
  glm::vec3 position;
  double falloff[3]; // 1/(c1 + c2r + c3(r^2))
};

std::ostream& operator<<(std::ostream& out, const Light& l);
