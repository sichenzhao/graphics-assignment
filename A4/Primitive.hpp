#pragma once

#include <vector>
#include <glm/glm.hpp>
#define BV

enum class PrimType {
    Primitive,
    Sphere,
    Cube,
    NonhierSphere,
    NonhierBox,
    Mesh,
    BoundingVolume
};

class Primitive {
public:
    PrimType m_type;
    Primitive();
    virtual ~Primitive();
};

class BoundingVolume : public Primitive {
public:
    double xmin, xmax, ymin, ymax, zmin, zmax;
    BoundingVolume(){}
    BoundingVolume(std::vector<glm::vec3> m_vertices){
        xmin = ymin = zmin = +std::numeric_limits<float>::infinity();
        xmax = ymax = zmax = -std::numeric_limits<float>::infinity();
        m_type = PrimType::BoundingVolume;
        for (auto it = m_vertices.begin(); it != m_vertices.end(); it++) {
            glm::vec3 tmp = *it;
            xmin = std::min(xmin, (double)tmp.x);
            ymin = std::min(ymin, (double)tmp.y);
            zmin = std::min(zmin, (double)tmp.z);
            xmax = std::max(xmax, (double)tmp.x);
            ymax = std::max(ymax, (double)tmp.y);
            zmax = std::max(zmax, (double)tmp.z);
        }
    }
    virtual ~BoundingVolume();
};

class NonhierSphere : public Primitive {
public:
    NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
    {
        m_type = PrimType::NonhierSphere;
    }
    virtual ~NonhierSphere();
    
    glm::vec3 m_pos;
    double m_radius;
};

class NonhierBox : public Primitive {
public:
    NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
    {
        m_type = PrimType::NonhierBox;
    }
    
    virtual ~NonhierBox();
    
    glm::vec3 m_pos;
    double m_size;
};

class Sphere : public Primitive {
public:
    NonhierSphere realSphere;
    Sphere()
    : realSphere(NonhierSphere(glm::vec3(0.0f), 1.0)){
        m_type = PrimType::Sphere;
    }
    virtual ~Sphere();
};

class Cube : public Primitive {
public:
    NonhierBox realCube;
    Cube()
    : realCube(NonhierBox(glm::vec3(0.0f), 1.0)){
        m_type = PrimType::Cube;
    }
    virtual ~Cube();
};