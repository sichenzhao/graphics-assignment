#include "Primitive.hpp"

Primitive::Primitive():m_type(PrimType::Primitive)
{
}

Primitive::~Primitive()
{
}

// TODO: Shouldn't support default Ctor
//BoundingVolume::BoundingVolume(){}

BoundingVolume::BoundingVolume(std::vector<glm::vec3> m_vertices){
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

std::shared_ptr<IntersecInfo> BoundingVolume::intersect(glm::vec4 p, glm::vec4 ray, const double min, const double max){
    // init
    double lt = infd;
    
    glm::vec4 b0 = glm::vec4(xmin, ymin, zmin, 0.0f);
    glm::vec4 b1 = glm::vec4(xmax, ymax, zmax, 0.0f);
    
    glm::vec3 normal;
    
    double tmin, tmax, txmin, txmax, tymin, tymax, tzmin, tzmax;
    
    if (ray.x >= 0) {
        txmin = tmin = (b0.x - p.x) / ray.x;
        txmax = tmax = (b1.x - p.x) / ray.x;
    } else {
        txmin = tmin = (b1.x - p.x) / ray.x;
        txmax = tmax = (b0.x - p.x) / ray.x;
    }
    
    if (ray.y >= 0) {
        tymin = (b0.y - p.y) / ray.y;
        tymax = (b1.y - p.y) / ray.y;
    } else {
        tymin = (b1.y - p.y) / ray.y;
        tymax = (b0.y - p.y) / ray.y;
    }
    
    if ( (tmin > tymax) || (tymin > tmax) )
        return NULL;
    
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    
    if (ray.z >= 0) {
        tzmin = (b0.z - p.z) / ray.z;
        tzmax = (b1.z - p.z) / ray.z;
    } else {
        tzmin = (b1.z - p.z) / ray.z;
        tzmax = (b0.z - p.z) / ray.z;
    }
    
    if ( (tmin > tzmax) || (tzmin > tmax) )
        return NULL;
    
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    
    if((tmin >= min + eps) && (tmax < max - eps)) {
        assert(tmin <= tmax);
        // hits nh_cube
        lt = std::min(lt, tmin);
        
        if(lt > txmin - eps && lt < txmin + eps) normal = glm::vec3(1,0,0);
        if (lt > txmax - eps && lt < txmax + eps) normal = glm::vec3(-1, 0, 0);
        if (lt > tymin - eps && lt < tymin + eps) normal = glm::vec3(0, 1, 0);
        if (lt > tymax - eps && lt < tymax + eps) normal = glm::vec3(0, -1, 0);
        if (lt > tzmin - eps && lt < tzmin + eps) normal = glm::vec3(0, 0, 1);
        if (lt > tzmax - eps && lt < tzmax + eps) normal = glm::vec3(0, 0, -1);
        return std::shared_ptr<IntersecInfo>( new IntersecInfo(glm::vec4(normal, 0.0f), p + ((float)lt * ray), true, lt));
    }
    return NULL;
}

BoundingVolume::~BoundingVolume()
{
}

Sphere::Sphere()
: realSphere(NonhierSphere(glm::vec3(0.0f), 1.0)){
    m_type = PrimType::Sphere;
}

std::shared_ptr<IntersecInfo> Sphere::intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max){
    return realSphere.intersect(primaryPoint, primaryRay, min, max);
}

Sphere::~Sphere()
{
}

Cube::Cube()
: realCube(NonhierBox(glm::vec3(0.0f), 1.0)){
    m_type = PrimType::Cube;
}

std::shared_ptr<IntersecInfo> Cube::intersect(glm::vec4 p, glm::vec4 ray, const double min, const double max){
    return realCube.intersect(p, ray, min, max);
}

Cube::~Cube()
{
}

NonhierSphere::NonhierSphere(const glm::vec3& pos, double radius)
: m_pos(pos), m_radius(radius)
{
    m_type = PrimType::NonhierSphere;
}

std::shared_ptr<IntersecInfo> NonhierSphere::intersect(glm::vec4 p, glm::vec4 ray, const double min, const double max){
    double lt = infd;
    bool nhsBool = false;
    
    glm::vec4 center = glm::vec4(m_pos, 1.0f);
    glm::vec4 hitNormal = glm::vec4(0.0f);
    
    float A = glm::dot(ray, ray);
    float B = glm::dot(p - center, ray);
    B = 2*B;
    float C = dot(p - center, p - center) - pow(m_radius, 2);
    
    float determ = pow(B, 2) - 4*A*C;
    
    if(determ < 0 - eps){
        // no root
        return NULL;
    } else {
        if(determ < 0 + eps){
            // one root
            lt = -B/(2*A);
        } else {
            // two roots
            lt = -B - sqrt(determ);
            lt = lt / (2*A);
        }
        
        if(lt >= min + eps && lt < max - eps){
            nhsBool = true;
            // closest one
            hitNormal = (p + (ray)*(float)lt - center);
            return std::shared_ptr<IntersecInfo>( new IntersecInfo(hitNormal, p+(float)lt*ray, true, lt));
        }
    }
    return NULL;
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::NonhierBox(const glm::vec3& pos, double size)
: m_pos(pos), m_size(size), realBox(std::vector<glm::vec3>({pos, pos + glm::vec3(size)}))
{
    m_type = PrimType::NonhierBox;
}

std::shared_ptr<IntersecInfo> NonhierBox::intersect(glm::vec4 p, glm::vec4 ray, const double min, const double max){
    return realBox.intersect(p, ray, min, max);
}

NonhierBox::~NonhierBox()
{
}
