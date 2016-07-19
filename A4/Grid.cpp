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
    float wx = abs(bvp->xmax - bvp->xmin);
    float wy = abs(bvp->ymax - bvp->ymin);
    float wz = abs(bvp->zmax - bvp->zmin);
    
    if (wx==0) {
        wx += eps;
    }
    if (wy==0) {
        wy += eps;
    }
    if (wz==0) {
        wz += eps;
    }
    
    if (m_facesp->size() > 1000 || wx * wy * wz==0) {
        nx = ny = nz = 10;
    } else {
        unsigned long num_objects = m_facesp->size();
        float multiplier = 2.0f;
        float s = pow(wx * wy * wz / num_objects, 0.3333333);
        nx = multiplier * wx / s + 1;
        ny = multiplier * wy / s + 1;
        nz = multiplier * wz / s + 1;
    }
    
    nx = ny = nz = 6;
    
    cx = max((wx/nx), (float)eps);
    cy = max(wy / ny, (float)eps);
    cz = max(wz / nz, (float)eps);
    
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

static void update_t(float &t, const float firstT, const float dt){
    if(t<=firstT + eps && t>=firstT - eps) {
        t += dt;
        return;
    }
    if (t < firstT) {
        while (t <= firstT) {
            t += dt;
        }
        return;
    } else {
        while (t > firstT) {
            t -= dt;
        }
        t += dt;
        if(t <= firstT + eps && dt > eps){
            t += dt;
        }
        assert(t > firstT - eps && (t-firstT < dt + eps));
        return;
    }
}

std::shared_ptr<IntersecInfo> Grid::intersect(glm::vec4 p, glm::vec4 ray, const double firstT, const double secondT, bool bvHitIn, const double min, const double max){
    assert(secondT >= firstT - eps);
    
    shared_ptr<IntersecInfo> retInfo = NULL;
    glm::vec3 normalTriangle;
    
    if (ray.x == 0) {
        ray.x += eps;
    }
    
    if (ray.y == 0) {
        ray.y += eps;
    }
    
    if (ray.z == 0) {
        ray.z += eps;
    }
    
    // every axis corresponding delta t
    float dtx, dty, dtz;
    dtx = abs(cx / ray.x);
    dty = abs(cy / ray.y);
    dtz = abs(cz / ray.z);
    
    // find the start cell of ray bbox intersection
    glm::vec4 startP = p + ray * (float) firstT;
    if (firstT == secondT) {
        startP = p;
    }
    // fix startP coord when too small
    float xd, yd, zd;
    xd = startP.x - bvp->xmin;
    yd = startP.y - bvp->ymin;
    zd = startP.z - bvp->zmin;
    if (xd > -eps && xd < eps) {
        startP.x = bvp->xmin;
    }
    if (yd > -eps && yd < eps) {
        startP.y = bvp->ymin;
    }
    if (zd > -eps && zd < eps) {
        startP.z = bvp->zmin;
    }
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
    assert(ix<nx && ix>=0);
    assert(iy<ny && iy>=0);
    assert(iz<nz && iz>=0);
    
    // judge if startP in bounding box surface or not
    bool hitIn = bvHitIn;
    
    // find the initial t for each coordinates
    float tx_next, ty_next, tz_next;
    tx_next = (bvp->xmin + ix*dtx - p.x) / ray.x;
    ty_next = (bvp->ymin + iy*dty - p.y) / ray.y;
    tz_next = (bvp->zmin + iz*dtz - p.z) / ray.z;
    update_t(tx_next, firstT, dtx);
    update_t(ty_next, firstT, dty);
    update_t(tz_next, firstT, dtz);
    
    while (true) {
        for (auto it=cells[ix + iy*nx + iz*ny*nx].begin(); it!=cells[ix + iy*nx + iz*ny*nx].end(); it++) {
            shared_ptr<IntersecInfo> tmpInfo = NULL;
            tmpInfo = (*it)->intersect(p, ray, min, max);
            if (tmpInfo != NULL) {
                tmpInfo->hitIn = hitIn;
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
            if(iz<0 || iz >= nz) break;
            tx_next += dtx;
            ix += (ray.x > 0)?1:-1;
            if(ix<0 || ix >= nx) break;
        } else if (ty_next < tz_next) {
            // ymin
            if(iz<0 || iz >= nz) break;
            ty_next += dty;
            iy += (ray.y > 0)?1:-1;
            if(iy<0 || iy >= ny) break;
        } else {
            // zmin
            if(iz<0 || iz >= nz) break;
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
