//
// Created by jlhar on 11/27/2025.
//

#ifndef HELLOC_INDEXBUF_HPP
#define HELLOC_INDEXBUF_HPP

#include "common.hpp"

/* Represents an index buffer (IBO). */
class IndexBuf {
public:
    IndexBuf(GLushort *data, int dataSizeBytes);
    ~IndexBuf();

    void BindBuffer();
    void UnbindBuffer();
    int GetCount() { return mCount; }

private:
    GLuint mIbo;
    int mCount;
};

#endif //HELLOC_INDEXBUF_HPP
