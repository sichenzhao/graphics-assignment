#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
    PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
    virtual ~PhongMaterial();
    
    glm::vec3 m_kd;
    glm::vec3 m_ks;
    
    glm::vec3 ** uv;
    int width, height;
    glm::vec3 get_m_kd();
    glm::vec3 get_m_kd(double x, double y);
    
    double m_shininess;
};
