#include <glm/ext.hpp>
#include <algorithm>

#include "A4.hpp"

#include <thread>

#include "GeometryNode.hpp"
#include "Mesh.hpp"

static bool showDebug = false;

//#define DEBUG_Z
#ifdef DEBUG_Z
#define threadNum 1
void dout(std::string msg){
    if(showDebug){
        std::cout << msg << std::endl;
    }
}

void dout(std::string msg, bool show){
    if(show){
        std::cout << msg << std::endl;
    }
}
#else
#define threadNum 20
#define RELEASE
void dout(std::string msg){}

void dout(std::string msg, bool show){}
#endif

#define MAX_BOUNCE 5

glm::vec3 rayColor(glm::vec3 eye, glm::vec3 pixelPoint, Light light, int lightNum, SceneNode* root, const glm::vec3 & ambient, const int maxBounce){
    dout("==============ray start=================");
    dout("maxbounce is " + std::to_string(maxBounce));
    if (maxBounce<=0 || isnan(pixelPoint.x)) {
        return glm::vec3(0.0);
    }
    glm::vec3 col = glm::vec3(-1.0);
    if (maxBounce != MAX_BOUNCE) {
        col = glm::vec3(0.0);
    }
    glm::vec3 hitNormal = glm::vec3(0.0);
    double t = infd;
    PhongMaterial* mat = NULL;
    std::shared_ptr<IntersecInfo> hitInfo = hitWrapper(root, eye, pixelPoint, &mat, t, hitNormal, glm::mat4(1.0));
    
    if(hitInfo == NULL){
        // return background
        return col;
    } else {
        // hit
        
        // ambient light
        col.r = hitInfo->mat->get_m_kd(hitInfo->u, hitInfo->v).r*(ambient.r)/lightNum;
        col.g = hitInfo->mat->get_m_kd(hitInfo->u, hitInfo->v).g*(ambient.g)/lightNum;
        col.b = hitInfo->mat->get_m_kd(hitInfo->u, hitInfo->v).b*(ambient.b)/lightNum;
        
        // determine shadow
        bool isShadow = false;
        
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
            col += directLight(hitInfo->mat->get_m_kd(hitInfo->u, hitInfo->v), glm::vec3(hitInfo->hitPoint), glm::vec3(hitInfo->normal), light.position, light.colour * falloff);
            
            // specular
            if ((!hitInfo->mat->isMirror) && (!hitInfo->mat->isTrans)) {
                col += indirectLight(hitInfo->mat->m_ks, glm::vec3(hitInfo->hitPoint), glm::vec3(hitInfo->normal), light.position, light.colour * falloff, eye, hitInfo->mat->m_shininess);
            }
        }
        
        // recursive mirror specular
        // Since mirror reflection, shininess is not working for V and R are the same
        glm::vec3 r_primaryRay = glm::normalize(pixelPoint - eye);
        glm::vec3 r_normalVec = glm::normalize(glm::vec3(hitInfo->normal));
        
        glm::vec3 reflectedRay = glm::reflect(r_primaryRay, r_normalVec);
        
        glm::vec3 reflectedColor = glm::vec3(0.0);
        glm::vec3 refractedColor = glm::vec3(0.0);
        
        /**
        dout("reflected normal " + glm::to_string(glm::normalize(glm::vec3(hitInfo->normal))));
        dout("returned hit point " + glm::to_string(glm::vec3(hitInfo->hitPoint)));
        dout("reflected hit point " + glm::to_string(glm::vec3(hitInfo->hitPoint) + 10*eps * reflectedRay));
        dout("reflected ray " + glm::to_string(2*reflectedRay));
        dout("reflected point " + glm::to_string(2*reflectedRay + glm::vec3(hitInfo->hitPoint)));
         **/
#ifdef MR
        // to avoid reflected ray hit the object again, use (8*eps*reflectedray + hitpoint) as a new start point
        reflectedColor = hitInfo->mat->m_ks * rayColor(glm::vec3(hitInfo->hitPoint) + 8*eps * reflectedRay, 2*reflectedRay + glm::vec3(hitInfo->hitPoint) + 8*eps * reflectedRay, light, lightNum, root, ambient, maxBounce-1);
        
        //if (reflectedColor != glm::vec3(0.0)) {
        //dout("reflectedColor is " + glm::to_string(reflectedColor));
        //}

        //float reflectedPortion = 0.5;
        float reflectedPortion = 0;
        if (hitInfo->mat->isTrans) {
            
            // calculate refracted ray direction
            // TODO: now assume alwasy from vacuum to indices
            {
                float eta = (hitInfo->hitIn) ? (1/hitInfo->mat->transInd) : (hitInfo->mat->transInd);
                float k = 1 - eta*eta*(1.0 - glm::dot(-r_primaryRay, r_normalVec)*glm::dot(-r_primaryRay, r_normalVec));
                
                if (k<0) {
                    // 全反射
                    reflectedPortion = 1;
                } else {
                    // calculate refracted color
                    glm::vec3 refractedRay = eta * r_primaryRay - eta * (dot(r_normalVec, r_primaryRay) + sqrt(k)) * r_normalVec;
                    
                    int errorAv = 10;
                    
                    dout("eye " + glm::to_string(eye));
                    dout("pixelPoint " + glm::to_string(pixelPoint));
                    dout("r_primary " + glm::to_string(r_primaryRay));
                    dout("refraction normal " + glm::to_string(r_normalVec));
                    dout("refraction hit point " + glm::to_string(glm::vec3(hitInfo->hitPoint) + errorAv * eps * refractedRay));
                    dout("refracted ray " + glm::to_string(refractedRay));
                    dout("Gonna recursively call rayColor");
                    
                    refractedColor = hitInfo->mat->m_ks * rayColor(glm::vec3(hitInfo->hitPoint) + errorAv * eps * refractedRay, refractedRay + glm::vec3(hitInfo->hitPoint), light, lightNum, root, ambient, maxBounce-1);
                }
            }
            
            // TODO: apply frenel's equation
            reflectedColor *= reflectedPortion;
            refractedColor *= 1 - reflectedPortion;
        }
#endif
        
        if (reflectedColor==glm::vec3(0.0)){
            dout("returned black");
        } else {
            dout(glm::to_string(reflectedColor) + "!!!!!!!");
        }
        
        reflectedColor = glm::clamp(reflectedColor, 0.0f, 1.0f);
        refractedColor = glm::clamp(refractedColor, 0.0f, 1.0f);
        
        col += reflectedColor;
        col += refractedColor;
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
            dout(glm::to_string(eye4), true);
            dout(glm::to_string(ray), true);
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

glm::vec3 getCol (glm::vec3 eye,
                  glm::vec3 pointOnImage,
                  glm::vec3 left,
                  glm::vec3 up,
                  Light* const* it,
                  int lightNum,
                  float rand,
                  float d,
                  float focalD,
                  
                  // What to render
                  SceneNode * root,
                  
                  const glm::vec3 & ambient) {
#ifdef DF
    glm::vec3 col = glm::vec3(0.0f);
    int eyeNum = 3;
    // TODO: for testing, define focal distance here
    // TODO: suppose camera disk radius is one
    float cameraR = 0.0010f;
    float t = focalD / d;
    for (int j = 0; j < eyeNum; j++) {
        glm::vec3 randomizedEye = eye + cameraR*rand*left + cameraR*rand*up;
        pointOnImage = eye + t * (pointOnImage - eye);
        col += (1/((float) eyeNum)) * rayColor(randomizedEye, pointOnImage,  **it, lightNum, root, ambient, MAX_BOUNCE);
    }
#else
    glm::vec3 col = rayColor(eye, pointOnImage, **it, lightNum, root, ambient, MAX_BOUNCE);
#endif
    return col;
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
            //showDebug = ((x<=w/2+10) && (y<=h/2+10)) && ((x>=w/2-10) && (y>=h/2-10));
            showDebug = ((x==w/2) && (y==h/2));
            dout(std::to_string(x) + ", " + std::to_string(y));
#ifdef RELEASE
            // clear screen
            //std::cout << "\033[2J";
            
            // print percentage on (1,1)
            //std::cout << "\033\033[" << 1 << ";" << 1 << "H" << (y*w + x)*100/(h*w) << "%" << std::endl;
#endif
            
            
            float rand = static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX);
            rand = rand * 2 + -1;
      
            up = glm::normalize(up);
            left = glm::normalize(left);
            
            float focalD = sqrt(glm::dot(view - eye, view - eye));
#ifdef AA
            for (int i = 0; i < 4; i++) {
                glm::vec3 pointOnImage = eye + d*glm::normalize((view - eye)) + (h/2 - y)*glm::normalize(up) + (w/2 - x)*left;
                pointOnImage -= ((float)(i%2)) * left;
                pointOnImage -= ((float)(i/2)) * up;
                
                // rand from -1 to 1
                pointOnImage += 0.5f * rand * left;
                pointOnImage += 0.5f * rand * up;
                
                int lightNum = (int)lights.size();
                for (auto it = lights.begin(); it != lights.end(); it++) {
                    glm::vec3 col = getCol(eye, pointOnImage, left, up, &(*it), lightNum, rand, d, focalD, root, ambient);
                    col = 0.25f * col;
                    //printColor(x, y, col.x, col.y, col.z);
                    image(x,y,0) += col.x;
                    image(x,y,1) += col.y;
                    image(x,y,2) += col.z;
                }
            }
#else
            // TODO: change signature of rayColor from pointOnImage to primaryRay
            glm::vec3 pointOnImage = eye + d*glm::normalize((view - eye)) + (h/2 - y)*glm::normalize(up) + (w/2 - x)*glm::normalize(left);
            
            int lightNum = (int)lights.size();
            for (auto it = lights.begin(); it != lights.end(); it++) {
                glm::vec3 col = getCol(eye, pointOnImage, left, up, &(*it), lightNum, rand, d, focalD, root, ambient);
                //printColor(x, y, col.x, col.y, col.z);
                image(x,y,0) += col.x;
                image(x,y,1) += col.y;
                image(x,y,2) += col.z;
            }
#endif
                
            image(x,y,0) = std::min(1.0, image(x,y,0));
            image(x,y,1) = std::min(1.0, image(x,y,1));
            image(x,y,2) = std::min(1.0, image(x,y,2));
        }
    }
    return;
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
    
    std::thread* threads[threadNum];
    
    if(threadNum > 1){
        for(int i=0; i<threadNum; i++){
            threads[i] = new std::thread(render, h*(i+1)/threadNum, w, i*h/threadNum, 0, h, w, eye, left, up, view, d, root, std::ref(image), std::ref(lights), ambient);
        }
        
        for (int i=0; i<threadNum; i++) {
            if (threads[i]->joinable()) {
                threads[i]->join();
            }
        }
    } else {
        // ye xe ys xs
        render(h*(0+1)/threadNum, w, 0*h/threadNum, 0, h, w, eye, left, up, view, d, root, std::ref(image), std::ref(lights), ambient);
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
            if (image(x, y, 0)<0 || image(x,y,1)<0 || image(x,y,2)<0) {
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
