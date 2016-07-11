//
//  Grid.cpp
//  CS488-Projects
//
//  Created by SIchenZhao on 2016-06-25.
//  Copyright (c) 2016 none. All rights reserved.
//

#include "Grid.h"

using namespace std;

Grid::Grid(){}

Grid::Grid(BoundingVolume *bvp, const std::vector<Triangle> *m_facesp){
    this->bvp = bvp;
    this->m_facesp = m_facesp;
    
    setupCells();
}

Grid::~Grid(){}


void Grid::setupCells(){
    // float xmin, ymin, zmin, xmax, ymax, zmax;
    // xmin = bvp->xmin;
    
    // determine grid numbers on each axis
    // nx, ny, nz
    float wx = bvp->xmax - bvp->xmin;
    float wy = bvp->ymax - bvp->ymin;
    float wz = bvp->zmax - bvp->zmin;
    //nx = ny = nz = 10;
    nx = ny = nz = 3;
    /**
     unsigned long num_objects = m_facesp->size();
     float multiplier = 2.0f;
     float s = pow(wx * wy * wz / num_objects, 0.3333333);
     nx = multiplier * wx / s + 1;
     ny = multiplier * wy / s + 1;
     nz = multiplier * wz / s + 1;
     **/
    
    cx = wx / nx;
    cy = wy / ny;
    cz = wz / nz;
    
    // reserve enough space for cells
    cells.reserve(nx * ny * nz);
    for (int i = 0; i < nx*ny*nz; i++) {
        vector<Triangle*> tmp;
        cells.push_back(tmp);
    }
    
    // put objects into cells
    for (auto it = m_facesp->begin(); it != m_facesp->end(); it++) {
        addObj((Triangle*)&(*it));
    }
}

// Add a triangle to cells
void Grid::addObj(Triangle* triangle){
    glm::vec3 v1, v2, v3;
    v1 = (*(triangle->m_vertices))[triangle->iv1];
    v2 = (*(triangle->m_vertices))[triangle->iv2];
    v3 = (*(triangle->m_vertices))[triangle->iv3];
    
    float xmin, xmax, ymin, ymax, zmin, zmax;
    xmin = min(min(v1.x, v2.x), v3.x);
    ymin = min(min(v1.y, v2.y), v3.y);
    zmin = min(min(v1.z, v2.z), v3.z);
    
    xmax = max(max(v1.x, v2.x), v3.x);
    ymax = max(max(v1.y, v2.y), v3.y);
    zmax = max(max(v1.z, v2.z), v3.z);
    
    // calculate which two cells are from min and max
    int nx1, ny1, nz1, nx2, ny2, nz2;
    nx1 = (xmin - bvp->xmin) / cx;
    ny1 = (ymin - bvp->ymin) / cy;
    nz1 = (zmin - bvp->zmin) / cz;
    
    nx2 = (xmax - bvp->xmin) / cx;
    ny2 = (ymax - bvp->ymin) / cy;
    nz2 = (zmax - bvp->zmin) / cz;
    
    nx1 = (nx1 == nx) ? nx1 - 1:nx1;
    ny1 = (ny1 == ny) ? ny1 - 1:ny1;
    nz1 = (nz1 == nz) ? nz1 - 1:nz1;
    
    nx2 = (nx2 == nx) ? nx2 - 1:nx2;
    ny2 = (ny2 == ny) ? ny2 - 1:ny2;
    nz2 = (nz2 == nz) ? nz2 - 1:nz2;
    
    assert(nx1<nx && nx1<=nx2);
    assert(ny1<ny && ny1<=ny2);
    assert(nz1<nz && nz1<=nz2);
    assert(nx2<nx);
    assert(ny2<ny);
    assert(nz2<nz);
    
    // add triangle to all bounding volumes
    for (int x = nx1; x<=nx2; x++) {
        for (int y = ny1; y<=ny2; y++) {
            for (int z = nz1; z<=nz2; z++) {
                cells[x + nx*y + nx*ny*z].push_back(triangle);
            }
        }
    }
}


std::shared_ptr<IntersecInfo> Grid::intersect(glm::vec4 p, glm::vec4 ray, const double firstT, const double secondT, const double min, const double max){
    shared_ptr<IntersecInfo> retInfo = NULL;
    glm::vec3 normalTriangle;
    
    // every axis corresponding delta t
    float dtx, dty, dtz;
    dtx = cx / ray.x;
    dty = cy / ray.y;
    dtz = cz / ray.z;
    
    // find the start cell of ray bbox intersection
    glm::vec4 startP = p + ray * (float) firstT;
    int ix, iy, iz;
    ix = (startP.x - bvp->xmin) / cx;
    iy = (startP.y - bvp->ymin) / cy;
    iz = (startP.z - bvp->zmin) / cz;
    
    if (ix == nx) {
        ix--;
    }
    if (iy == ny) {
        iy--;
    }
    if (iz == nz) {
        iz--;
    }
    
    // find the initial t for each coordinates
    float tx_next, ty_next, tz_next;
    tx_next = (bvp->xmin - p.x) / ray.x;
    ty_next = (bvp->ymin - p.y) / ray.y;
    tz_next = (bvp->zmin - p.z) / ray.z;
    while ((tx_next<firstT) || (ty_next<firstT) || (tz_next<firstT)) {
        if (tx_next<firstT) {
            tx_next += dtx;
        }
        if (ty_next<firstT) {
            ty_next += dty;
        }
        if (tz_next<firstT){
            tz_next += dtz;
        }
    }
    
    while (true) {
        for (auto it=cells[ix + iy*nx + iz*ny*nx].begin(); it!=cells[ix + iy*nx + iz*ny*nx].end(); it++) {
            shared_ptr<IntersecInfo> tmpInfo = NULL;
            tmpInfo = (*it)->intersect(p, ray, min, max);
            if (tmpInfo != NULL) {
                if (retInfo != NULL && tmpInfo->t < retInfo->t) {
                    retInfo = tmpInfo;
                } else if (retInfo == NULL) {
                    retInfo = tmpInfo;
                }
            }
        }
        
        if (retInfo != NULL) {
            return retInfo;
        }
        
        if (tx_next < ty_next && tx_next < tz_next) {
            // xmin
            assert(ix<nx);
            tx_next += dtx;
            ix += (ray.x > 0)?1:-1;
            if(ix<0 || ix >= nx) break;
        } else if (ty_next < tz_next) {
            // ymin
            assert(iy<ny);
            ty_next += dty;
            iy += (ray.y > 0)?1:-1;
            if(iy<0 || iy >= ny) break;
        } else {
            // zmin
            assert(iz<nz);
            tz_next += dtz;
            iz += (ray.z > 0)?1:-1;
            if(iz<0 || iz >= nz) break;
        }
    }
    
    if (retInfo != NULL) {
        assert(retInfo->t<max-eps && retInfo->t>min+eps);
    }
    
    return retInfo;
}
