#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Material.hpp"
#include "PhongMaterial.hpp"

// important!! In order for grid to work, cannot be changed
//#define BV

// important!! In order for mirror reflection
#define MR

// important!! In order for antiAliasing works
//#define AA

// important!! In order for Depth of Field works
//#define DF

//static const float inff = std::numeric_limits<float>::infinity();
static const double infd = std::numeric_limits<double>::infinity();

static const double eps = 0.00001;

enum class PrimType {
    Primitive,
    Sphere,
    Cube,
    NonhierSphere,
    NonhierBox,
    Mesh,
    BoundingVolume,
    Triangle,
    Ellipsoid,
    EllipticCone
};

struct IntersecInfo{
    glm::vec4 normal;
    glm::vec4 hitPoint;
    PhongMaterial* mat;
    bool isIntersect;
    float t;
    double u; // x
    double v; // y
    bool hitIn;
    
    IntersecInfo(glm::vec4 norm, glm::vec4 hitP, bool isInter, float lt, bool hitIn, double u=0, double v=0){
        normal = norm;
        hitPoint = hitP;
        mat = NULL;
        t = lt;
        isIntersect = isInter;
        this->u = u;
        this->v = v;
        this->hitIn = hitIn;
    }

    ~IntersecInfo(){
        // mat is shared, will be deleted as program finished
    }
};

class Primitive {
public:
    PrimType m_type;
    Primitive();
    virtual std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max) = 0;
    virtual ~Primitive();
};

class EllipticCone : public Primitive {
public:
    double p;
    double q;
    double r;
    double height;
    
    EllipticCone(double p, double q, double r, double height);
    
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max);
    
    virtual ~EllipticCone();
};

class Ellipsoid : public Primitive {
public:
    double a;
    double b;
    double c;
    
    Ellipsoid(double a, double b, double c);
    
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max);
    
    virtual ~Ellipsoid();
};

class BoundingVolume : public Primitive {
public:
    double xmin, xmax, ymin, ymax, zmin, zmax;
    
    // Have to support default Ctor for mesh
    BoundingVolume();
    
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max);
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max, double* anotherT);
    
    BoundingVolume(std::vector<glm::vec3> m_vertices);
    
    virtual ~BoundingVolume();
};

class NonhierSphere : public Primitive {
public:
    glm::vec3 m_pos;
    double m_radius;
    
    NonhierSphere(const glm::vec3& pos, double radius);
    virtual ~NonhierSphere();
    
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max);

};

class NonhierBox : public Primitive {
    BoundingVolume realBox;
public:
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max);
    
    NonhierBox(const glm::vec3& pos, double size);
    
    virtual ~NonhierBox();
    
    glm::vec3 m_pos;
    double m_size;
};

class Sphere : public Primitive {
public:
    NonhierSphere realSphere;
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max);
    
    Sphere();
    virtual ~Sphere();
};

class Cube : public Primitive {
public:
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 p, glm::vec4 ray, const double min, const double max);
    NonhierBox realCube;
    Cube();
    virtual ~Cube();
};


class Triangle: public Primitive
{
public:
    size_t iv1;
    size_t iv2;
    size_t iv3;
    const std::vector<glm::vec3>* m_vertices;
    
    Triangle( size_t pv1, size_t pv2, size_t pv3 )
    : iv1( pv1 )
    , iv2( pv2 )
    , iv3( pv3 )
    , m_vertices( NULL )
    {
        m_type = PrimType::Triangle;
    }
    
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max);
};
