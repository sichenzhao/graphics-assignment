#pragma once

#include <glm/glm.hpp>

#include "PhongMaterial.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Light.hpp"
#include "Image.hpp"

#include <set>

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
);

glm::vec3 getCol (glm::vec3 eye,
                  glm::vec3 pointOnImage,
                  glm::vec3 left,
                  glm::vec3 up,
                  Light * const* it,
                  int lightNum,
                  float rand,
                  float d,
                  
                  // What to render
                  SceneNode * root,
                  
                  const glm::vec3 & ambient);

void render(const int ye,
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
            const glm::vec3 & ambient);

glm::vec3 rayColor(glm::vec3 eye, glm::vec3 p, Light light, int lightNum, SceneNode* root, const glm::vec3 & ambient, const int maxBounce = 1);

glm::vec3 directLight(glm::vec3 mkd, glm::vec3 hitPoint, glm::vec3 hitNormal, glm::vec3 lp, glm::vec3 lc);

glm::vec3 indirectLight(glm::vec3 mks, glm::vec3 hitPoint, glm::vec3 hitNormal, glm::vec3 lp, glm::vec3 lc, glm::vec3 eye, double shininess);

std::shared_ptr<IntersecInfo>  hitWrapper(SceneNode* root, glm::vec3 eye, glm::vec3 pixel, PhongMaterial** mat, double &t, glm::vec3 &hitNormal, glm::mat4 invM, double min = 0, double max = std::numeric_limits<double>::infinity());

std::shared_ptr<IntersecInfo> hit(glm::vec3 eye, glm::vec3 pixel, GeometryNode node, glm::mat4 invM, double min = 0, double max = std::numeric_limits<double>::infinity());
