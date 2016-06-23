#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
    m_type = PrimType::Mesh;
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}

#ifdef BV
    bvb = BoundingVolume(m_vertices);
#endif
}

bool Mesh::hitTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 primaryPoint, glm::vec3 primaryRay, double &lt, const double min, const double max, glm::vec3& n){
    // Möller-Trumbore algorithm
    bool retBool = false;
    
    glm::vec3 e1, e2; // Edge1, Edge2
    glm::vec3 P, Q, T;
    double det, inv_det, u, v;
    double t;
    
    e1 = v2 - v1;
    e2 = v3 - v1;
    
    // see if primaryRay parallel to plane of triangle
    P = glm::cross(primaryRay, e2);
    det = glm::dot(e1, P);
    if(det > -eps && det < eps) return false;
    inv_det = 1 / det;
    
    
    T = primaryPoint - v1;
    u = glm::dot(T, P) * inv_det;
    if(u < -eps || u > 1+eps) return false;
    
    Q = glm::cross(T, e1);
    v = glm::dot(primaryRay, Q) * inv_det;
    if(v<-eps || u+v > 1+eps) return false;
    
    t = glm::dot(e2, Q)*inv_det;
    
    if((t >= min+eps) && (t <= max-eps)){
        n = glm::cross(e1, e2);
        if(glm::dot(n, primaryRay) > 0-eps){
            n = -n;
        }
        lt = t;
        return true;
    }
    
    
    return retBool;
}


std::shared_ptr<IntersecInfo> Mesh::intersect(glm::vec4 primaryPoint, glm::vec4 primaryRay, const double min, const double max){
#ifdef BV
    return bvb.intersect(primaryPoint, primaryRay, min, max);
#else
    bool mBool = false;
    double tMesh = infd;
    double lt = infd;
    glm::vec3 normalTriangle;
    glm::vec3 v1, v2, v3, n;
    for(auto it = m_faces.begin(); it != m_faces.end(); it++){
        v1 = m_vertices[(*it).v1];
        v2 = m_vertices[(*it).v2];
        v3 = m_vertices[(*it).v3];
        
        if(hitTriangle(v1, v2, v3, glm::vec3(primaryPoint), glm::vec3(primaryRay), lt, min, max, n)){
            mBool = true;
            if(lt < tMesh){
                tMesh = lt;
                normalTriangle = n;
            }
        }
    }
    
    if (mBool) {
        return std::shared_ptr<IntersecInfo>(new IntersecInfo(glm::vec4(normalTriangle, 0.0), primaryPoint + (float)tMesh * primaryRay, true, tMesh));
    }
    return NULL;
#endif
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
