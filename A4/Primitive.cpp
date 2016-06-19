#include "Primitive.hpp"

Primitive::Primitive():m_type(PrimType::Primitive)
{
}

Primitive::~Primitive()
{
}

Sphere::Sphere()
{
    m_type = PrimType::Sphere;
}

Sphere::~Sphere()
{
}

Cube::Cube()
{
    m_type = PrimType::Cube;
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::~NonhierBox()
{
}
