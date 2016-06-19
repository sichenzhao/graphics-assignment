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

glm::vec3 rayColor(glm::vec3 eye, glm::vec3 p, Light light, std::set<GeometryNode*> nodes, const glm::vec3 & ambient);

glm::vec3 directLight(glm::vec3 mkd, glm::vec3 hitPoint, glm::vec3 lp, glm::vec3 lc);

void extractNodes(SceneNode* root, std::set<GeometryNode*> &nodesList);

bool hit(glm::vec3 eye, glm::vec3 pixel, GeometryNode node, PhongMaterial** mat, double &t, double min = 1, double max = std::numeric_limits<double>::infinity());
