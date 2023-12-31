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

inline void mul44x3(float y[3], const float A[16], const float x[3])
{
    float *yo, ytmp[3];
    yo = x==y ? ytmp : y;

    yo[0] = A[0] * x[0]  +  A[1] * x[1]  +  A[ 2] * x[2]    +    A[ 3];
    yo[1] = A[4] * x[0]  +  A[5] * x[1]  +  A[ 6] * x[2]    +    A[ 7];
    yo[2] = A[8] * x[0]  +  A[9] * x[1]  +  A[10] * x[2]    +    A[11];

    if(x==y) {y[0] = yo[0];     y[1] = yo[1];     y[2] = yo[2];}
}

inline void rot44x3(float y[3], const float A[16], const float x[3])
{
    float *yo, ytmp[3];
    yo = x==y ? ytmp : y;

    yo[0] = A[0] * x[0]  +  A[1] * x[1]  +  A[ 2] * x[2];
    yo[1] = A[4] * x[0]  +  A[5] * x[1]  +  A[ 6] * x[2];
    yo[2] = A[8] * x[0]  +  A[9] * x[1]  +  A[10] * x[2];

    if(x==y) {y[0] = yo[0];     y[1] = yo[1];     y[2] = yo[2];}
}

inline void transform(
    const float T[16],
    VtxIdxStruct &vis,
    uint32_t v0=0,
    uint32_t nv=-1)
{
    if(nv==-1) nv = uint32_t(vis.vtxData.size() / vis.stride);

    assert(v0+nv <= vis.vtxData.size());

    for(uint32_t vi=0; vi<nv; vi++) {
        uint32_t offset = (v0+vi)*vis.stride;
        auto *pvtx = &vis.vtxData[offset+vis.offset_vertex];
        mul44x3(pvtx, T, pvtx);
        if(vis.offset_normal >= 3) {
            auto *pnml = &vis.vtxData[offset+vis.offset_normal];
            rot44x3(pnml, T, pnml);
        }
    }
}

inline void setVertexColor(
    const float col[4],
    VtxIdxStruct &vis,
    uint32_t v0=0,
    uint32_t nv=-1)
{
    if(nv==-1) nv = uint32_t(vis.vtxData.size() / vis.stride);

    assert(v0+nv<=vis.vtxData.size());

    for(uint32_t vi=0; vi<nv; vi++) {
        uint32_t offset = (v0+vi)*vis.stride + vis.offset_color;
        memcpy(&vis.vtxData[offset], col, 4*sizeof(float));
    }
}

inline void addTexcoords(
    VtxIdxStruct &vis,
    const int texIndex,
    const float x0, const float x1,
    const float y0, const float y1,
    uint32_t v0=0,
    uint32_t nv=-1)
{
    if(nv==-1) nv = uint32_t(vis.vtxData.size() / vis.stride);

    assert(v0+nv<=vis.vtxData.size());
    assert(vis.stride);

    float scale_x = 1.f/(x1-x0);
    float scale_y = 1.f/(y1-y0);

    int off = vis.offset_texCoords[texIndex];
    for(uint32_t vi=0; vi<nv; vi++) {
        int off_x = (v0+vi)*vis.stride;
        int off_u = off_x + off;
        vis.vtxData[off_u  ] = ( (vis.vtxData[off_x  ] - x0)) * scale_x;
//      vis.vtxData[off_u+1] = ( (vis.vtxData[off_x+1] - y0)) * scale_y;
        vis.vtxData[off_u+1] = (-(vis.vtxData[off_x+1] - y1)) * scale_y;
    }
}

inline int nextCorner(const int c) {return ((c/3)*3) + ((c+1)%3);}
inline int prevCorner(const int c) {return ((c/3)*3) + ((c+2)%3);}

inline void buildOpp(
    vector<int>         &opp,
    const vector<int>   &idx)
{
    struct e {
        int64_t key;
        int c;
    };
    vector<e> s;
    s.resize(idx.size());

    for(int c=0; c<(int)idx.size(); c++) {
        int vn = idx[nextCorner(c)];
        int vp = idx[prevCorner(c)];
        if(vn<vp) {
            s[c].key = int64_t(vn) + (int64_t(vp)<<32);
        } else {
            s[c].key = int64_t(vp) + (int64_t(vn)<<32);
        }
        s[c].c = c;
    }

    sort(s.begin(), s.end(), [](const e &a, const e &b)->bool{return a.key < b.key;});

    opp.resize(idx.size());
    memset(&opp[0], 0xFF, opp.size() * sizeof(opp[0]));

    for(int i=1; i<(int)s.size(); i++) {
        if(s[i].key==s[i-1].key) {
            opp[s[i  ].c] = s[i-1].c;
            opp[s[i-1].c] = s[i  ].c;
        }
    }
}

inline void subdivide(
    vector<float> &vtx,
    vector<int>   &idx,
    const vector<float> &vtx0,
    const vector<int>   &idx0,
    const int stride)
{
    vector<int> opp;
    buildOpp(opp, idx0);

    vtx.resize(vtx0.size() + stride*idx0.size());
    for(size_t i=0; i<vtx0.size(); i++) {
        vtx[i] = vtx0[i];
    }

    vector<int> newVe;
    newVe.resize(idx0.size());
    for(size_t i=0; i<newVe.size(); i++) {
        newVe[i] = -1;
    }

    int vei = int(vtx0.size() / stride);
    idx.resize(idx0.size()*4);
    int trNewV[3];
    for(int c=0; c<(int)idx0.size(); c++) {
        int cn = nextCorner(c);
        int cp = prevCorner(c);
        int co = opp[c];
        assert(opp[co]==c);

        if(newVe[c]==-1 && (co==-1 || newVe[co]==-1)) {
                  float *ve = &vtx [     vei*stride];
            const float *vn = &vtx0[idx0[cn]*stride];
            const float *vp = &vtx0[idx0[cp]*stride];
            for(int si=0; si<stride; si++) {
                ve[si] = (vn[si] + vp[si]) * 0.5f;
            }
            newVe[c] = vei;
            if(co>=0) {
                newVe[co] = vei;
            }
            vei++;
        }

        trNewV[c%3] = newVe[c];

        int ni = (c/3) * (3*4);
        if((c%3) == 2) {
            auto v0 = idx0[(c/3)*3  ];  //       v0
            auto v1 = idx0[(c/3)*3+1];  //      /  \     .
            auto v2 = idx0[(c/3)*3+2];  //     /    \    .
            auto e0 = trNewV[0];        //    e2-----e1
            auto e1 = trNewV[1];        //   /  \  /  \  .
            auto e2 = trNewV[2];        //  v1 --e0--- v2

            idx[ni++] = e1;
            idx[ni++] = e2;
            idx[ni++] = e0;

            idx[ni++] = v0;
            idx[ni++] = e2;
            idx[ni++] = e1;

            idx[ni++] = e2;
            idx[ni++] = v1;
            idx[ni++] = e0;

            idx[ni++] = e1;
            idx[ni++] = e0;
            idx[ni++] = v2;
        }
    }
    vtx.resize(vei*stride);
}

}
