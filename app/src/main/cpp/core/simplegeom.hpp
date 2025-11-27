//
// Created by jlhar on 11/27/2025.
//

#ifndef HELLOC_SIMPLEGEOM_HPP
#define HELLOC_SIMPLEGEOM_HPP

#include "indexbuf.hpp"
#include "vertexbuf.hpp"

// Convenience class that represents a geometry in terms of a
// vertex buffer + index buffer.
class SimpleGeom {
public:
    VertexBuf *vbuf;
    IndexBuf *ibuf;

    SimpleGeom() {
        vbuf = NULL;
        ibuf = NULL;
    }

    SimpleGeom(VertexBuf *vb, IndexBuf *ib) {
        vbuf = vb;
        ibuf = ib;
    }

    SimpleGeom(VertexBuf *vb) {
        vbuf = vb;
        ibuf = NULL;
    }

    ~SimpleGeom() {
        if (vbuf) {
            delete vbuf;
        }
        if (ibuf) {
            delete ibuf;
        }
    }
};

#endif //HELLOC_SIMPLEGEOM_HPP
