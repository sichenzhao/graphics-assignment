#include <glm/ext.hpp>
#include <algorithm>

#include "A4.hpp"

#include "GeometryNode.hpp"
#include "Mesh.hpp"

//#define DEBUG_Z
#ifdef DEBUG_Z
void dout(std::string msg){
    std::cout << msg << std::endl;
}
#else
#define RELEASE
void dout(std::string msg){}
#endif

static const float inff = std::numeric_limits<float>::infinity();
static const double infd = std::numeric_limits<double>::infinity();
//static const float eps = FLT_EPSILON;
static const float eps = 0.00001;

glm::vec3 rayColor(glm::vec3 eye, glm::vec3 pixelPoint, Light light, int lightNum, SceneNode* root, const glm::vec3 & ambient){
    glm::vec3 col = glm::vec3(0.0f);
    glm::vec3 hitNormal = glm::vec3(0.0f);
    double t = infd;
    PhongMaterial* mat = NULL;
    hitWrapper(root, eye, pixelPoint, &mat, t, hitNormal, glm::mat4(1.0f));
    
    if(infd==t || NULL==mat){
        // return background
        return col;
    } else {
        // hit
        
        // ambient light
        col.r = mat->m_kd.r*(ambient.r)/lightNum;
        col.g = mat->m_kd.g*(ambient.g)/lightNum;
        col.b = mat->m_kd.b*(ambient.b)/lightNum;
        
        glm::vec3 hitPoint = eye + t*(pixelPoint - eye);
        
        // determine shadow
        bool isShadow = false;
        
        // TODO: shadow of others
        {
            PhongMaterial* tmp = NULL;
            double tmpt = infd;
            glm::vec3 tmpNormal = glm::vec3(0.0f);
            isShadow = hitWrapper(root, hitPoint, light.position, &tmp, tmpt, tmpNormal,  glm::mat4(1.0f), 0+eps, 1-eps);
        }
        
        if (!isShadow) {
            // diffuse
            // direct light
            col += directLight(mat->m_kd, hitPoint, hitNormal, light.position, light.colour);
            
            // specular
            // TODO: do specular recursively, no need for simple image now
            col += indirectLight(mat->m_ks, hitPoint, hitNormal, light.position, light.colour, eye, mat->m_shininess);
            return col;
        }
    }
    return col;
}


bool hitBoundingBox(glm::vec3 b0, glm::vec3 b1, glm::vec3 eye, glm::vec3 dir, double &lt, double min, double max, glm::vec3& normal){
    
    // init
    lt = infd;
    
    double tmin, tmax, txmin, txmax, tymin, tymax, tzmin, tzmax;
    
    if (dir.x >= 0) {
        txmin = tmin = (b0.x - eye.x) / dir.x;
        txmax = tmax = (b1.x - eye.x) / dir.x;
    } else {
        txmin = tmin = (b1.x - eye.x) / dir.x;
        txmax = tmax = (b0.x - eye.x) / dir.x;
    }
    
    if (dir.y >= 0) {
        tymin = (b0.y - eye.y) / dir.y;
        tymax = (b1.y - eye.y) / dir.y;
    } else {
        tymin = (b1.y - eye.y) / dir.y;
        tymax = (b0.y - eye.y) / dir.y;
    }
    
    if ( (tmin > tymax) || (tymin > tmax) )
        return false;
    
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    
    if (dir.z >= 0) {
        tzmin = (b0.z - eye.z) / dir.z;
        tzmax = (b1.z - eye.z) / dir.z;
    } else {
        tzmin = (b1.z - eye.z) / dir.z;
        tzmax = (b0.z - eye.z) / dir.z;
    }
    
    if ( (tmin > tzmax) || (tzmin > tmax) )
        return false;
    
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
        return true;
    }
    return false;
}

bool hitTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 eye, glm::vec3 dir, double &lt, const double min, const double max, glm::vec3& n){
    // Möller-Trumbore algorithm
    lt = infd;
    bool retBool = false;
    
    glm::vec3 e1, e2; // Edge1, Edge2
    glm::vec3 P, Q, T;
    double det, inv_det, u, v;
    double t;
    
    e1 = v2 - v1;
    e2 = v3 - v1;
    
    // see if dir parallel to plane of triangle
    P = glm::cross(dir, e2);
    det = glm::dot(e1, P);
    if(det > -eps && det < eps) return false;
    inv_det = 1 / det;
    
    
    T = eye - v1;
    u = glm::dot(T, P) * inv_det;
    if(u < eps || u > 1+eps) return false;
    
    Q = glm::cross(T, e1);
    v = glm::dot(dir, Q) * inv_det;
    if(v<eps || u+v > 1+eps) return false;
    
    t = glm::dot(e2, Q)*inv_det;
    
    if((t > min+eps) && (t < max-eps)){
        n = glm::cross(e1, e2);
        if(glm::dot(n, dir) > +eps){
            n = -n;
        }
        lt = t;
        return true;
    }
    
    
    return retBool;
}

// TODO: hit functions for all primaries
// updated t and material if got intersection with less but greater than one t
// TODO: what if the pixel is inside some primitives? -- should be black?
bool hit(glm::vec3 eye, glm::vec3 pixel, GeometryNode node, PhongMaterial **mat, double &t, glm::vec3 &hitNormal, glm::mat4 invM, double min, double max){
    bool retBool = false;
    PrimType primitiveType = node.m_primitive->m_type;
    double lt = infd;
    
    // primary ray
    glm::vec3 dir = pixel - eye;
    
    // TODO: superimpose model to world, to transform ray and eye into MCS
    glm::mat4 w2m = node.get_transform();
    if(node.get_transform() != glm::mat4( 1.0f)){
        glm::mat4 w2m_inv = node.get_inverse();
        
        eye = glm::vec3(w2m_inv * glm::vec4(eye, 1.0f));
        pixel = glm::vec3(w2m_inv * glm::vec4(pixel, 1.0f));
        
        dir = pixel - eye;
    }
    
    if (primitiveType == PrimType::Sphere) {
        primitiveType = PrimType::NonhierSphere;
        node.m_primitive = &(static_cast<Sphere*>(node.m_primitive))->realSphere;
    }
    
    if (primitiveType == PrimType::Cube){
        primitiveType = PrimType::NonhierBox;
        node.m_primitive = &(static_cast<Cube*>(node.m_primitive))->realCube;
    }
    
    // primitiveType is NonhierSphere
    if (primitiveType == PrimType::NonhierSphere) {
        bool nhsBool = false;
        NonhierSphere * primPtr = static_cast<NonhierSphere*>(node.m_primitive);
        float A = glm::dot(dir, dir);
        float B = glm::dot(eye - primPtr->m_pos, dir);
        B = 2*B;
        float C = dot(eye - primPtr->m_pos, eye - primPtr->m_pos) - pow(primPtr->m_radius, 2);
        
        float determ = pow(B, 2) - 4*A*C;
        
        if(determ < 0 - eps){
            // no root
        } else {            if(determ < 0 + eps){
                // one root
                lt = -B/(2*A);
            } else {
                // two roots
                lt = -B - sqrt(determ);
                lt = lt / (2*A);
            }
            if(lt >= min + eps && lt < max - eps){
                if(lt < t){
                    nhsBool = true;
                    // closest one
                    *mat = static_cast<PhongMaterial*>(node.m_material);
                    t = lt;
                    hitNormal = (eye + (dir)*t - primPtr->m_pos);
                }
            }
        }
        retBool = retBool || nhsBool;
    }
    
    // primitiveType is NonhierBox
    if (primitiveType == PrimType::NonhierBox) {
        bool nhbBool = false;
        NonhierBox * primPtr = static_cast<NonhierBox*>(node.m_primitive);
        
        // Assumes no transformation
        //assert(node.get_transform() == glm::mat4(1.0f));
        
        glm::vec3 b0 = primPtr->m_pos;
        double size = primPtr->m_size;
        glm::vec3 b1 = glm::vec3(b0.x + size, b0.y + size, b0.z + size);
        
        glm::vec3 box_normal;
        nhbBool = hitBoundingBox(b0, b1, eye, dir, lt, min, max, box_normal);
        
        retBool = retBool || nhbBool;
        if(nhbBool){
            if(lt < t){
                // closest one
                *mat = static_cast<PhongMaterial*>(node.m_material);
                t = lt;
                hitNormal = box_normal;
            }
        }
    }
    
    // primitiveType is Mesh
    if (primitiveType == PrimType::Mesh) {
        bool mBool = false;
        double tMesh = infd;
        glm::vec3 normalTriangle;
        Mesh * primPtr = static_cast<Mesh*>(node.m_primitive);
        
        //std::cout << primPtr->m_faces.size() << std::endl;
        
        for(auto it = primPtr->m_faces.begin(); it != primPtr->m_faces.end(); it++){
            // TODO: after considering scaling, no need to multiply by 100
            glm::vec3 v1, v2, v3, n;
            v1 = primPtr->m_vertices[(*it).v1];
            v2 = primPtr->m_vertices[(*it).v2];
            v3 = primPtr->m_vertices[(*it).v3];
            if(hitTriangle(v1, v2, v3, eye, dir, lt, min, max, n)){
                mBool = true;
                if(lt < tMesh){
                    tMesh = lt;
                    normalTriangle = n;
                }
            }
        }
        
        retBool = retBool || mBool;
        if(mBool){
            if(tMesh < t){
                t = tMesh;
                *mat = static_cast<PhongMaterial*>(node.m_material);
                hitNormal = normalTriangle;
            }
        }
    }
    
    hitNormal = glm::vec3(glm::inverse(invM)*glm::vec4(hitNormal, 0.0f));
    
    if(retBool){
        assert(t>min+eps);
        assert(t<max-eps);
    }
    // ignores other kinds of primitives for now
    return retBool;
}

// recursive wrapper of hit function
bool hitWrapper(SceneNode* root, glm::vec3 eye, glm::vec3 pixel, PhongMaterial** mat, double &t, glm::vec3 &hitNormal, glm::mat4 invM, double min, double max){
    bool retBool = false;
    
    if(root->m_nodeType == NodeType::GeometryNode){
        GeometryNode* gnodep = static_cast<GeometryNode*>(root);
        retBool = hit(eye, pixel, *gnodep, mat, t, hitNormal, invM, min, max) || retBool;
    } else {
        glm::mat4 w2m_inv = root->get_inverse();
        invM = w2m_inv * invM;
        //invM = invM * w2m_inv;
        
        eye = glm::vec3(w2m_inv * glm::vec4(eye, 1.0f));
        pixel = glm::vec3(w2m_inv * glm::vec4(pixel, 1.0f));
        for(SceneNode* child : root->children){
            retBool = hitWrapper(child, eye, pixel, mat, t, hitNormal, invM, min, max) || retBool;
        }
    }
    return retBool;
}

// direct light function for diffused object
glm::vec3 directLight(glm::vec3 mkd, glm::vec3 hitPoint, glm::vec3 hitNormal, glm::vec3 lp, glm::vec3 lc) {
    glm::vec3 col = glm::vec3(0.0f);
    // TODO: consider light falloff based on distance
    //double r = glm::dot(lp - hitPoint, lp - hitPoint);
    glm::vec3 L = glm::normalize((lp - hitPoint));
    
    hitNormal = glm::normalize(hitNormal);
    col = mkd * std::max(glm::dot(L, hitNormal), 0.0f) * lc;
    return col;
}

// indirect light for specular
glm::vec3 indirectLight(glm::vec3 mks, glm::vec3 hitPoint, glm::vec3 hitNormal, glm::vec3 lp, glm::vec3 lc, glm::vec3 eye, double shininess){
    glm::vec3 col = glm::vec3(0.0f);
    glm::vec3 V = glm::normalize(eye - hitPoint);
    glm::vec3 R = glm::normalize(glm::reflect(- lp + hitPoint, hitNormal));
    
    col = mks * pow(std::max(glm::dot(V, R), 0.0f), shininess) * lc;
    return col;
}

void printColor(int x, int y, int r, int g, int b) {
#ifdef DEBUG_Z
    std::cout << "(" << x << ", " << y << ") <-- (" << r << ", " << g << ", " << b << ")" << std::endl;
#endif
}

void A4_Render(
               // What to render
               SceneNode * root,
               
               // Image to write to, set to a given width and height
               Image & image,
               
               // Viewing parameters
               const glm::vec3 & eye,
               const glm::vec3 & view,
               const glm::vec3 & up,
               double fovy,
               
               // Lighting parameters
               const glm::vec3 & ambient,
               const std::list<Light *> & lights
               ) {
    
    int h = (int)image.height();
    std::cout << h << std::endl;
    int w = (int)image.width();
    
    // Fill in raytracing code here...
    // Assume one pixel is one unit of length
    double d = h/(2*tan(glm::radians(fovy/2)));
    double fovx = 2.0 * glm::atan((double)w/2, d);
    glm::vec3 left = glm::normalize(glm::cross(up, view - eye));

    for (int y = 0; y<h; ++y) {
        
        for (int x = 0; x < w; ++x) {
            #ifdef RELEASE
            // clear screen
            std::cout << "\033[2J";
            
            // print percentage on (1,1)
            std::cout << "\033\033[" << 1 << ";" << 1 << "H" << (y*w + x)*100/(h*w) << "%" << std::endl;
            #endif
            
            // Assume one pixel is width 1 unit, height 1 unit
            // Assume eye 800, lookAt -800, x, y 都一样, d > 0
            // TODO: maybe need to do superimpose to get more general mapping to WCS
            glm::vec3 pointOnImage = eye + d*glm::normalize((view - eye)) + (h/2 - y)*glm::normalize(up) + (w/2 - x)*glm::normalize(left);
            
            //glm::vec3 pointOnImage = glm::vec3(eye.x - w/2 + x, eye.y + h/2 - y, eye.z - d);
            //glm::vec3 pointOnImage = eye + glm::normalize(primaryRay);
            //glm::vec4 primaryRay = glm::vec4(pointOnImage - eye, 0.0f);
            
            int lightNum = (int)lights.size();
            for (auto it = lights.begin(); it != lights.end(); it++) {
                // TODO: merge multiple light sources effects
                // Right now, the first non-black wins
                glm::vec3 col = rayColor(eye, pointOnImage, **it, lightNum, root, ambient);
                //printColor(x, y, col.x, col.y, col.z);
                image(x,y,0) += col.x;
                image(x,y,1) += col.y;
                image(x,y,2) += col.z;
            }
            image(x,y,0) = std::max(0.0, std::min(1.0, image(x,y,0)));
            image(x,y,1) = std::max(0.0, std::min(1.0, image(x,y,1)));
            image(x,y,2) = std::max(0.0, std::min(1.0, image(x,y,2)));
        }
    }
    
#ifdef RELEASE
    std::cout << "\033\033[" << 1 << ";" << 1 << "H100%" << std::endl;
#endif
    std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
    "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
    "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
    "\t" << "d: " << d << std::endl <<
    "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;
    
    for(const Light * light : lights) {
        std::cout << "\t\t" <<  *light << std::endl;
    }
    std::cout << "\t}" << std::endl;
    std::cout <<")" << std::endl;
    
    // background
    for (uint y = 0; y < h; ++y) {
        for (uint x = 0; x < w; ++x) {
            //if((x+y)%(51)==0 && (y-x)%(17)==0){
                if (image(x, y, 0)==0 && image(x,y,1)==0 && image(x,y,2)==0) {
                    // Red: increasing from top to bottom
                    image(x, y, 0) = (double)(y%100) / 170;
                    // Green: increasing from left to right
                    image(x, y, 1) = (double)(x%40) / 100;
                    // Blue: in lower-left and upper-right corners
                    image(x, y, 2) = (double)((x+y)%10)/70;
                }
            //}
        }
    }
    
    //glm::vec4 test4 = glm::vec4(1,2,3,4);
    //dout(glm::to_string(glm::vec3(test4)));
}
