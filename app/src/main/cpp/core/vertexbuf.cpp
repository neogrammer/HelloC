#include "vertexbuf.hpp"

VertexBuf::VertexBuf(GLfloat* geomData, int dataSize, int stride)
    : VertexBuf(geomData, dataSize, stride, 0, 0) {} // Delegate to new constructor

VertexBuf::VertexBuf(GLfloat* geomData, int dataSize, int stride, int colorsOffset, int texCoordsOffset) {
    MY_ASSERT(dataSize % stride == 0);

    mPrimitive = GL_TRIANGLES;
    mVbo = 0;
    mStride = stride;
    mColorsOffset = colorsOffset;
    mTexCoordsOffset = texCoordsOffset;
    mCount = dataSize / stride;

    // build VBO
    glGenBuffers(1, &mVbo);
    BindBuffer();
    glBufferData(GL_ARRAY_BUFFER, dataSize, geomData, GL_STATIC_DRAW);
    UnbindBuffer();
}

void VertexBuf::BindBuffer() { glBindBuffer(GL_ARRAY_BUFFER, mVbo); }

void VertexBuf::UnbindBuffer() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

VertexBuf::~VertexBuf() {
    glDeleteBuffers(1, &mVbo);
    mVbo = 0;
}
