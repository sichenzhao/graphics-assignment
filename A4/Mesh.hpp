#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
    size_t v1;
    size_t v2;
    size_t v3;
    
    Triangle( size_t pv1, size_t pv2, size_t pv3 )
    : v1( pv1 )
    , v2( pv2 )
    , v3( pv3 )
    {}
};

// A polygonal mesh.
class Mesh : public Primitive {
    bool hitTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 eye, glm::vec3 dir, double &lt, const double min, const double max, glm::vec3& n);
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
