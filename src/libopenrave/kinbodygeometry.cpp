// -*- coding: utf-8 -*-
// Copyright (C) 2006-2014 Rosen Diankov (rosen.diankov@gmail.com)
//
// This file is part of OpenRAVE.
// OpenRAVE is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include "libopenrave.h"
#include <algorithm>

namespace OpenRAVE {

#define GTS_M_ICOSAHEDRON_X /* sqrt(sqrt(5)+1)/sqrt(2*sqrt(5)) */ \
    (dReal)0.850650808352039932181540497063011072240401406
#define GTS_M_ICOSAHEDRON_Y /* sqrt(2)/sqrt(5+sqrt(5))         */ \
    (dReal)0.525731112119133606025669084847876607285497935
#define GTS_M_ICOSAHEDRON_Z (dReal)0.0

// generate a sphere triangulation starting with an icosahedron
// all triangles are oriented counter clockwise
void GenerateSphereTriangulation(TriMesh& tri, int levels)
{
    TriMesh temp, temp2;

    temp.vertices.push_back(Vector(+GTS_M_ICOSAHEDRON_Z, +GTS_M_ICOSAHEDRON_X, -GTS_M_ICOSAHEDRON_Y));
    temp.vertices.push_back(Vector(+GTS_M_ICOSAHEDRON_X, +GTS_M_ICOSAHEDRON_Y, +GTS_M_ICOSAHEDRON_Z));
    temp.vertices.push_back(Vector(+GTS_M_ICOSAHEDRON_Y, +GTS_M_ICOSAHEDRON_Z, -GTS_M_ICOSAHEDRON_X));
    temp.vertices.push_back(Vector(+GTS_M_ICOSAHEDRON_Y, +GTS_M_ICOSAHEDRON_Z, +GTS_M_ICOSAHEDRON_X));
    temp.vertices.push_back(Vector(+GTS_M_ICOSAHEDRON_X, -GTS_M_ICOSAHEDRON_Y, +GTS_M_ICOSAHEDRON_Z));
    temp.vertices.push_back(Vector(+GTS_M_ICOSAHEDRON_Z, +GTS_M_ICOSAHEDRON_X, +GTS_M_ICOSAHEDRON_Y));
    temp.vertices.push_back(Vector(-GTS_M_ICOSAHEDRON_Y, +GTS_M_ICOSAHEDRON_Z, +GTS_M_ICOSAHEDRON_X));
    temp.vertices.push_back(Vector(+GTS_M_ICOSAHEDRON_Z, -GTS_M_ICOSAHEDRON_X, -GTS_M_ICOSAHEDRON_Y));
    temp.vertices.push_back(Vector(-GTS_M_ICOSAHEDRON_X, +GTS_M_ICOSAHEDRON_Y, +GTS_M_ICOSAHEDRON_Z));
    temp.vertices.push_back(Vector(-GTS_M_ICOSAHEDRON_Y, +GTS_M_ICOSAHEDRON_Z, -GTS_M_ICOSAHEDRON_X));
    temp.vertices.push_back(Vector(-GTS_M_ICOSAHEDRON_X, -GTS_M_ICOSAHEDRON_Y, +GTS_M_ICOSAHEDRON_Z));
    temp.vertices.push_back(Vector(+GTS_M_ICOSAHEDRON_Z, -GTS_M_ICOSAHEDRON_X, +GTS_M_ICOSAHEDRON_Y));

    const int nindices=60;
    int indices[nindices] = {
        0, 1, 2,
        1, 3, 4,
        3, 5, 6,
        2, 4, 7,
        5, 6, 8,
        2, 7, 9,
        0, 5, 8,
        7, 9, 10,
        0, 1, 5,
        7, 10, 11,
        1, 3, 5,
        6, 10, 11,
        3, 6, 11,
        9, 10, 8,
        3, 4, 11,
        6, 8, 10,
        4, 7, 11,
        1, 2, 4,
        0, 8, 9,
        0, 2, 9
    };

    Vector v[3];

    // make sure oriented CCW
    for(int i = 0; i < nindices; i += 3 ) {
        v[0] = temp.vertices[indices[i]];
        v[1] = temp.vertices[indices[i+1]];
        v[2] = temp.vertices[indices[i+2]];
        if( v[0].dot3((v[1]-v[0]).cross(v[2]-v[0])) < 0 ) {
            swap(indices[i], indices[i+1]);
        }
    }

    temp.indices.resize(nindices);
    std::copy(&indices[0],&indices[nindices],temp.indices.begin());

    TriMesh* pcur = &temp;
    TriMesh* pnew = &temp2;
    while(levels-- > 0) {

        pnew->vertices.resize(0);
        pnew->vertices.reserve(2*pcur->vertices.size());
        pnew->vertices.insert(pnew->vertices.end(), pcur->vertices.begin(), pcur->vertices.end());
        pnew->indices.resize(0);
        pnew->indices.reserve(4*pcur->indices.size());

        map< uint64_t, int > mapnewinds;
        map< uint64_t, int >::iterator it;

        for(size_t i = 0; i < pcur->indices.size(); i += 3) {
            // for ever tri, create 3 new vertices and 4 new triangles.
            v[0] = pcur->vertices[pcur->indices[i]];
            v[1] = pcur->vertices[pcur->indices[i+1]];
            v[2] = pcur->vertices[pcur->indices[i+2]];

            int inds[3];
            for(int j = 0; j < 3; ++j) {
                uint64_t key = ((uint64_t)pcur->indices[i+j]<<32)|(uint64_t)pcur->indices[i + ((j+1)%3) ];
                it = mapnewinds.find(key);

                if( it == mapnewinds.end() ) {
                    inds[j] = mapnewinds[key] = mapnewinds[(key<<32)|(key>>32)] = (int)pnew->vertices.size();
                    pnew->vertices.push_back((v[j]+v[(j+1)%3 ]).normalize3());
                }
                else {
                    inds[j] = it->second;
                }
            }

            pnew->indices.push_back(pcur->indices[i]);    pnew->indices.push_back(inds[0]);    pnew->indices.push_back(inds[2]);
            pnew->indices.push_back(inds[0]);    pnew->indices.push_back(pcur->indices[i+1]);    pnew->indices.push_back(inds[1]);
            pnew->indices.push_back(inds[2]);    pnew->indices.push_back(inds[0]);    pnew->indices.push_back(inds[1]);
            pnew->indices.push_back(inds[2]);    pnew->indices.push_back(inds[1]);    pnew->indices.push_back(pcur->indices[i+2]);
        }

        swap(pnew,pcur);
    }

    tri = *pcur;
}

/// \param ex half extents
void AppendBoxTriangulation(const Vector& pos, const Vector& ex, TriMesh& tri)
{
    // trivial
    Vector v[8] = { Vector(ex.x, ex.y, ex.z)+pos,
                    Vector(ex.x, ex.y, -ex.z)+pos,
                    Vector(ex.x, -ex.y, ex.z)+pos,
                    Vector(ex.x, -ex.y, -ex.z)+pos,
                    Vector(-ex.x, ex.y, ex.z)+pos,
                    Vector(-ex.x, ex.y, -ex.z)+pos,
                    Vector(-ex.x, -ex.y, ex.z)+pos,
                    Vector(-ex.x, -ex.y, -ex.z)+pos};
    int vertexoffset = (int)tri.vertices.size();
    const int nindices = 36;
    int indices[nindices] = {
        0+vertexoffset, 2+vertexoffset, 1+vertexoffset,
        1+vertexoffset, 2+vertexoffset, 3+vertexoffset,
        4+vertexoffset, 5+vertexoffset, 6+vertexoffset,
        5+vertexoffset, 7+vertexoffset, 6+vertexoffset,
        0+vertexoffset, 1+vertexoffset, 4+vertexoffset,
        1+vertexoffset, 5+vertexoffset, 4+vertexoffset,
        2+vertexoffset, 6+vertexoffset, 3+vertexoffset,
        3+vertexoffset, 6+vertexoffset, 7+vertexoffset,
        0+vertexoffset, 4+vertexoffset, 2+vertexoffset,
        2+vertexoffset, 4+vertexoffset, 6+vertexoffset,
        1+vertexoffset, 3+vertexoffset, 5+vertexoffset,
        3+vertexoffset, 7+vertexoffset, 5+vertexoffset
    };
    tri.vertices.insert(tri.vertices.end(), &v[0], &v[8]);
    tri.indices.insert(tri.indices.end(), &indices[0], &indices[nindices]);
}

KinBody::GeometryInfo::GeometryInfo() : XMLReadable("geometry")
{
    _vDiffuseColor = Vector(1,1,1);
    _type = GT_None;
    _fTransparency = 0;
    _vRenderScale = _vCollisionScale = Vector(1,1,1);
    _bVisible = true;
    _bModifiable = true;
}

bool KinBody::GeometryInfo::InitCollisionMesh(float fTessellation)
{
    if( _type == GT_TriMesh || _type == GT_None ) {
        return true;
    }

    // is clear() better since it releases the memory?
    _meshcollision.indices.resize(0);
    _meshcollision.vertices.resize(0);

    if( fTessellation < 0.01f ) {
        fTessellation = 0.01f;
    }
    // start tesselating
    switch(_type) {
    case GT_Sphere: {
        // log_2 (1+ tess)
        GenerateSphereTriangulation(_meshcollision, 3 + (int)(logf(fTessellation) / logf(2.0f)) );
        dReal fRadius = GetSphereRadius();
        FOREACH(it, _meshcollision.vertices) {
            *it *= fRadius;
        }
        break;
    }
    case GT_Box: {
        // trivial
        Vector ex = GetBoxExtents();
        Vector v[8] = { Vector(ex.x, ex.y, ex.z),
                        Vector(ex.x, ex.y, -ex.z),
                        Vector(ex.x, -ex.y, ex.z),
                        Vector(ex.x, -ex.y, -ex.z),
                        Vector(-ex.x, ex.y, ex.z),
                        Vector(-ex.x, ex.y, -ex.z),
                        Vector(-ex.x, -ex.y, ex.z),
                        Vector(-ex.x, -ex.y, -ex.z) };
        const int nindices = 36;
        int indices[] = {
            0, 2, 1,
            1, 2, 3,
            4, 5, 6,
            5, 7, 6,
            0, 1, 4,
            1, 5, 4,
            2, 6, 3,
            3, 6, 7,
            0, 4, 2,
            2, 4, 6,
            1, 3, 5,
            3, 7, 5
        };
        _meshcollision.vertices.resize(8);
        std::copy(&v[0],&v[8],_meshcollision.vertices.begin());
        _meshcollision.indices.resize(nindices);
        std::copy(&indices[0],&indices[nindices],_meshcollision.indices.begin());
        break;
    }
    case GT_Cylinder: {
        // cylinder is on z axis
        dReal rad = GetCylinderRadius(), len = GetCylinderHeight()*0.5f;
        int numverts = (int)(fTessellation*48.0f) + 3;
        dReal dtheta = 2 * PI / (dReal)numverts;
        _meshcollision.vertices.push_back(Vector(0,0,len));
        _meshcollision.vertices.push_back(Vector(0,0,-len));
        _meshcollision.vertices.push_back(Vector(rad,0,len));
        _meshcollision.vertices.push_back(Vector(rad,0,-len));
        for(int i = 0; i < numverts+1; ++i) {
            dReal s = rad * RaveSin(dtheta * (dReal)i);
            dReal c = rad * RaveCos(dtheta * (dReal)i);
            int off = (int)_meshcollision.vertices.size();
            _meshcollision.vertices.push_back(Vector(c, s, len));
            _meshcollision.vertices.push_back(Vector(c, s, -len));

            _meshcollision.indices.push_back(0);       _meshcollision.indices.push_back(off-2);       _meshcollision.indices.push_back(off);
            _meshcollision.indices.push_back(1);       _meshcollision.indices.push_back(off+1);       _meshcollision.indices.push_back(off-1);
            _meshcollision.indices.push_back(off-2);   _meshcollision.indices.push_back(off-1);         _meshcollision.indices.push_back(off);
            _meshcollision.indices.push_back(off);   _meshcollision.indices.push_back(off-1);         _meshcollision.indices.push_back(off+1);
        }
        break;
    }
    case GT_Cage: {
        const Vector& vCageBaseExtents = _vGeomData;
        for (size_t i = 0; i < _vSideWalls.size(); ++i) {
            const SideWall &s = _vSideWalls[i];
            const size_t vBase = _meshcollision.vertices.size();
            AppendBoxTriangulation(Vector(0, 0, s.vExtents[2]), s.vExtents, _meshcollision);

            for (size_t j = 0; j < 8; ++j) {
                _meshcollision.vertices[vBase + j] = s.transf * _meshcollision.vertices[vBase + j];
            }
        }
        // finally add the base
        AppendBoxTriangulation(Vector(0, 0, vCageBaseExtents.z),vCageBaseExtents, _meshcollision);
        break;
    }
    case GT_Container: {
        const Vector& outerextents = _vGeomData;
        const Vector& innerextents = _vGeomData2;
        const Vector& bottomcross = _vGeomData3;
        // +x wall
        AppendBoxTriangulation(Vector((outerextents[0]+innerextents[0])/4.,0,outerextents[2]/2.), Vector((outerextents[0]-innerextents[0])/4., outerextents[1]/2., outerextents[2]/2.), _meshcollision);
        // -x wall
        AppendBoxTriangulation(Vector(-(outerextents[0]+innerextents[0])/4.,0,outerextents[2]/2.), Vector((outerextents[0]-innerextents[0])/4., outerextents[1]/2., outerextents[2]/2.), _meshcollision);
        // +y wall
        AppendBoxTriangulation(Vector(0,(outerextents[1]+innerextents[1])/4.,outerextents[2]/2.), Vector(outerextents[0]/2., (outerextents[1]-innerextents[1])/4., outerextents[2]/2.), _meshcollision);
        // -y wall
        AppendBoxTriangulation(Vector(0,-(outerextents[1]+innerextents[1])/4.,outerextents[2]/2.), Vector(outerextents[0]/2., (outerextents[1]-innerextents[1])/4., outerextents[2]/2.), _meshcollision);
        // bottom
        if( outerextents[2] - innerextents[2] >= 1e-6 ) { // small epsilon error can make thin triangles appear, so test with a reasonable threshold
            AppendBoxTriangulation(Vector(0,0,(outerextents[2]-innerextents[2])/2.), Vector(outerextents[0]/2., outerextents[1]/2., (outerextents[2]-innerextents[2])/2), _meshcollision);
        }
        // cross
        if( bottomcross[2] > 0 ) {
            if( bottomcross[0] > 0 ) {
                AppendBoxTriangulation(Vector(0, 0, bottomcross[2]/2+outerextents[2]-innerextents[2]), Vector(bottomcross[0]/2, innerextents[1]/2, bottomcross[2]/2), _meshcollision);
            }
            if( bottomcross[1] > 0 ) {
                AppendBoxTriangulation(Vector(0, 0, bottomcross[2]/2+outerextents[2]-innerextents[2]), Vector(innerextents[0]/2, bottomcross[1]/2, bottomcross[2]/2), _meshcollision);
            }
        }
        break;
    }
    default:
        throw OPENRAVE_EXCEPTION_FORMAT(_("unrecognized geom type %d!"), _type, ORE_InvalidArguments);
    }

    return true;
}

bool KinBody::GeometryInfo::ComputeInnerEmptyVolume(Transform& tInnerEmptyVolume, Vector& abInnerEmptyExtents) const
{
    switch(_type) {
    case GT_Cage: {
        Vector vmin, vmax;
        vmax.z = vmin.z = _vGeomData.z*2;
        
        // initialize to the base extents if there is no wall
        vmin.x = -_vGeomData.x;
        vmin.y = -_vGeomData.y;
        vmax.x = _vGeomData.x;
        vmax.y = _vGeomData.y;

        FOREACH(itwall, _vSideWalls) {
            // compute the XYZ extents of the wall
            Vector vxaxis = geometry::ExtractAxisFromQuat(itwall->transf.rot, 0);
            Vector vyaxis = geometry::ExtractAxisFromQuat(itwall->transf.rot, 1);
            Vector vzaxis = geometry::ExtractAxisFromQuat(itwall->transf.rot, 2);

            Vector vprojectedextents;
            for(int idim = 0; idim < 3; ++idim) {
                vprojectedextents[idim] = RaveFabs(vxaxis[idim])*itwall->vExtents.x + RaveFabs(vyaxis[idim])*itwall->vExtents.y + RaveFabs(vzaxis[idim])*itwall->vExtents.z;
            }

            // the origin of the side wall is the bottom center
            if( vmax.z < itwall->transf.trans.z + 2*vprojectedextents.z ) {
                vmax.z = itwall->transf.trans.z + 2*vprojectedextents.z;
            }

            switch(itwall->type) {
            case GeometryInfo::SWT_NX:
                vmin.x = itwall->transf.trans.x + vprojectedextents.x;
                break;
            case GeometryInfo::SWT_NY:
                vmin.y = itwall->transf.trans.y + vprojectedextents.y;
                break;
            case GeometryInfo::SWT_PX:
                vmax.x = itwall->transf.trans.x - vprojectedextents.x;
                break;
            case GeometryInfo::SWT_PY:
                vmax.y = itwall->transf.trans.y - vprojectedextents.y;
                break;
            }
        }

        // force inner region
        // if there are walls, respect the walls first
        if( _vGeomData2.x > 0 ) {
            if( vmin.x < -0.5*_vGeomData2.x ) {
                vmin.x = -0.5*_vGeomData2.x;
            }
            if( vmax.x > 0.5*_vGeomData2.x ) {
                vmax.x = 0.5*_vGeomData2.x;
            }
        }
        if( _vGeomData2.y > 0 ) {
            if( vmin.y < -0.5*_vGeomData2.y ) {
                vmin.y = -0.5*_vGeomData2.y;
            }
            if( vmax.y > 0.5*_vGeomData2.y ) {
                vmax.y = 0.5*_vGeomData2.y;
            }
        }

        // the top has no constraints, so use the max of walls and force inner region
        if( vmax.z < _vGeomData.z*2 + _vGeomData2.z ) {
            vmax.z = _vGeomData.z*2 + _vGeomData2.z;
        }

        abInnerEmptyExtents = 0.5*(vmax - vmin);
        tInnerEmptyVolume = _t;
        tInnerEmptyVolume.trans += tInnerEmptyVolume.rotate(0.5*(vmax + vmin));
        return true;
    }
    case GT_Container: {
        Transform tempty;
        // full outer extents - full inner extents + inner extents = _vGeomData.z - 0.5*_vGeomData2.z
        tempty.trans.z = _vGeomData.z - 0.5 * _vGeomData2.z;
        tInnerEmptyVolume = _t*tempty;
        abInnerEmptyExtents = 0.5*_vGeomData2;
        return true;
    }
    default:
        return false;
    }
}

void RaveSerializeJSON(rapidjson::Value &rSideWall, rapidjson::Document::AllocatorType& allocator, const KinBody::GeometryInfo::SideWall& sidewall)
{
    RAVE_SERIALIZEJSON_ENSURE_OBJECT(rSideWall);
    RAVE_SERIALIZEJSON_ADDMEMBER(rSideWall, allocator, "transform", sidewall.transf);
    RAVE_SERIALIZEJSON_ADDMEMBER(rSideWall, allocator, "halfExtents", sidewall.vExtents);
    RAVE_SERIALIZEJSON_ADDMEMBER(rSideWall, allocator, "type", (int)sidewall.type);
}

void RaveDeserializeJSON(const rapidjson::Value &value, KinBody::GeometryInfo::SideWall& sidewall)
{
    RAVE_DESERIALIZEJSON_ENSURE_OBJECT(value);
    RAVE_DESERIALIZEJSON_REQUIRED(value, "transform", sidewall.transf);
    RAVE_DESERIALIZEJSON_REQUIRED(value, "halfExtents", sidewall.vExtents);
    int type=0;
    RAVE_DESERIALIZEJSON_REQUIRED(value, "type", type);
    sidewall.type = (KinBody::GeometryInfo::SideWallType)type;
}

void KinBody::GeometryInfo::SerializeJSON(rapidjson::Value &value, rapidjson::Document::AllocatorType& allocator, const dReal fUnitScale, int options)
{
    RAVE_SERIALIZEJSON_ENSURE_OBJECT(value);

    //RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "sid", sid);
    RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "name", _name);

    Transform tscaled=_t;
    tscaled.trans *= fUnitScale;
    RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "transform", tscaled);

    switch(_type) {
    case GT_Box:
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "type", "box");
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "halfExtents", _vGeomData*fUnitScale);
        break;

    case GT_Container:
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "type", "container");
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "outerExtents", _vGeomData*fUnitScale);
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "innerExtents", _vGeomData2*fUnitScale);
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "bottomCross", _vGeomData3*fUnitScale);
        break;

    case GT_Cage: {
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "type", "cage");
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "baseExtents", _vGeomData*fUnitScale);

        std::vector<SideWall> vScaledSideWalls = _vSideWalls;
        FOREACH(itwall, vScaledSideWalls) {
            itwall->transf.trans *= fUnitScale;
            itwall->vExtents *= fUnitScale;
        }
        if( _vGeomData2.x > g_fEpsilon ) {
            RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "innerSizeX", _vGeomData2.x*fUnitScale);
        }
        if( _vGeomData2.y > g_fEpsilon ) {
            RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "innerSizeY", _vGeomData2.y*fUnitScale);
        }
        if( _vGeomData2.z > g_fEpsilon ) {
            RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "innerSizeZ", _vGeomData2.z*fUnitScale);
        }
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "sideWalls", vScaledSideWalls);
        break;
    }
    case GT_Sphere:
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "type", "sphere");
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "radius", _vGeomData.x*fUnitScale);
        break;

    case GT_Cylinder:
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "type", "cylinder");
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "radius", _vGeomData.x*fUnitScale);
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "height", _vGeomData.y*fUnitScale);
        break;

    case GT_TriMesh:
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "type", "trimesh");
        RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "mesh", _meshcollision);
        break;

    default:
        break;
    }

    RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "transparency", _fTransparency);
    RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "visible", _bVisible);
    RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "diffuseColor", _vDiffuseColor);
    RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "ambientColor", _vAmbientColor);
    RAVE_SERIALIZEJSON_ADDMEMBER(value, allocator, "modifiable", _bModifiable);
}

void KinBody::GeometryInfo::DeserializeJSON(const rapidjson::Value &value, const dReal fUnitScale)
{
    RAVE_DESERIALIZEJSON_ENSURE_OBJECT(value);

    //RAVE_DESERIALIZEJSON_REQUIRED(value, "sid", sid);
    RAVE_DESERIALIZEJSON_REQUIRED(value, "name", _name);
    RAVE_DESERIALIZEJSON_REQUIRED(value, "transform", _t);

    _t.trans *= fUnitScale;

    std::string typestr;
    RAVE_DESERIALIZEJSON_REQUIRED(value, "type", typestr);

    if (typestr == "box") {
        _type = GT_Box;
        RAVE_DESERIALIZEJSON_REQUIRED(value, "halfExtents", _vGeomData);
        _vGeomData *= fUnitScale;
    }
    else if (typestr == "container") {
        _type = GT_Container;
        RAVE_DESERIALIZEJSON_REQUIRED(value, "outerExtents", _vGeomData);
        RAVE_DESERIALIZEJSON_REQUIRED(value, "innerExtents", _vGeomData2);
        RAVE_DESERIALIZEJSON_REQUIRED(value, "bottomCross", _vGeomData3);

        _vGeomData *= fUnitScale;
        _vGeomData2 *= fUnitScale;
        _vGeomData3 *= fUnitScale;
    }
    else if (typestr == "cage") {
        _type = GT_Cage;
        RAVE_DESERIALIZEJSON_REQUIRED(value, "baseExtents", _vGeomData);
        _vGeomData *= fUnitScale;

        _vGeomData2 = Vector();
        RAVE_DESERIALIZEJSON_OPTIONAL(value, "innerSizeX", _vGeomData2.x);
        RAVE_DESERIALIZEJSON_OPTIONAL(value, "innerSizeY", _vGeomData2.y);
        RAVE_DESERIALIZEJSON_OPTIONAL(value, "innerSizeZ", _vGeomData2.z);
        _vGeomData2 *= fUnitScale;

        RAVE_DESERIALIZEJSON_REQUIRED(value, "sideWalls", _vSideWalls);
        FOREACH(itsidewall, _vSideWalls) {
            itsidewall->transf.trans *= fUnitScale;
            itsidewall->vExtents *= fUnitScale;
        }
    }
    else if (typestr == "sphere") {
        _type = GT_Sphere;
        RAVE_DESERIALIZEJSON_REQUIRED(value, "radius", _vGeomData.x);

        _vGeomData *= fUnitScale;
    }
    else if (typestr == "cylinder") {
        _type = GT_Cylinder;
        RAVE_DESERIALIZEJSON_REQUIRED(value, "radius", _vGeomData.x);
        RAVE_DESERIALIZEJSON_REQUIRED(value, "height", _vGeomData.y);

        _vGeomData.x *= fUnitScale;
        _vGeomData.y *= fUnitScale;

    }
    else if (typestr == "trimesh" or typestr == "mesh") {
        _type = GT_TriMesh;
        RAVE_DESERIALIZEJSON_REQUIRED(value, "mesh", _meshcollision);

        FOREACH(itvertex, _meshcollision.vertices) {
            *itvertex *= fUnitScale;
        }
    }
    else {
        throw OPENRAVE_EXCEPTION_FORMAT("failed to deserialize json, unsupported geometry type \"%s\"", typestr, ORE_InvalidArguments);
    }

    RAVE_DESERIALIZEJSON_REQUIRED(value, "transparency", _fTransparency);
    RAVE_DESERIALIZEJSON_REQUIRED(value, "visible", _bVisible);
    RAVE_DESERIALIZEJSON_REQUIRED(value, "diffuseColor", _vDiffuseColor);
    RAVE_DESERIALIZEJSON_REQUIRED(value, "ambientColor", _vAmbientColor);
    RAVE_DESERIALIZEJSON_REQUIRED(value, "modifiable", _bModifiable);
}

KinBody::Link::Geometry::Geometry(KinBody::LinkPtr parent, const KinBody::GeometryInfo& info) : _parent(parent), _info(info)
{
}

bool KinBody::Link::Geometry::InitCollisionMesh(float fTessellation)
{
    return _info.InitCollisionMesh(fTessellation);
}

bool KinBody::Link::Geometry::ComputeInnerEmptyVolume(Transform& tInnerEmptyVolume, Vector& abInnerEmptyExtents) const
{
    return _info.ComputeInnerEmptyVolume(tInnerEmptyVolume, abInnerEmptyExtents);
}

AABB KinBody::Link::Geometry::ComputeAABB(const Transform& t) const
{
    AABB ab;
    TransformMatrix tglobal = t * _info._t;

    switch(_info._type) {
    case GT_None:
        ab.extents.x = 0;
        ab.extents.y = 0;
        ab.extents.z = 0;
        break;
    case GT_Box: // origin of box is at the center
        ab.extents.x = RaveFabs(tglobal.m[0])*_info._vGeomData.x + RaveFabs(tglobal.m[1])*_info._vGeomData.y + RaveFabs(tglobal.m[2])*_info._vGeomData.z;
        ab.extents.y = RaveFabs(tglobal.m[4])*_info._vGeomData.x + RaveFabs(tglobal.m[5])*_info._vGeomData.y + RaveFabs(tglobal.m[6])*_info._vGeomData.z;
        ab.extents.z = RaveFabs(tglobal.m[8])*_info._vGeomData.x + RaveFabs(tglobal.m[9])*_info._vGeomData.y + RaveFabs(tglobal.m[10])*_info._vGeomData.z;
        ab.pos = tglobal.trans;
        break;
    case GT_Container: // origin of container is at the bottom
        ab.extents.x = 0.5*(RaveFabs(tglobal.m[0])*_info._vGeomData.x + RaveFabs(tglobal.m[1])*_info._vGeomData.y + RaveFabs(tglobal.m[2])*_info._vGeomData.z);
        ab.extents.y = 0.5*(RaveFabs(tglobal.m[4])*_info._vGeomData.x + RaveFabs(tglobal.m[5])*_info._vGeomData.y + RaveFabs(tglobal.m[6])*_info._vGeomData.z);
        ab.extents.z = 0.5*(RaveFabs(tglobal.m[8])*_info._vGeomData.x + RaveFabs(tglobal.m[9])*_info._vGeomData.y + RaveFabs(tglobal.m[10])*_info._vGeomData.z);
        ab.pos = tglobal.trans + Vector(tglobal.m[2], tglobal.m[6], tglobal.m[10])*(0.5*_info._vGeomData.z);
        break;
    case GT_Sphere:
        ab.extents.x = ab.extents.y = ab.extents.z = _info._vGeomData[0];
        ab.pos = tglobal.trans;
        break;
    case GT_Cylinder:
        ab.extents.x = (dReal)0.5*RaveFabs(tglobal.m[2])*_info._vGeomData.y + RaveSqrt(max(dReal(0),1-tglobal.m[2]*tglobal.m[2]))*_info._vGeomData.x;
        ab.extents.y = (dReal)0.5*RaveFabs(tglobal.m[6])*_info._vGeomData.y + RaveSqrt(max(dReal(0),1-tglobal.m[6]*tglobal.m[6]))*_info._vGeomData.x;
        ab.extents.z = (dReal)0.5*RaveFabs(tglobal.m[10])*_info._vGeomData.y + RaveSqrt(max(dReal(0),1-tglobal.m[10]*tglobal.m[10]))*_info._vGeomData.x;
        ab.pos = tglobal.trans; //+(dReal)0.5*_info._vGeomData.y*Vector(tglobal.m[2],tglobal.m[6],tglobal.m[10]);
        break;
    case GT_Cage: {
        // have to return the entire volume, even the inner region since a lot of code use the bounding box to compute cropping and other functions
        const Vector& vCageBaseExtents = _info._vGeomData;
        const Vector& vCageForceInnerFull = _info._vGeomData2;

        Vector vmin, vmax;
        vmin.x = -vCageBaseExtents.x;
        vmin.y = -vCageBaseExtents.y;
        vmax.x = vCageBaseExtents.x;
        vmax.y = vCageBaseExtents.y;
        vmax.z = vCageBaseExtents.z*2;
        for (size_t i = 0; i < _info._vSideWalls.size(); ++i) {
            const GeometryInfo::SideWall &s = _info._vSideWalls[i];
            TransformMatrix sidewallmat = s.transf;
            Vector vselocal = s.vExtents;
            Vector vsegeom;
            vsegeom.x = RaveFabs(sidewallmat.m[0])*vselocal.x + RaveFabs(sidewallmat.m[1])*vselocal.y + RaveFabs(sidewallmat.m[2])*vselocal.z;
            vsegeom.y = RaveFabs(sidewallmat.m[4])*vselocal.x + RaveFabs(sidewallmat.m[5])*vselocal.y + RaveFabs(sidewallmat.m[6])*vselocal.z;
            vsegeom.z = RaveFabs(sidewallmat.m[8])*vselocal.x + RaveFabs(sidewallmat.m[9])*vselocal.y + RaveFabs(sidewallmat.m[10])*vselocal.z;

            Vector vsidemin = s.transf.trans - vsegeom;
            Vector vsidemax = s.transf.trans + vsegeom;
            
            if( vmin.x > vsidemin.x ) {
                vmin.x = vsidemin.x;
            }
            if( vmin.y > vsidemin.y ) {
                vmin.y = vsidemin.y;
            }
            if( vmin.z > vsidemin.z ) {
                vmin.z = vsidemin.z;
            }
            if( vmax.x < vsidemax.x ) {
                vmax.x = vsidemax.x;
            }
            if( vmax.y < vsidemax.y ) {
                vmax.y = vsidemax.y;
            }
            if( vmax.z < vsidemax.z ) {
                vmax.z = vsidemax.z;
            }
        }

        if( vCageForceInnerFull.x > 0 ) {
            if( vmin.x > -0.5*vCageForceInnerFull.x ) {
                vmin.x = -0.5*vCageForceInnerFull.x;
            }
            if( vmax.x < 0.5*vCageForceInnerFull.x ) {
                vmax.x = 0.5*vCageForceInnerFull.x;
            }
        }
        if( vCageForceInnerFull.y > 0 ) {
            if( vmin.y > -0.5*vCageForceInnerFull.y ) {
                vmin.y = -0.5*vCageForceInnerFull.y;
            }
            if( vmax.y < 0.5*vCageForceInnerFull.y ) {
                vmax.y = 0.5*vCageForceInnerFull.y;
            }
        }
        if( vCageForceInnerFull.z > 0 ) {
            if( vmax.z < vCageBaseExtents.z*2+vCageForceInnerFull.z ) {
                vmax.z = vCageBaseExtents.z*2+vCageForceInnerFull.z;
            }
        }

        // now that vmin and vmax are in geom space, transform them
        Vector vgeomextents = 0.5*(vmax-vmin);
        
        ab.extents.x = RaveFabs(tglobal.m[0])*vgeomextents.x + RaveFabs(tglobal.m[1])*vgeomextents.y + RaveFabs(tglobal.m[2])*vgeomextents.z;
        ab.extents.y = RaveFabs(tglobal.m[4])*vgeomextents.x + RaveFabs(tglobal.m[5])*vgeomextents.y + RaveFabs(tglobal.m[6])*vgeomextents.z;
        ab.extents.z = RaveFabs(tglobal.m[8])*vgeomextents.x + RaveFabs(tglobal.m[9])*vgeomextents.y + RaveFabs(tglobal.m[10])*vgeomextents.z;
        ab.pos = tglobal*(0.5*(vmin+vmax));
        break;

    }
    case GT_TriMesh: {
        // Cage: init collision mesh?
        // just use _info._meshcollision
        if( _info._meshcollision.vertices.size() > 0) {
            Vector vmin, vmax; vmin = vmax = tglobal*_info._meshcollision.vertices.at(0);
            FOREACHC(itv, _info._meshcollision.vertices) {
                Vector v = tglobal * *itv;
                if( vmin.x > v.x ) {
                    vmin.x = v.x;
                }
                if( vmin.y > v.y ) {
                    vmin.y = v.y;
                }
                if( vmin.z > v.z ) {
                    vmin.z = v.z;
                }
                if( vmax.x < v.x ) {
                    vmax.x = v.x;
                }
                if( vmax.y < v.y ) {
                    vmax.y = v.y;
                }
                if( vmax.z < v.z ) {
                    vmax.z = v.z;
                }
            }
            ab.extents = (dReal)0.5*(vmax-vmin);
            ab.pos = (dReal)0.5*(vmax+vmin);
        }
        else {
            ab.pos = tglobal.trans;
        }
        break;
    }
    default:
        throw OPENRAVE_EXCEPTION_FORMAT(_("unknown geometry type %d"), _info._type, ORE_InvalidArguments);
    }

    return ab;
}

void KinBody::Link::Geometry::serialize(std::ostream& o, int options) const
{
    SerializeRound(o,_info._t);
    o << _info._type << " ";
    SerializeRound3(o,_info._vRenderScale);
    if( _info._type == GT_TriMesh ) {
        _info._meshcollision.serialize(o,options);
    }
    else {
        SerializeRound3(o,_info._vGeomData);
    }
}

void KinBody::Link::Geometry::SetCollisionMesh(const TriMesh& mesh)
{
    OPENRAVE_ASSERT_FORMAT0(_info._bModifiable, "geometry cannot be modified", ORE_Failed);
    LinkPtr parent(_parent);
    _info._meshcollision = mesh;
    parent->_Update();
}

bool KinBody::Link::Geometry::SetVisible(bool visible)
{
    if( _info._bVisible != visible ) {
        _info._bVisible = visible;
        LinkPtr parent(_parent);
        parent->GetParent()->_PostprocessChangedParameters(Prop_LinkDraw);
        return true;
    }
    return false;
}

void KinBody::Link::Geometry::SetTransparency(float f)
{
    LinkPtr parent(_parent);
    _info._fTransparency = f;
    parent->GetParent()->_PostprocessChangedParameters(Prop_LinkDraw);
}

void KinBody::Link::Geometry::SetDiffuseColor(const RaveVector<float>& color)
{
    LinkPtr parent(_parent);
    _info._vDiffuseColor = color;
    parent->GetParent()->_PostprocessChangedParameters(Prop_LinkDraw);
}

void KinBody::Link::Geometry::SetAmbientColor(const RaveVector<float>& color)
{
    LinkPtr parent(_parent);
    _info._vAmbientColor = color;
    parent->GetParent()->_PostprocessChangedParameters(Prop_LinkDraw);
}

/*
 * Ray-box intersection using IEEE numerical properties to ensure that the
 * test is both robust and efficient, as described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */
//static bool RayAABBIntersect(const Ray &r, float t0, float t1) const
//{
//    dReal tmin, tmax, tymin, tymax, tzmin, tzmax;
//    tmin = (parameters[r.sign[0]].x() - r.origin.x()) * r.inv_direction.x();
//    tmax = (parameters[1-r.sign[0]].x() - r.origin.x()) * r.inv_direction.x();
//    tymin = (parameters[r.sign[1]].y() - r.origin.y()) * r.inv_direction.y();
//    tymax = (parameters[1-r.sign[1]].y() - r.origin.y()) * r.inv_direction.y();
//    if ( (tmin > tymax) || (tymin > tmax) )
//        return false;
//    if (tymin > tmin)
//        tmin = tymin;
//    if (tymax < tmax)
//        tmax = tymax;
//    tzmin = (parameters[r.sign[2]].z() - r.origin.z()) * r.inv_direction.z();
//    tzmax = (parameters[1-r.sign[2]].z() - r.origin.z()) * r.inv_direction.z();
//    if ( (tmin > tzmax) || (tzmin > tmax) )
//        return false;
//    if (tzmin > tmin)
//        tmin = tzmin;
//    if (tzmax < tmax)
//        tmax = tzmax;
//    return ( (tmin < t1) && (tmax > t0) );
//}

bool KinBody::Link::Geometry::ValidateContactNormal(const Vector& _position, Vector& _normal) const
{
    Transform tinv = _info._t.inverse();
    Vector position = tinv*_position;
    Vector normal = tinv.rotate(_normal);
    const dReal feps=0.00005f;
    switch(_info._type) {
    case GT_Box: {
        // transform position in +x+y+z octant
        Vector tposition=position, tnormal=normal;
        if( tposition.x < 0) {
            tposition.x = -tposition.x;
            tnormal.x = -tnormal.x;
        }
        if( tposition.y < 0) {
            tposition.y = -tposition.y;
            tnormal.y = -tnormal.y;
        }
        if( tposition.z < 0) {
            tposition.z = -tposition.z;
            tnormal.z = -tnormal.z;
        }
        // find the normal to the surface depending on the region the position is in
        dReal xaxis = -_info._vGeomData.z*tposition.y+_info._vGeomData.y*tposition.z;
        dReal yaxis = -_info._vGeomData.x*tposition.z+_info._vGeomData.z*tposition.x;
        dReal zaxis = -_info._vGeomData.y*tposition.x+_info._vGeomData.x*tposition.y;
        dReal penetration=0;
        if((zaxis < feps)&&(yaxis > -feps)) { // x-plane
            if( RaveFabs(tnormal.x) > RaveFabs(penetration) ) {
                penetration = tnormal.x;
            }
        }
        if((zaxis > -feps)&&(xaxis < feps)) { // y-plane
            if( RaveFabs(tnormal.y) > RaveFabs(penetration) ) {
                penetration = tnormal.y;
            }
        }
        if((yaxis < feps)&&(xaxis > -feps)) { // z-plane
            if( RaveFabs(tnormal.z) > RaveFabs(penetration) ) {
                penetration = tnormal.z;
            }
        }
        if( penetration < -feps ) {
            _normal = -_normal;
            return true;
        }
        break;
    }
    case GT_Cylinder: { // z-axis
        dReal fInsideCircle = position.x*position.x+position.y*position.y-_info._vGeomData.x*_info._vGeomData.x;
        dReal fInsideHeight = 2.0f*RaveFabs(position.z)-_info._vGeomData.y;
        if((fInsideCircle < -feps)&&(fInsideHeight > -feps)&&(normal.z*position.z<0)) {
            _normal = -_normal;
            return true;
        }
        if((fInsideCircle > -feps)&&(fInsideHeight < -feps)&&(normal.x*position.x+normal.y*position.y < 0)) {
            _normal = -_normal;
            return true;
        }
        break;
    }
    case GT_Sphere:
        if( normal.dot3(position) < 0 ) {
            _normal = -_normal;
            return true;
        }
        break;
    case GT_None:
    default:
        break;
    }
    return false;
}

void KinBody::Link::Geometry::SetRenderFilename(const std::string& renderfilename)
{
    LinkPtr parent(_parent);
    _info._filenamerender = renderfilename;
    parent->GetParent()->_PostprocessChangedParameters(Prop_LinkGeometry);
}

void KinBody::Link::Geometry::SetName(const std::string& name)
{
    LinkPtr parent(_parent);
    _info._name = name;
    parent->GetParent()->_PostprocessChangedParameters(Prop_LinkGeometry);

}

uint8_t KinBody::Link::Geometry::GetSideWallExists() const
{
    uint8_t mask = 0;
    for (size_t i = 0; i < _info._vSideWalls.size(); ++i) {
        mask |= 1 << _info._vSideWalls[i].type;
    }
    return mask;
}

}
