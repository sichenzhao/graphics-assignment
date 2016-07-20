#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
    , uv(0)
    , width(0)
    , height(0)
    , isTrans(false)
    , transInd(-1)
    , isMirror(false)
{}

PhongMaterial::~PhongMaterial()
{}

glm::vec3 PhongMaterial::get_m_kd(){
    return m_kd;
}

glm::vec3 PhongMaterial::get_m_kd(double x, double y){
    if (uv == NULL) {
        return m_kd;
    } else {
        return uv[(int)(x*width)][(int)(y*height)];
    }
}

void PhongMaterial::setTransparent(bool val, double index) {
    isTrans = val;
    transInd = index;
}

void PhongMaterial::setMirror(){
    isMirror = true;
}