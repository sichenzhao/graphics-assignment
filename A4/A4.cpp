#include <glm/ext.hpp>
#include <algorithm>

#include "A4.hpp"

#include <thread>

#include "GeometryNode.hpp"
#include "Mesh.hpp"

static bool showDebug = false;

#define DEBUG_Z
#ifdef DEBUG_Z
void dout(std::string msg){
    if(showDebug){
        std::cout << msg << std::endl;
    }
}
#else
#define RELEASE
void dout(std::string msg){}
#endif

#define MAX_BOUNCE 5

glm::vec3 rayColor(glm::vec3 eye, glm::vec3 pixelPoint, Light light, int lightNum, SceneNode* root, const glm::vec3 & ambient, const int maxBounce){
    dout("==============ray start=================");
    dout("maxbounce is " + std::to_string(maxBounce));
    if (maxBounce<=0) {
        return glm::vec3(0.0);
    }
    glm::vec3 col = glm::vec3(0.0);
    glm::vec3 hitNormal = glm::vec3(0.0);
    double t = infd;
    PhongMaterial* mat = NULL;
    std::shared_ptr<IntersecInfo> hitInfo = hitWrapper(root, eye, pixelPoint, &mat, t, hitNormal, glm::mat4(1.0));
    
    if(hitInfo == NULL){
        // return background
        dout("==============ray end=================");
        return col;
    } else {
        // hit
        
        // ambient light
        col.r = hitInfo->mat->m_kd.r*(ambient.r)/lightNum;
        col.g = hitInfo->mat->m_kd.g*(ambient.g)/lightNum;
        col.b = hitInfo->mat->m_kd.b*(ambient.b)/lightNum;
        
        // determine shadow
        bool isShadow = false;
        
        // TODO: do specular recursively, do light rendering like it is real
        // shadow of others
        {
            PhongMaterial* tmp = NULL;
            double tmpt = infd;
            glm::vec3 tmpNormal = glm::vec3(0.0);
            std::shared_ptr<IntersecInfo>tmpPtr = hitWrapper(root, glm::vec3(hitInfo->hitPoint), light.position, &tmp, tmpt, tmpNormal,  glm::mat4(1.0), 0+eps, 1-eps);
            isShadow = (tmpPtr==NULL)? false : tmpPtr->isIntersect;
        }
        
        if (!isShadow) {
            // calculate light falloff effects
            double r = glm::length(light.position - glm::vec3(hitInfo->hitPoint));
            double falloff = 1/(light.falloff[0] + light.falloff[1]*r + light.falloff[2]*r*r);
            
            // diffuse
            // direct light
            col += directLight(hitInfo->mat->m_kd, glm::vec3(hitInfo->hitPoint), glm::vec3(hitInfo->normal), light.position, light.colour * falloff);
            
            // specular
            //col += indirectLight(hitInfo->mat->m_ks, glm::vec3(hitInfo->hitPoint), glm::vec3(hitInfo->normal), light.position, light.colour * falloff, eye, hitInfo->mat->m_shininess);
        }
        
        // recursive mirror specular
        // Since mirror reflection, shininess is not working for V and R are the same
        
        if (hitInfo->mat->m_ks == glm::vec3(0.9)) {
            dout("mirror normal " + glm::to_string(glm::normalize(glm::vec3(hitInfo->normal))));
            dout("mirror hit point " + glm::to_string(hitInfo->hitPoint));
            dout("primary ray " + glm::to_string(glm::vec3(hitInfo->hitPoint) - eye));
            dout("reflected ray " + glm::to_string(glm::reflect(glm::vec3(hitInfo->hitPoint) - eye, glm::normalize(glm::vec3(hitInfo->normal)))));
            dout("Gonna recursively call rayColor");
        }
        
        glm::vec3 reflectedColor = hitInfo->mat->m_ks * rayColor(glm::vec3(hitInfo->hitPoint), glm::reflect(glm::vec3(hitInfo->hitPoint) - eye, glm::normalize(glm::vec3(hitInfo->normal))) + glm::vec3(hitInfo->hitPoint), light, lightNum, root, ambient, maxBounce-1);
        
        if (reflectedColor==glm::vec3(0.0)){
            dout("returned black");
        } else {
            dout(glm::to_string(reflectedColor) + "!!!!!!!");
        }
        
        col += reflectedColor;
    }
    dout("==============ray end=================");
    return col;
}


std::shared_ptr<IntersecInfo> hit(glm::vec3 eye, glm::vec3 pixel, GeometryNode node, glm::mat4 invM, double min, double max){
    glm::vec4 eye4 = glm::vec4(eye, 1.0);
    // primary ray
    glm::vec4 ray = glm::vec4(pixel - eye, 0.0);
    
    std::shared_ptr<IntersecInfo> retInfo = NULL;
    
    // just call the same virtual function
    retInfo = node.m_primitive->intersect(eye4, ray, min, max);
    
    if(retInfo!=NULL){
        if(retInfo->t < min+eps){
            std::cout << retInfo->t << std::endl;
        }
        assert(retInfo->t>min+eps);
        assert(retInfo->t<max-eps);
        retInfo->mat = static_cast<PhongMaterial*>(node.m_material);
        retInfo->normal = glm::transpose(node.get_inverse()) * retInfo->normal;
        retInfo->hitPoint = node.get_transform() * retInfo->hitPoint;
    }
    return retInfo;
}

// recursive wrapper of hit function
std::shared_ptr<IntersecInfo>  hitWrapper(SceneNode* root, glm::vec3 eye, glm::vec3 pixel, PhongMaterial** mat, double &t, glm::vec3 &hitNormal, glm::mat4 invM, double min, double max){
    std::shared_ptr<IntersecInfo> retInfo = NULL;
    std::shared_ptr<IntersecInfo> tmpInfo = NULL;
    
    glm::mat4 w2m_inv = root->get_inverse();
    
    eye = glm::vec3(w2m_inv * glm::vec4(eye, 1.0));
    pixel = glm::vec3(w2m_inv * glm::vec4(pixel, 1.0));
    
    if(root->m_nodeType == NodeType::GeometryNode){
        GeometryNode* gnodep = static_cast<GeometryNode*>(root);
        return hit(eye, pixel, *gnodep, invM, min, max);
    } else {
        // Scene or Joint nodes
        for(SceneNode* child : root->children){
            tmpInfo = hitWrapper(child, eye, pixel, mat, t, hitNormal, invM, min, max);
            if(tmpInfo != NULL) {
                if(retInfo == NULL){
                    retInfo = tmpInfo;
                } else {
                    if(retInfo->t > tmpInfo->t){
                        retInfo = tmpInfo;
                    }
                }
            }
        }
    }
    
    if(retInfo != NULL){
        // https://goo.gl/2P84tL link to why use tranpose(inverse) here
        retInfo->normal = glm::transpose(root->get_inverse()) * retInfo->normal;
        retInfo->hitPoint = root->get_transform() * retInfo->hitPoint;
    }
    return retInfo;
}

// direct light function for diffused object
glm::vec3 directLight(glm::vec3 mkd, glm::vec3 hitPoint, glm::vec3 hitNormal, glm::vec3 lp, glm::vec3 lc) {
    glm::vec3 col = glm::vec3(0.0);
    glm::vec3 L = glm::normalize((lp - hitPoint));
    
    hitNormal = glm::normalize(hitNormal);
    col = mkd * std::max((double)glm::dot(L, hitNormal), 0.0) * lc;
    return col;
}

// indirect light for specular
glm::vec3 indirectLight(glm::vec3 mks, glm::vec3 hitPoint, glm::vec3 hitNormal, glm::vec3 lp, glm::vec3 lc, glm::vec3 eye, double shininess){
    glm::vec3 col = glm::vec3(0.0);
    glm::vec3 V = glm::normalize(eye - hitPoint);
    glm::vec3 R = glm::normalize(glm::reflect(- lp + hitPoint, hitNormal));
    
    col = mks * pow(std::max((double)glm::dot(V, R), 0.0), shininess) * lc;
    return col;
}

void printColor(int x, int y, int r, int g, int b) {
#ifdef DEBUG_Z
    std::cout << "(" << x << ", " << y << ") <-- (" << r << ", " << g << ", " << b << ")" << std::endl;
#endif
}

void render(
            const int ye,
            const int xe,
            const int ys,
            const int xs,
            const int h,
            const int w,
            glm::vec3 eye,
            glm::vec3 left,
            glm::vec3 up,
            glm::vec3 view,
            double d,
            
            // What to render
            SceneNode * root,
            
            // Image to write to, set to a given width and height
            Image & image,
            
            // Lighting parameters
            const std::list<Light *> & lights,
            const glm::vec3 & ambient){
    
    for (int y = ys; y<ye; ++y) {
        
        for (int x = xs; x < xe; ++x) {
            showDebug = ((x==w/2) && (y==h/2));
#ifdef RELEASE
            // clear screen
            //std::cout << "\033[2J";
            
            // print percentage on (1,1)
            //std::cout << "\033\033[" << 1 << ";" << 1 << "H" << (y*w + x)*100/(h*w) << "%" << std::endl;
#endif
            
            // TODO: change signature of rayColor from pointOnImage to primaryRay
            glm::vec3 pointOnImage = eye + d*glm::normalize((view - eye)) + (h/2 - y)*glm::normalize(up) + (w/2 - x)*glm::normalize(left);
            
            int lightNum = (int)lights.size();
            for (auto it = lights.begin(); it != lights.end(); it++) {
                glm::vec3 col = rayColor(eye, pointOnImage, **it, lightNum, root, ambient, MAX_BOUNCE);
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
    int w = (int)image.width();
    
    // Fill in raytracing code here...
    // Assume one pixel is one unit of length
    double d = h/(2*tan(glm::radians(fovy/2)));
    //double fovx = 2.0 * glm::atan((double)w/2, d);
    glm::vec3 left = glm::normalize(glm::cross(up, view - eye));
    
#define threadNum 1
    std::thread* threads[threadNum];
    
    for(int i=0; i<threadNum; i++){
        threads[i] = new std::thread(render, h*(i+1)/threadNum, w, i*h/threadNum, 0, h, w, eye, left, up, view, d, root, std::ref(image), std::ref(lights), ambient);
    }
    
    for (int i=0; i<threadNum; i++) {
        threads[i]->join();
    }


    
#ifdef RELEASE
    //std::cout << "\033\033[" << 1 << ";" << 1 << "H100%" << std::endl;
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
