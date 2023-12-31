/*
MIT License

Copyright (c) 2023 BJLAB LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once


namespace INSHAPE {

using namespace std;

template <class VA, class A>
inline void push_back(int stride, VA &v, A a)
{
    v.push_back((decltype(v.back()))a);
    for(int i=1; i<stride; i++) {
        v.push_back(A(0));
    }
}

template <class VA, class A0, class... A123>
inline void push_back(int stride, VA &v, A0 a0, A123... a123)
{
    v.push_back((decltype(v.back()))a0);
    return push_back(stride-1, v, a123...);
}

inline void setFaceID(VtxIdxStruct &vis, const int faceID)
{
    if(vis.offset_faceID < 0) return;

    auto ix = (uint32_t)(vis.vtxData.size()) - vis.stride;
    vis.vtxData[ix+vis.offset_faceID] = float(faceID);
}

inline void setNormal(VtxIdxStruct &vis, const float nx, const float ny, const float nz)
{
    if(vis.offset_normal < 0) return;

    auto ix = (uint32_t)(vis.vtxData.size()) - vis.stride;
    vis.vtxData[ix+vis.offset_normal  ] = nx;
    vis.vtxData[ix+vis.offset_normal+1] = ny;
    vis.vtxData[ix+vis.offset_normal+2] = nz;
}

inline void setColor(VtxIdxStruct &vis, const float r, const float g, const float b, const float a)
{
    if(vis.offset_color < 0) return;

    auto ix = (uint32_t)(vis.vtxData.size()) - vis.stride;
    vis.vtxData[ix+vis.offset_color  ] = r;
    vis.vtxData[ix+vis.offset_color+1] = g;
    vis.vtxData[ix+vis.offset_color+2] = b;
    vis.vtxData[ix+vis.offset_color+3] = a;
}

template <class U>
inline void setTexCoords(vector<float> &vtxData, const int loc, U u)
{
    vtxData[loc] = u;
}

template <class U0, class... U123>
inline void setTexCoords(vector<float> &vtxData, const int loc, U0 u, U123... u123)
{
    setTexCoords(vtxData, loc+1, u123);
}

template <class U0, class... U123>
inline void setTexCoords(VtxIdxStruct &vis, const int texCoordID, U0 u, U123... u123)
{
    if(vis.offset_texCoords[texCoordID] < 0) return;

    auto loc = (uint32_t)(vis.vtxData.size()) - vis.stride
             + vis.offset_texCoords[texCoordID];
    setTexCoords(vis.vtxData, loc, u, u123);
}

// ----------------------------------------------------------------------------------
inline void append_vtx_idx(
    VtxIdxStruct &vis,
    const vector<float> &vtx,
    const vector<int>   &idx)
{
    assert(0==(vtx.size()%vis.stride));

    {
        int idx_offset = int(vis.vtxData.size()/vis.stride);
        auto n = vis.idxData.size();
        vis.idxData.resize(vis.idxData.size() + idx.size());
        for(size_t i=0; i<idx.size(); i++) {
            vis.idxData[n++] = idx[i] + idx_offset;
        }
    }
    {
        auto n = vis.vtxData.size();
        vis.vtxData.resize(vis.vtxData.size() + vtx.size());
        for(size_t i=0; i<vtx.size(); i++) {
            vis.vtxData[n++] = vtx[i];
        }
    }
}

// ----------------------------------------------------------------------------------
inline VtxIdxRange append_points(
    VtxIdxStruct &vis,
    const int   n)
{
    VtxIdxRange out;
    out.v0 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i0 = uint32_t(vis.idxData.size());

    vis.vtxData.resize(vis.vtxData.size() + vis.stride*n);
    for(int i=0; i<n; i++) {
        int vi = out.v0 + i*vis.stride;
        for(int j=0; j<vis.stride; j++) {
            vis.vtxData[vi+j] = float(rand()) / RAND_MAX;
        }
    }

    out.v1 = uint32_t(vis.vtxData.size() / vis.stride);

    return out;
}

inline VtxIdxRange append_lines(
    VtxIdxStruct &vis,
    const int   n)
{
    VtxIdxRange out;
    out.v0 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i0 = uint32_t(vis.idxData.size());

    vis.vtxData.resize(vis.vtxData.size() + vis.stride*n*2);
    vis.idxData.resize(vis.idxData.size() +            n*2);

    for(int i=0; i<n; i++) {
        int vi = out.v0 + i*vis.stride;
        for(int j=0; j<vis.stride; j++) {
            vis.vtxData[vi+j] = float(rand()) / RAND_MAX;
        }
        vis.vtxData[vi+0] += 1;
        vis.vtxData[vi+1] += 1;
        vis.vtxData[vi+2] += 1;

        int ii = out.i0 + i*2;
        vis.idxData[ii  ] = i*2;
        vis.idxData[ii+1] = i*2+1;
    }

    out.v1 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i1 = uint32_t(vis.idxData.size());

    return out;
}

inline VtxIdxRange append_plane(
    VtxIdxStruct &vis,
    const float x0, const float x1,
    const float y0, const float y1,
    const int   nx, const int   ny,
    bool normal_to_positive_z)
{
    VtxIdxRange out;
    out.v0 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i0 = uint32_t(vis.idxData.size());

    uint32_t v0 = out.v0;

    push_back(vis.stride, vis.vtxData, x0, y0, 0.f);
    push_back(vis.stride, vis.vtxData, x1, y0, 0.f);
    push_back(vis.stride, vis.vtxData, x1, y1, 0.f);
    push_back(vis.stride, vis.vtxData, x0, y1, 0.f);

    if(normal_to_positive_z) {
        push_back(0, vis.idxData,  1,2,0);
        push_back(0, vis.idxData,  0,2,3);
    } else {
        push_back(0, vis.idxData,  1,0,2);
        push_back(0, vis.idxData,  0,3,2);
    }

    out.v1 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i1 = uint32_t(vis.idxData.size());
    return out;
}

inline VtxIdxRange append_disk(
    VtxIdxStruct &vis,
    const float x, bool normal_to_positive_x,
    const float r_yz,
    const int   n)
{
    VtxIdxRange out;
    out.v0 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i0 = uint32_t(vis.idxData.size());

    float nx = normal_to_positive_x ? 1.f : -1.f;

    uint32_t icen = out.v0;
    push_back(vis.stride, vis.vtxData, x, 0.f, 0.f);
    setFaceID(vis, false==normal_to_positive_x ? 1 : 2);
    setNormal(vis, nx, 0.f,0.f);

    uint32_t v0 = icen + 1;
    for(int i=0; i<n; i++)
    {
        float angle = 2*3.141592654f *   i / n;
        push_back(vis.stride, vis.vtxData, x, r_yz*sin(angle), r_yz*cos(angle));
        setFaceID(vis, false==normal_to_positive_x ? 1 : 2);
        setNormal(vis, nx, 0.f,0.f);

        // index
        int ixp = (i+n-1) % n;
        if(normal_to_positive_x) {
            push_back(0, vis.idxData, icen, v0+ixp, v0+i);
        } else {
            push_back(0, vis.idxData, icen, v0+i, v0+ixp);
        }
    }

    out.v1 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i1 = uint32_t(vis.idxData.size());
    return out;
}

inline VtxIdxRange append_tube(
    VtxIdxStruct &vis,
    const float x0, const float x1,
    const float r0, const float r1,
    const int   nL/*1*/, const int   nS,
    const bool flipNormal=false)
{
    VtxIdxRange out;
    out.v0 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i0 = uint32_t(vis.idxData.size());

    uint32_t v0 = out.v0;

    for(int i=0; i<nS; i++) {

        float angle = 2*3.141592654f *   i / nS;

        float y  = sinf(angle);
        float z  = cosf(angle);

        for(int j=0; j<(nL+1); j++) {
            float m1 = float(j)/nL;
            float m0 = 1.f - m1;
            float x = x0*m0 + m1*x1; 
            float r = r0*m0 + m1*r1; 

            push_back(vis.stride, vis.vtxData, x, r*y, r*z);     setFaceID(vis, 0);
            if(false==flipNormal) {
                setNormal(vis, 0.f, y, z);
            } else {
                setNormal(vis, 0.f, -y, -z);
            }
        }

        // index
        int ixp = v0 + (nL+1) * ((i+nS-1)%nS);
        int ixc = v0 + (nL+1) * i;
        for(int j=1; j<(nL+1); j++) {
            //  ixp+j-1  ixp+j
            //  ixc+j-1  ixc+j
            if(false==flipNormal) {
                push_back(0, vis.idxData, ixp+j-1, ixc+j-1, ixc+j,
                                          ixp+j-1, ixc+j,   ixp+j);
            } else {
                push_back(0, vis.idxData, ixp+j-1, ixc+j, ixc+j-1,
                                          ixp+j-1, ixp+j, ixc+j );
            }
        }
    }

    out.v1 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i1 = uint32_t(vis.idxData.size());
    return out;
}

inline VtxIdxRange append_ring(
    VtxIdxStruct &vis,
    const float x, bool normal_to_positive_x,
    const float r0, const float r1, const int n)
{
    VtxIdxRange out;
    out.v0 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i0 = uint32_t(vis.idxData.size());

    int nL = 1;
    int nS = n;

    uint32_t v0 = out.v0;

    for(int i=0; i<nS; i++) {

        float angle = 2*3.141592654f *   i / nS;

        float y  = sinf(angle);
        float z  = cosf(angle);

        for(int j=0; j<(nL+1); j++) {
            float m1 = float(j)/nL;
            float m0 = 1.f - m1;
            float r = r0*m0 + m1*r1; 

            push_back(vis.stride, vis.vtxData, x, r*y, r*z);     setFaceID(vis, 0);
            if(false==normal_to_positive_x) {
                setNormal(vis, -1.f, 0.f, 0.f);
            } else {
                setNormal(vis,  1.f, 0.f, 0.f);
            }
        }

        // index
        int ixp = v0 + (nL+1) * ((i+nS-1)%nS);
        int ixc = v0 + (nL+1) * i;
        for(int j=1; j<(nL+1); j++) {
            //  ixp+j-1  ixp+j
            //  ixc+j-1  ixc+j
            if(false==normal_to_positive_x) {
                push_back(0, vis.idxData, ixp+j-1, ixc+j-1, ixc+j,
                                          ixp+j-1, ixc+j,   ixp+j);
            } else {
                push_back(0, vis.idxData, ixp+j-1, ixc+j, ixc+j-1,
                                          ixp+j-1, ixp+j, ixc+j );
            }
        }
    }

    out.v1 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i1 = uint32_t(vis.idxData.size());

    return out;
}

inline VtxIdxRange append_icosahedron(VtxIdxStruct &vis, const float r)
{
    const float X = .525731112119133606f;
    const float Z = .850650808352039932f;
    const float N = 0.f;

    const float vtx[] {
        -X, N, Z,    +X, N, Z,    -X, N,-Z,    +X, N,-Z,    +N, Z, X,    +N, Z,-X,
        +N,-Z, X,    +N,-Z,-X,    +Z, X, N,    -Z, X, N,    +Z,-X, N,    -Z,-X, N
    };
    const uint32_t idx[] {
         0, 4, 1,     0, 9, 4,     9, 5, 4,     4, 5, 8,     4, 8, 1,
         8,10, 1,     8, 3,10,     5, 3, 8,     5, 2, 3,     2, 7, 3,
         7,10, 3,     7, 6,10,     7,11, 6,    11, 0, 6,     0, 1, 6,
         6, 1,10,     9, 0,11,     9,11, 2,     9, 2, 5,     7, 2,11
    };

    VtxIdxRange out;
    out.v0 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i0 = uint32_t(vis.idxData.size());

    uint32_t v0 = out.v0;

    const int nv = sizeof(vtx)/(3*sizeof(vtx[0]));
    for(int i=0; i<nv; i++) {
        push_back(vis.stride, vis.vtxData, r*vtx[i*3], r*vtx[i*3+1], r*vtx[i*3+2]);
    }

    const int ni = sizeof(idx)/(3*sizeof(idx[0]));
    for(int i=0; i<ni; i++) {
        push_back(0, vis.idxData, v0+idx[i*3], v0+idx[i*3+1], v0+idx[i*3+2]);
    }

    out.v1 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i1 = uint32_t(vis.idxData.size());

    return out;
}

inline VtxIdxRange append_sphere_icosahedron(VtxIdxStruct &vis, const float r, const int numSubdivs)
{
    VtxIdxRange out;
    out.v0 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i0 = uint32_t(vis.idxData.size());

    uint32_t v0 = out.v0;

    if(numSubdivs==0) {
        append_icosahedron(vis, r);
    } else {
        VtxIdxStruct visTmp = vis;
        visTmp.vtxData.clear();
        visTmp.idxData.clear();
        append_icosahedron(visTmp, r);
        vector<float> vtx;
        vector<int>   idx;
        for(int i=0; i<numSubdivs; i++) {
            vtx.clear();
            idx.clear();
            subdivide(vtx,idx, visTmp.vtxData, visTmp.idxData, visTmp.stride);
            visTmp.vtxData = vtx;
            visTmp.idxData = idx;
        }
        int n = int(vtx.size()/vis.stride);
        for(int i=0; i<n; i++) {
            float *v = &vtx[i*vis.stride + vis.offset_vertex];
            float d = sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
            float id = 1.f / (d!=0.f ? d : 1.f);
            if(vis.offset_normal >= 0) {
                float *n = &vtx[i*vis.stride + vis.offset_normal];
                n[0] =  v[0] * id;
                n[1] =  v[1] * id;
                n[2] =  v[2] * id;
            }
            float s = r*id;
            v[0] *= s;
            v[1] *= s;
            v[2] *= s;
        }
        append_vtx_idx(vis, vtx,idx);
    }

    out.v1 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i1 = uint32_t(vis.idxData.size());
    return out;
}

inline VtxIdxRange append_box(
    VtxIdxStruct &vis,
    const float x0, const float x1,
    const float y0, const float y1,
    const float z0, const float z1)
{
    VtxIdxRange out;
    out.v0 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i0 = uint32_t(vis.idxData.size());

    uint32_t v = out.v0;

    //-x
    push_back(vis.stride, vis.vtxData, x0, y1, z1);  setFaceID(vis, 0);  setNormal(vis, -1.f, 0.f, 0.f);
    push_back(vis.stride, vis.vtxData, x0, y0, z1);  setFaceID(vis, 0);  setNormal(vis, -1.f, 0.f, 0.f);
    push_back(vis.stride, vis.vtxData, x0, y1, z0);  setFaceID(vis, 0);  setNormal(vis, -1.f, 0.f, 0.f);
    push_back(vis.stride, vis.vtxData, x0, y0, z0);  setFaceID(vis, 0);  setNormal(vis, -1.f, 0.f, 0.f);
    push_back(0, vis.idxData, v+0,v+1,v+2,  v+2,v+1,v+3);
    v+=4;
    //+x
    push_back(vis.stride, vis.vtxData, x1, y1, z1);  setFaceID(vis, 1);  setNormal(vis, +1.f, 0.f, 0.f);
    push_back(vis.stride, vis.vtxData, x1, y1, z0);  setFaceID(vis, 1);  setNormal(vis, +1.f, 0.f, 0.f);
    push_back(vis.stride, vis.vtxData, x1, y0, z1);  setFaceID(vis, 1);  setNormal(vis, +1.f, 0.f, 0.f);
    push_back(vis.stride, vis.vtxData, x1, y0, z0);  setFaceID(vis, 1);  setNormal(vis, +1.f, 0.f, 0.f);
    push_back(0, vis.idxData, v+0,v+1,v+2,  v+2,v+1,v+3);
    v+=4;
    //-y
    push_back(vis.stride, vis.vtxData, x1, y0, z1);  setFaceID(vis, 2);  setNormal(vis, 0.f, -1.f, 0.f);
    push_back(vis.stride, vis.vtxData, x1, y0, z0);  setFaceID(vis, 2);  setNormal(vis, 0.f, -1.f, 0.f);
    push_back(vis.stride, vis.vtxData, x0, y0, z1);  setFaceID(vis, 2);  setNormal(vis, 0.f, -1.f, 0.f);
    push_back(vis.stride, vis.vtxData, x0, y0, z0);  setFaceID(vis, 2);  setNormal(vis, 0.f, -1.f, 0.f);
    push_back(0, vis.idxData, v+0,v+1,v+2,  v+2,v+1,v+3);
    v+=4;
    //+y
    push_back(vis.stride, vis.vtxData, x1, y1, z1);  setFaceID(vis, 3);  setNormal(vis, 0.f, +1.f, 0.f);
    push_back(vis.stride, vis.vtxData, x0, y1, z1);  setFaceID(vis, 3);  setNormal(vis, 0.f, +1.f, 0.f);
    push_back(vis.stride, vis.vtxData, x1, y1, z0);  setFaceID(vis, 3);  setNormal(vis, 0.f, +1.f, 0.f);
    push_back(vis.stride, vis.vtxData, x0, y1, z0);  setFaceID(vis, 3);  setNormal(vis, 0.f, +1.f, 0.f);
    push_back(0, vis.idxData, v+0,v+1,v+2,  v+2,v+1,v+3);
    v+=4;
    //-z
    push_back(vis.stride, vis.vtxData, x0, y0, z0);  setFaceID(vis, 4);  setNormal(vis, 0.f, 0.f, -1.f);
    push_back(vis.stride, vis.vtxData, x1, y0, z0);  setFaceID(vis, 4);  setNormal(vis, 0.f, 0.f, -1.f);
    push_back(vis.stride, vis.vtxData, x0, y1, z0);  setFaceID(vis, 4);  setNormal(vis, 0.f, 0.f, -1.f);
    push_back(vis.stride, vis.vtxData, x1, y1, z0);  setFaceID(vis, 4);  setNormal(vis, 0.f, 0.f, -1.f);
    push_back(0, vis.idxData, v+0,v+1,v+2,  v+2,v+1,v+3);
    v+=4;
    //+z
    push_back(vis.stride, vis.vtxData, x0, y0, z1);  setFaceID(vis, 5);  setNormal(vis, 0.f, 0.f, +1.f);
    push_back(vis.stride, vis.vtxData, x0, y1, z1);  setFaceID(vis, 5);  setNormal(vis, 0.f, 0.f, +1.f);
    push_back(vis.stride, vis.vtxData, x1, y0, z1);  setFaceID(vis, 5);  setNormal(vis, 0.f, 0.f, +1.f);
    push_back(vis.stride, vis.vtxData, x1, y1, z1);  setFaceID(vis, 5);  setNormal(vis, 0.f, 0.f, +1.f);
    push_back(0, vis.idxData, v+0,v+1,v+2,  v+2,v+1,v+3);

    out.v1 = uint32_t(vis.vtxData.size() / vis.stride);
    out.i1 = uint32_t(vis.idxData.size());
    return out;
}

}
