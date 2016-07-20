#include "Primitive.hpp"
#include "polyroots.hpp"

#include <iostream>

using namespace std;

Primitive::Primitive():m_type(PrimType::Primitive)
{
}

Primitive::~Primitive()
{
}

Ellipsoid::Ellipsoid(double a, double b, double c)
: a(a), b(b), c(c)
{}

std::shared_ptr<IntersecInfo> Ellipsoid::intersect(glm::vec4 eye, glm::vec4 ray, const double min, const double max) {
    glm::vec3 p = glm::vec3(eye);
    glm::vec3 q = glm::vec3(a, b, c);
    glm::vec3 r = glm::vec3(ray);
    double lt = infd;
    glm::vec4 hitNormal = glm::vec4(0.0);
    glm::vec4 hitPoint = glm::vec4(0.0);
    bool hitIn = true;
    
    float A = glm::dot((r/q), (r/q));
    float B = 2 * glm::dot(p/q, r/q);
    float C = dot(p/q, p/q) - 1;
    
    double roots[2];
    size_t rootsNum = quadraticRoots(A, B, C, roots);
    
    if (rootsNum == 0) {
        return NULL;
    } else if (rootsNum == 1) {
        lt = roots[0];
    } else {
        lt = std::min(roots[0], roots[1]);
        if(lt <= min + eps){
            lt = std::max(roots[0], roots[1]);
            hitIn = false;
        }
    }
    
    if(lt >= min + eps && lt < max - eps){
        hitPoint = eye+(float)lt*ray;
        
        // get two points on plane, with hitPoint will get two vectors
        glm::vec3 v1, v2;
        v1 = glm::vec3(1,0,(1-hitPoint.x/a/a)*c*c/hitPoint.z) - glm::vec3(hitPoint);
        v2 = glm::vec3(0,1,(1-hitPoint.y/b/b)*c*c/hitPoint.z) - glm::vec3(hitPoint);
        hitNormal = glm::vec4(glm::cross(v1, v2), 0);
        if (glm::dot(hitNormal, ray) > 0) {
            hitNormal = -hitNormal;
        }
        return std::shared_ptr<IntersecInfo>( new IntersecInfo(hitNormal, hitPoint, true, lt, hitIn));
    }
    return NULL;
}

Ellipsoid::~Ellipsoid(){};

EllipticCone::EllipticCone(double a, double b, double c, double height)
: p(a), q(b), r(c), height(height)
{}

std::shared_ptr<IntersecInfo> EllipticCone::intersect(glm::vec4 eye, glm::vec4 ray, const double min, const double max) {
    if (std::abs(height) <= eps) {
        return NULL;
    }
    const double hCons1 = (height - eye.z)/ray.z;
    const double hCons2 = (-height-eye.z)/ray.z;
    const double heightMin = std::min(hCons1, hCons2);
    const double heightMax = std::max(hCons1, hCons2);
    
    const double appMin = std::max(min, heightMin);
    const double appMax = std::min(max, heightMax);
    
    bool hitIn = true;
    
    glm::vec3 h = glm::vec3(p*p, q*q, -r*r);
    glm::vec3 e = glm::vec3(eye);
    glm::vec3 R = glm::vec3(ray);
    double lt = infd;
    glm::vec4 hitNormal = glm::vec4(0.0);
    glm::vec4 hitPoint = glm::vec4(0.0);
    
    float A = glm::dot(R, R/h);
    float B = 2 * glm::dot(e, R/h);
    float C = dot(e, e/h);
    
    double roots[2];
    size_t rootsNum = quadraticRoots(A, B, C, roots);
    
    if (rootsNum == 0) {
        return NULL;
    } else if (rootsNum == 1) {
        lt = roots[0];
    } else {
        lt = std::min(roots[0], roots[1]);
        if(lt <= appMin + eps){
            lt = std::max(roots[0], roots[1]);
        }
    }
    
    if(lt >= appMin + eps && lt < appMax - eps){
        hitPoint = eye + (float)lt*ray;
        hitNormal = glm::normalize(glm::vec4(glm::cross(glm::vec3(hitPoint.x+1,hitPoint.y - q*q/p/p*hitPoint.x/hitPoint.y,0), glm::vec3(hitPoint)), 0));
        if (glm::dot(hitNormal, ray) > 0) {
            hitNormal = -hitNormal;
        }
        return std::shared_ptr<IntersecInfo>( new IntersecInfo(hitNormal, hitPoint, true, lt, hitIn));
    }
    return NULL;
}

EllipticCone::~EllipticCone(){};

// Need to support default Ctor for mesh
BoundingVolume::BoundingVolume(){}

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

std::shared_ptr<IntersecInfo> BoundingVolume::intersect(glm::vec4 p, glm::vec4 ray,const double min, const double max){
    return intersect(p, ray, min, max, NULL);
}

std::shared_ptr<IntersecInfo> BoundingVolume::intersect(glm::vec4 p, glm::vec4 ray,const double min, const double max, double* anotherT){
    assert(!isnan(ray.x) && !isnan(ray.y) && !isnan(ray.z));

    // init
    double lt = infd;
    
    glm::vec4 b0 = glm::vec4(xmin, ymin, zmin, 0.0);
    glm::vec4 b1 = glm::vec4(xmax, ymax, zmax, 0.0);
    
    glm::vec3 normal;
    bool hitIn  = true;
    
    double tmin, tmax, txmin, txmax, tymin, tymax, tzmin, tzmax;
    
    if (ray.x >= 0) {
        txmin = tmin = (b0.x - p.x) / (ray.x + eps);
        txmax = tmax = (b1.x - p.x) / (ray.x + eps);
    } else {
        txmin = tmin = (b1.x - p.x) / ray.x;
        txmax = tmax = (b0.x - p.x) / ray.x;
    }
    //assert(tmin <= tmax);
    
    if (ray.y >= 0) {
        tymin = (b0.y - p.y) / (ray.y + eps);
        tymax = (b1.y - p.y) / (ray.y + eps);
    } else {
        tymin = (b1.y - p.y) / ray.y;
        tymax = (b0.y - p.y) / ray.y;
    }
    
    if ( (tmin > tymax+eps) || (tymin-eps > tmax) )
        return NULL;
    
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    //assert(tmin <= tmax);
    
    if (ray.z >= 0) {
        tzmin = (b0.z - p.z) / (ray.z + eps);
        tzmax = (b1.z - p.z) / (ray.z + eps);
    } else {
        tzmin = (b1.z - p.z) / ray.z;
        tzmax = (b0.z - p.z) / ray.z;
    }
    
    if ( (tmin > tzmax+eps) || (tzmin-eps > tmax) )
        return NULL;
    
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    
    if(tmin > tmax){
        double tmp = tmin;
        tmin = tmax;
        tmax = tmp;
    }
    
    assert(tmin <= tmax);
    
    double realT;
    if (tmin<=min+eps) {
        if (tmax<=min+eps || tmax>=max-eps) {
            return NULL;
        }
        // return hit point
        realT = tmax;
        hitIn = false;
    } else if(tmin>=max - eps){
        return NULL;
    } else {
        realT = tmin;
    }
    
    if(anotherT!=NULL){
        *anotherT = std::min(tmax, max);
    }
    
    assert(tmin <= tmax);
    // hits nh_cube
    lt = std::min(lt, realT);
    
    if (lt > txmin - eps && lt < txmin + eps) normal = glm::vec3(1,0,0);
    if (lt > txmax - eps && lt < txmax + eps) normal = glm::vec3(-1, 0, 0);
    if (lt > tymin - eps && lt < tymin + eps) normal = glm::vec3(0, 1, 0);
    if (lt > tymax - eps && lt < tymax + eps) normal = glm::vec3(0, -1, 0);
    if (lt > tzmin - eps && lt < tzmin + eps) normal = glm::vec3(0, 0, 1);
    if (lt > tzmax - eps && lt < tzmax + eps) normal = glm::vec3(0, 0, -1);
    
    assert(lt > min+eps && lt < max - eps);

    // if not hit in, the normal should be reflected
    return std::shared_ptr<IntersecInfo>( new IntersecInfo(((hitIn)?1.0f:-1.0f) * glm::vec4(normal, 0.0), p + ((float)lt * ray), true, lt, hitIn));
}

BoundingVolume::~BoundingVolume()
{
}

Sphere::Sphere()
: realSphere(NonhierSphere(glm::vec3(0.0), 1.0)){
    m_type = PrimType::Sphere;
}

std::shared_ptr<IntersecInfo> Sphere::intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max){
    return realSphere.intersect(primaryPoint, primaryRay, min, max);
}

Sphere::~Sphere()
{
}

Cube::Cube()
: realCube(NonhierBox(glm::vec3(0.0), 1.0)){
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
    
    glm::vec4 center = glm::vec4(m_pos, 1.0);
    glm::vec4 hitNormal = glm::vec4(0.0);
    glm::vec4 hitPoint = glm::vec4(0.0);
    bool hitIn = true;
    
    float A = glm::dot(ray, ray);
    float B = glm::dot(p - center, ray);
    B = 2*B;
    float C = dot(p - center, p - center) - pow(m_radius, 2);
    
    /**
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
     **/
    
    double roots[2];
    size_t rootsNum = quadraticRoots(A, B, C, roots);
    
    if (rootsNum == 0) {
        return NULL;
    } else if (rootsNum == 1) {
        lt = roots[0];
    } else {
        lt = std::min(roots[0], roots[1]);
        if(lt <= min + eps){
            lt = std::max(roots[0], roots[1]);
            hitIn = false;
        }
    }
    
    if(lt >= min + eps && lt < max - eps){
        hitNormal = (p + (ray)*(float)lt - center);
        hitPoint = p+(float)lt*ray;
        
        
        // displacement for texture mapping
        glm::vec4 disp = hitPoint - center;
        
        double u,v; // u width, v height
        
        // width
        float angle_a = acos(-disp.z/sqrt(disp.x*disp.x + disp.z*disp.z));
        assert(angle_a <= glm::radians(180.0f) && angle_a >= glm::radians(0.0f));
        if (disp.x<0) {
            angle_a = glm::radians(360.0f) - angle_a;
        }
        u = angle_a/glm::radians(360.0f);
        if (u==1) {
            u = 0;
        }
        
        // height
        float angle_b = atan(disp.y/sqrt(disp.x*disp.x + disp.z*disp.z));
        assert(angle_b < glm::radians(90.0f) && angle_b > glm::radians(-90.0f));
        v = (angle_b+glm::radians(90.0f))/glm::radians(180.0f);
        if (v == 1) {
            v = 0;
        }
        
        return std::shared_ptr<IntersecInfo>( new IntersecInfo(hitNormal, hitPoint, true, lt, hitIn, u, v));
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


std::shared_ptr<IntersecInfo> Triangle::intersect(glm::vec4 p, glm::vec4 ray, const double min, const double max){
    // Möller-Trumbore algorithm
    glm::vec3 n;
    
    glm::vec3 e1, e2; // Edge1, Edge2
    glm::vec3 P, Q, T;
    double det, inv_det, u, v;
    double t;
    
    glm::vec3 v1, v2, v3;
    v1 = (*m_vertices)[iv1];
    v2 = (*m_vertices)[iv2];
    v3 = (*m_vertices)[iv3];
    
    glm::vec3 primaryPoint = glm::vec3(p);
    glm::vec3 primaryRay = glm::vec3(ray);
    
    e1 = v2 - v1;
    e2 = v3 - v1;
    
    // see if primaryRay parallel to plane of triangle
    P = glm::cross(primaryRay, e2);
    det = glm::dot(e1, P);
    if(det > -eps && det < eps) return NULL;
    inv_det = 1 / det;
    
    
    T = primaryPoint - v1;
    u = glm::dot(T, P) * inv_det;
    if(u < -eps || u > 1+eps) return NULL;
    
    Q = glm::cross(T, e1);
    v = glm::dot(primaryRay, Q) * inv_det;
    if(v<-eps || u+v > 1+eps) return NULL;
    
    t = glm::dot(e2, Q)*inv_det;
    
    if((t > min+eps) && (t <= max-eps)){
        n = glm::cross(e1, e2);
        if(glm::dot(n, primaryRay) > 0-eps){
            n = -n;
        }
        return std::shared_ptr<IntersecInfo>(new IntersecInfo(glm::vec4(n, 0.0), p+(((float)(t-eps))*ray), true, t, true));
    }
    return NULL;
}