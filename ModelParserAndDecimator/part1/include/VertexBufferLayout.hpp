#ifndef VERTEX_BUFFER_LAYOUT_HPP
#define VERTEX_BUFFER_LAYOUT_HPP

// The glad library helps setup OpenGL extensions.
#include <glad/glad.h>


class VertexBufferLayout{ 
public:
    // Generates a new buffer
    VertexBufferLayout();
    // Destroys all of our buffers.
    ~VertexBufferLayout();
    // Selects the buffer to bind
    // We only need to bind to a buffer
    // again if we are updating the data.
    void Bind();
    // Unbind our buffers
    void Unbind();

    // Creates a vertex and index buffer object
    // Format is: x,y,z, s,t
    // vcount: the number of vertices
    // icount: the number of indices
    // vdata: A pointer to an array of data for vertices
    // idata: A pointer to an array of data for indices
    // NOTE: Works only for floats--could support other formats.
    //       
    void CreatePositionBufferLayout(unsigned int vcount,unsigned int icount, float* vdata, unsigned int* idata );

    // Creates a vertex and index buffer object
    // Format is: x,y,z, s,t
    void CreateTextureBufferLayout(unsigned int vcount,unsigned int icount, float* vdata, unsigned int* idata );

private:
    // Vertex Array Object
    GLuint m_VAOId;
    // Vertex Buffer
    GLuint m_vertexPositionBuffer;
    // Index Buffer Object
    GLuint m_indexBufferObject;
    // Stride of data (how do I get to the next vertex)
    unsigned int m_stride{0};
};


#endif
