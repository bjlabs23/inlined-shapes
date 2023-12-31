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

struct VtxIdxStruct
{
    int  stride{3};

    int  offset_vertex{0};
    int  offset_normal{-1};
    int  offset_color {-1};
    int  offset_faceID{-1};
    int  offset_texCoords[8]{0,0,0,0,0,0,0,0};
    int  numTexCoords[8]{0,0,0,0,0,0,0,0};

    inline int getMaxNumTexCoords() const {return sizeof(numTexCoords)/sizeof(numTexCoords[0]);}

    vector<float> vtxData;
    vector<int>   idxData;

    void validate() const {
        int size = 3*int(offset_vertex>=0)
                 +   int(offset_faceID>=0)
                 + 3*int(offset_normal>=0)
                 + 4*int(offset_color >=0);
        for(int i=0; i<sizeof(numTexCoords)/sizeof(numTexCoords[0]); i++) {
            size += numTexCoords[i];
        }
        assert(stride >= size);
    }
};

struct VtxIdxRange
{
    uint32_t v0, v1;
    uint32_t i0, i1;
};

}
