#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Material.hpp"
#include "PhongMaterial.hpp"
//#define BV

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
    BoundingVolume
};

struct IntersecInfo{
    glm::vec4 normal;
    glm::vec4 hitPoint;
    PhongMaterial* mat;
    bool isIntersect;
    float t;
    
    IntersecInfo(glm::vec4 norm, glm::vec4 hitP, bool isInter, float lt){
        normal = norm;
        hitPoint = hitP;
        mat = NULL;
        t = lt;
        isIntersect = isInter;
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

class BoundingVolume : public Primitive {
public:
    double xmin, xmax, ymin, ymax, zmin, zmax;
    
    //BoundingVolume();
    
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max);
    
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
