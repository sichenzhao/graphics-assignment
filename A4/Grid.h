//
//  Grid.h
//  CS488-Projects
//
//  Created by SIchenZhao on 2016-06-25.
//  Copyright (c) 2016 none. All rights reserved.
//

#ifndef __CS488_Projects__Grid__
#define __CS488_Projects__Grid__

#include <stdio.h>
#include "Primitive.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"

#include <vector>
#include <tuple>

class Grid {
public:
    BoundingVolume *bvp;
    const std::vector<Triangle>* m_facesp;
    // Material* mat; no need to worry about material
    
    Grid();
    Grid(BoundingVolume *bvp, const std::vector<Triangle> *m_facesp);
    
    void setupCells();
    void addObj(Triangle* triangle);
    
    std::shared_ptr<IntersecInfo> intersect(glm::vec4 p, glm::vec4 ray, const double firstT, const double secondT, bool bvHitIn, const double min, const double max);
    
    // Number of cells
    int nx, ny, nz;
    
    // size of cells
    float cx, cy, cz;
    
    // now one grid per mesh, just need to held triangle to do hit
    //std::vector<std::tuple<Primitive*, Material*, glm::mat4> > objectsVec;
    std::vector<std::vector<Triangle*> > cells;
    
    virtual ~Grid();
};

#endif /* defined(__CS488_Projects__Grid__) */
