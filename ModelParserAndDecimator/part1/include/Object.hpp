#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <glad/glad.h>

#include <vector>
#include <string>

#include "Shader.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"
#include "Transform.hpp"

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Purpose:
// An abstraction to create multiple objects
//
//
class Object{
public:
    // Object Constructor
    Object();
    // Object destructor
    ~Object();
    // Create textured geometry
    void MakeGeometry(std::string objFileName);
    // Updates and transformatinos applied to object
    void Update(unsigned int screenWidth, unsigned int screenHeight);
    // How to draw the object
    void Render();
    //return indice data
    std::vector<GLuint> getIndiceData() {return m_indices;}
    // Returns an objects transform
    Transform& GetTransform();
        inline int getOriginalSize() const { return originalSize; }
    //perform decimate once
    void decimate();
    //find the length of an edge (distance between two points)
    float edgeLength(float x1, float y1, float z1, float x2, float y2, float z2 );
    // return index of shortest edge
    std::vector<int> findShortestEdge();
    // find shortest edge, using the greedy method
    std::vector<int> findShortestEdgeGreedy();
    //clean up indice data 
    void cleanIndiceData();
    //repeatedly decimate until we've reduced the mesh by a specified percentage
    void decimatePercentage(float n);
    //return the length of the longest edge
    float findLongestEdge();
private:
    // Object vertices
    std::vector<GLfloat> m_vertices;
    // Object indicies
    std::vector<GLuint> m_indices;
    // For now we have one shader per object.
    Shader m_shader;
    // For now we have one buffer per object.
    VertexBufferLayout m_vertexBufferLayout;
    // Store the objects transformations
    Transform m_transform; 
    // Store the 'camera' projection
    glm::mat4 m_projectionMatrix;
        //original number of faces 
    int originalSize;
    //the length of the longest edge
    float longestEdgeLength;
    //the tolerance
    float tolerance;
};

#endif
