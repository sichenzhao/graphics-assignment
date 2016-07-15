#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

    void setMaterial( Material *material );
    void setMaterial( Material *material, const std::string filename);

	Material *m_material;
	Primitive *m_primitive;
};
