#pragma once

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

class Sphere : public Primitive {
public:
    Sphere();
    virtual ~Sphere();
};

class Cube : public Primitive {
public:
    Cube();
    virtual ~Cube();
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
