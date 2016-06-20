#include <glm/ext.hpp>

#include "A4.hpp"

#include "GeometryNode.hpp"

#define DEBUG_Z
#ifdef DEBUG_Z
void dout(std::string msg){
    std::cout << msg << std::endl;
}
#else
void dout(std::string msg){}
#endif

float inff = std::numeric_limits<float>::infinity();
double infd = std::numeric_limits<double>::infinity();
static float eps = FLT_EPSILON;

glm::vec3 rayColor(glm::vec3 eye, glm::vec3 pixelPoint, Light light, std::set<GeometryNode*> nodes, const glm::vec3 & ambient){
    glm::vec3 col = glm::vec3(0.0f);
    double t = infd;
    PhongMaterial* mat = NULL;
    GeometryNode* hitNode = NULL;
    for (auto it = nodes.begin(); it != nodes.end(); it++) {
        if(hit(eye, pixelPoint, **it, &mat, t)){
            hitNode = *it;
        }
    }
    if(infd==t || NULL==mat){
        // return background
        return col;
    } else {
        // hit
        
        // ambient light
        col.r = mat->m_shininess + mat->m_kd.r*(ambient.r);
        col.g = mat->m_shininess + mat->m_kd.g*(ambient.g);
        col.b = mat->m_shininess + mat->m_kd.b*(ambient.b);
        
        //diffuse
        if (mat->m_kd != glm::vec3(0.0f)) {
            glm::vec3 hitPoint = eye + t*(pixelPoint - eye);
            
            // determine shadow
            bool isShadow = false;
            
            // shadow for itself
            if(hitNode->m_primitive->m_type == PrimType::NonhierSphere){
                NonhierSphere * primPtr = static_cast<NonhierSphere*>(hitNode->m_primitive);
                isShadow = (glm::dot(hitPoint - primPtr->m_pos, light.position - hitPoint) < 0);
            }
            
            // shadow of others
            /**
            for(auto it = nodes.begin(); it != nodes.end(); it++) {
                if(isShadow) break;
                if(*it==hitNode) continue;
                PhongMaterial* tmp = NULL;
                double tmpt = infd;
                isShadow = isShadow || hit(hitPoint, light.position, **it, &tmp, tmpt, 0, 1);
            }
             **/
            
            // TODO: shadow is not working for now (everything is shadowed)
            if(isShadow){
                // shadow, no direct
                // std::dout << "shadow" << i << std::endl;
            }else {
                // direct light
                col = directLight(mat->m_kd, hitPoint, light.position, light.colour);
            }
        }
        
        // specular
        if (mat->m_ks != glm::vec3(0.0f)) {
            // TODO: do specular recursively, no need for simple image now
            return col;
        }
    }
    return col;
}

// TODO: hit functions for all primaries
// updated t and material if got intersection with less but greater than one t
// TODO: what if the pixel is inside some primitives? -- should be black?
bool hit(glm::vec3 eye, glm::vec3 pixel, GeometryNode node, PhongMaterial **mat, double &t, double min, double max){
    bool retBool = false;
    PrimType primitiveType = node.m_primitive->m_type;
    double lt;
    
    // primitiveType is NonhierSphere
    if (primitiveType == PrimType::NonhierSphere) {
        NonhierSphere * primPtr = static_cast<NonhierSphere*>(node.m_primitive);
        float A = glm::dot(pixel - eye, pixel - eye);
        float B = glm::dot(eye - primPtr->m_pos, pixel - eye);
        B = 2*B;
        float C = dot(eye - primPtr->m_pos, eye - primPtr->m_pos) - pow(primPtr->m_radius, 2);
        
        float determ = pow(B, 2) - 4*A*C;
        
        if(determ < 0 - eps){
            // no root
        } else {
            retBool = true;
            if(determ < 0 + eps){
                // one root
                lt = -B/(2*A);
            } else {
                // two roots
                lt = -B - sqrt(determ);
                lt = lt / (2*A);
            }
            if(lt >= min + eps && lt < max - eps){
                t = std::min(t, lt);
                if(t==lt){
                    // closest one
                    *mat = static_cast<PhongMaterial*>(node.m_material);
                }
            }
        }
    }
    
    // primitiveType is NonhierBox
    if (primitiveType == PrimType::NonhierBox) {
        dout("box");
        
        NonhierBox * primPtr = static_cast<NonhierBox*>(node.m_primitive);
        
        // Assumes no transformation
        assert(node.get_transform() == glm::mat4(1.0f));
        
        glm::vec3 b0 = primPtr->m_pos;
        double size = primPtr->m_size;
        glm::vec3 b1 = glm::vec3(b0.x + size, b0.y + size, b0.z + size);
        glm::vec3 dir = pixel - eye;
        
        double tmin, tmax, tymin, tymax, tzmin, tzmax;
        if (dir.x >= 0) {
            tmin = (b0.x - eye.x) / dir.x;
            tmax = (b1.x - eye.x) / dir.x;
        } else {
            tmin = (b1.x - eye.x) / dir.x;
            tmax = (b0.x - eye.x) / dir.x;
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

        if((lt >= min + eps) && (lt < max - eps)) {
            // hits nh_cube
            dout("hit cube");
            t = std::min(t, tmin);
            if(t==tmin){
                // closest one
                *mat = static_cast<PhongMaterial*>(node.m_material);
            }
        }
        
    }
    // ignores other kinds of primitives for now
    return retBool;
}

// direct light function for diffused object
glm::vec3 directLight(glm::vec3 mkd, glm::vec3 hitPoint, glm::vec3 lp, glm::vec3 lc) {
    glm::vec3 col = glm::vec3(0.0f);
    // TODO: consider light falloff based on distance
    //double r = glm::dot(lp - hitPoint, lp - hitPoint);
    col.r = lc.r * mkd.r;
    col.g = lc.g * mkd.g;
    col.b = lc.b * mkd.b;
    return col;
}

void extractNodes(SceneNode* root, std::set<GeometryNode*> &nodesList) {
    if (root->m_nodeType==NodeType::GeometryNode) {
        // add it
        nodesList.insert(static_cast<GeometryNode*>(root));
        //std::dout << "added" << std::endl;
    }
    
    for (SceneNode* node : root->children) {
        extractNodes(node, nodesList);
    }
    return;
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
    
    size_t h = image.height();
    size_t w = image.width();
    
    // Fill in raytracing code here...
    double d = h/(2*tan(glm::radians(fovy/2)));
    
    // Extract all GeometryNode into a list
    std::set<GeometryNode*> nodesList;
    extractNodes(root, nodesList);
    /**
    for (auto it = nodesList.begin(); it != nodesList.end(); it++) {
        std::dout << (*it)->m_name << std::endl;
    }
     **/
    
    for (uint y = 0; y<h; ++y) {
        for (uint x = 0; x < w; ++x) {
            // Assume one pixel is width 1 unit, height 1 unit
            // Assume eye 800, lookAt -800, x, y 都一样, d > 0
            // TODO: maybe need to do superimpose to get more general mapping to WCS
            glm::vec3 pointOnImage = glm::vec3(eye.x - w/2 + x, eye.y + h/2 - y, eye.z - d);
            //glm::vec4 primaryRay = glm::vec4(pointOnImage - eye, 0.0f);
            
            for (auto it = lights.begin(); it != lights.end(); it++) {
                if (image(x, y, 0)==0 && image(x,y,1)==0 && image(x,y,2)==0) {
                    // TODO: merge multiple light sources effects
                    // Right now, the first non-black wins
                    glm::vec3 col = rayColor(eye, pointOnImage, **it, nodesList, ambient);
                    image(x,y,0) = col.x;
                    image(x,y,1) = col.y;
                    image(x,y,2) = col.z;
                }
            }
            
        }
    }
    
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
    /**
    for (uint y = 0; y < h; ++y) {
        for (uint x = 0; x < w; ++x) {
            if (image(x, y, 0)==0 && image(x,y,1)==0 && image(x,y,2)==0) {
                // Red: increasing from top to bottom
                image(x, y, 0) = (double)y / h;
                // Green: increasing from left to right
                image(x, y, 1) = (double)x / w;
                // Blue: in lower-left and upper-right corners
                image(x, y, 2) = ((y < h/2 && x < w/2)
                                  || (y >= h/2 && x >= w/2)) ? 1.0 : 0.0;
            }
        }
    }
     **/
}
