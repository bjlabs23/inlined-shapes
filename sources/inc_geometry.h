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

inline void generate_points(
    VtxIdxStruct &vis,
    const int   n)
{
    vis.validate();

    append_points(vis, n);
}

inline void generate_lines(
    VtxIdxStruct &vis,
    const int   n)
{
    vis.validate();

    append_lines(vis, n);
}

inline void generate_cylinder(
    VtxIdxStruct &vis,
    const float x0, const float x1,
    const float r0, const float r1,
    const int   nL/*1*/, const int   nS)
{
    vis.validate();

    //cylinder
    append_tube(vis, x0,x1,r0,r1,nL,nS);

    // left
    append_disk(vis, x0, false, r0, nS);

    // right
    append_disk(vis, x1, true, r1, nS);
}

inline void generate_hollowCylinder(
    VtxIdxStruct &vis,
    const float x0,  const float x1,
    const float ir0, const float ir1,
    const float or0, const float or1,
    const int   nL/*1*/, const int   nS)
{
    vis.validate();

    //cylinder
    append_tube(vis, x0,x1, or0,or1, nL,nS, false);
    append_tube(vis, x0,x1, ir0,ir1, nL,nS, true);

    // left
    append_ring(vis, x0, false, ir0, or0, nS);

    // right
    append_ring(vis, x1, true, ir1, or1, nS);
}

inline void generate_plane(
    VtxIdxStruct &vis,
    const float x0, const float x1,
    const float y0, const float y1,
    const int   nx, const int   ny)
{
    vis.validate();

    append_plane(vis, x0,x1, y0,y1, nx,ny, true);
}

inline void generate_box(
    VtxIdxStruct &vis,
    const float x0, const float x1,
    const float y0, const float y1,
    const float z0, const float z1,
    const int   nx, const int   ny, const int   nz)
{
    vis.validate();

    append_box(vis, x0,x1, y0,y1, z0,z1);
}

inline void generate_sphere(
    VtxIdxStruct &vis,
    const float r, const int numSubdivs)
{
    vis.validate();

    append_sphere_icosahedron(vis, r, numSubdivs);
}

}
