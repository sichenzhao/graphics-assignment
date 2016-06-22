#pragma once

#define BV
#include <glm/glm.hpp>

enum class PrimType {
    Primitive,
    Sphere,
    Cube,
    NonhierSphere,
    NonhierBox,
    Mesh
};

class Primitive {
public:
    PrimType m_type;
    Primitive();
    virtual ~Primitive();
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