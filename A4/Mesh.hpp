#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

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

// A polygonal mesh.
class Mesh : public Primitive {
    //bool hitTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 eye, glm::vec3 dir, double &lt, const double min, const double max, glm::vec3& n);
public:
    Mesh( const std::string& fname );
    
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max);
    
    std::vector<glm::vec3> m_vertices;
    std::vector<Triangle> m_faces;
    
#ifdef BV
    // Bounding Volume box
    BoundingVolume bvb;
#endif
    
    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
