#include "GeometryNode.hpp"
#include "Image.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

// setMatrial for texture mapped sphere
void GeometryNode::setMaterial( Material *mat, const std::string filename )
{
    m_material = mat;
    if (m_primitive->m_type != PrimType::Sphere) {
        return;
    }
    
    Image image(0,0);
    image.loadPng(filename);
    
    PhongMaterial* p_material = static_cast<PhongMaterial*>(m_material);
    p_material->uv = image.m_uv;
    p_material->width = image.m_width;
    p_material->height = image.m_height;
    
    // Test loaded correctly
    /**
    for (uint y = 0; y < image.m_width; ++y) {
        for (uint x = 0; x < image.m_height; ++x) {
            // Red: increasing from top to bottom
            image(x, y, 0) = image.m_uv[x][y].r;
            // Green: increasing from left to right
            image(x, y, 1) = image.m_uv[x][y].g;
            // Blue: in lower-left and upper-right corners
            image(x, y, 2) = image.m_uv[x][y].b;
        }
    }
    image.savePng("test_loading.png");
     **/
}
