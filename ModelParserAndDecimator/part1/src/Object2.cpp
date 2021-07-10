#include "Object.hpp"
#include "Error.hpp"
#include <iostream> 
#include <fstream>
#include <string> 
#include <sstream>
#include <vector>
#include <algorithm>

Object::Object(){
}

Object::~Object(){
    
}

void Object::MakeGeometry(std::string objFileName){

    std::ifstream inFile;
    //Open file
    inFile.open(objFileName);

    m_indices.clear();
    m_vertices.clear();

   //store normal data
   std::vector<float> normalData;
   std::string texFileName;
   std::string mtl_file;

   std::vector<GLfloat> verticesTemp;
   std::vector<std::vector<GLint>> textureIndexes;

    //Get number of vertices and faces
    if(inFile.is_open()){
        std::string line;
        while(getline(inFile, line)){
           if(line.substr(0, 2) == "v "){ // found vertex data
            //Find numbers in line using stringstream
            std::stringstream s; 
            s << line;
            std::string temp_s;
            float temp_i;
            while(!s.eof()){
                s >> temp_s;
                if(std::stringstream(temp_s) >> temp_i) {
                    m_vertices.push_back(temp_i);
                }
                temp_s = ""; 
            }
        }
        else if(line.substr(0, 1) == "f"){ //Found face data
            //Find numbers in line using stringstream
            std::stringstream s; 
            s << line;
            std::string temp_s;
            int temp_i;
            int temp_i2;
            std::vector<GLint> index_temp;
            while(!s.eof()){
                s >> temp_s;
                if(std::stringstream(temp_s) >> temp_i) {
                    //subtract one because objs are 1-indexed, and we want 0-indexed data.
                    m_indices.push_back(temp_i - 1);
                temp_s = ""; 
            }
          }
      }
    }
    }
        //Close file
        inFile.close();

         originalSize = m_indices.size() / 3;
        std::cout << "Number of Triangles in mesh: " << originalSize << "\n";
        longestEdgeLength = findLongestEdge();
        tolerance = longestEdgeLength*0.7;

        // Create a buffer and set the stride of information
        // NOTE: Howe we are leveraging our data structure in order to very cleanly
        //       get information into and out of our data structure.
        m_vertexBufferLayout.CreateTextureBufferLayout(m_vertices.size(),m_indices.size(),m_vertices.data(),m_indices.data());
        
        // Setup shaders
        std::string vertexShader = m_shader.LoadShader("./shaders/vert.glsl");
        std::string fragmentShader = m_shader.LoadShader("./shaders/frag.glsl");
        // Actually create our shader
        m_shader.CreateShader(vertexShader,fragmentShader);
}

void Object::Update(unsigned int screenWidth, unsigned int screenHeight){
        // Make sure we are updating the correct 'buffers'
        m_vertexBufferLayout.Bind(); 
        m_shader.Bind();
 
         // Here we apply the 'view' matrix which creates perspective.
        // The first argument is 'field of view'
        // Then perspective
        // Then the near and far clipping plane.
        // Note I cannot see anything closer than 0.1f units from the screen.
        m_projectionMatrix = glm::perspective(45.0f,(float)screenWidth/(float)screenHeight,0.1f,10.0f);
        // Set the uniforms in our current shader
        m_shader.SetUniformMatrix4fv("modelTransformMatrix",m_transform.GetTransformMatrix());
        m_shader.SetUniformMatrix4fv("projectionMatrix", &m_projectionMatrix[0][0]);
}

void Object::Render(){
        // Select this objects buffer to render
        m_vertexBufferLayout.Bind();
        // Select this objects texture to render
        // m_texture.Bind();
        // Select this objects shader to render
        m_shader.Bind();
	    //Render data
    	glDrawElements(GL_TRIANGLES,
					m_indices.size(),  // The number of indicies, not triangles.
					GL_UNSIGNED_INT,   // Make sure the data type matches
					nullptr);          // Offset pointer to the data. nullptr
                                       // because we are currently bound:
}

// Returns the actual transform stored in our object
// which can then be modified
Transform& Object::GetTransform(){
    return m_transform; 
}

void Object::decimate(){
    bool found = false;
   //find shortest edge and 2 vertices that make it up
   int index1, index2;
   std::vector<int> shortEdge;
   if(m_indices.size()/3 < 15000){
        shortEdge = findShortestEdge();
   }
   else {
       while(!found){
        shortEdge = findShortestEdgeGreedy();
        if(shortEdge.size() != 0){
            found = true;
        }
        else {
            tolerance += 0.2;
        }
       }
   }
   //indicies of the two vertices we will delete and replace with a new one
    index1 = shortEdge.data()[0];
    index2 = shortEdge.data()[1];
 
   // Find midpoint between two vertices
   float x1 = m_vertices.data()[index1*3];
   float y1 = m_vertices.data()[index1*3 + 1];
   float z1 = m_vertices.data()[index1*3 + 2];
 
   float x2 = m_vertices.data()[index2*3];
   float y2 = m_vertices.data()[index2*3 + 1];
   float z2 = m_vertices.data()[index2*3 + 2];
 
   float x = (x1 + x2)/2.0;
   float y = (y1 + y2)/2.0;
   float z = (z1 + z2)/2.0;
 
   // create a new vertex and add to the end of the vertex list
   m_vertices.push_back(x);
   m_vertices.push_back(y);
   m_vertices.push_back(z);
 
   int numDeletableVs = 0;
   int indexPlusOffset;
   int counter = 0;
 
   for(int i = 0; i < m_indices.size(); i++) {
       if(m_indices.data()[i] == index2 || m_indices.data()[i] == index1){
           //replace old vertices connecting removed edge with new vertice at midpoint
           m_indices.at(i) = (m_vertices.size() - 2) / 3;
       }
}

//clean up the indice data by deleting any faces that have 2 of the same vertice
cleanIndiceData();
}
 
//find the length of an edge (distance between two points)
float Object::edgeLength(float x1, float y1, float z1, float x2, float y2, float z2 ) {
   float distance = sqrt(pow(x2 - x1, 2) +
               pow(y2 - y1, 2) +
               pow(z2 - z1, 2) * 1.0);
   return distance;
}
 
//Find smallest edge and return its index
std::vector<int> Object::findShortestEdge(){
   float shortestLength;
   int tracker = 0;
   int vert1, vert2, vert3;
   float x1, y1, z1, x2, y2, z2, x3, y3, z3;
 
   int vertSmall1, vertSmall2;
 
   std::vector<int> output;
   for(int x = 0; x < m_indices.size(); x++){
       if(tracker == 2){
           vert1 = m_indices.data()[x-2];
           vert2 = m_indices.data()[x-1];
           vert3 = m_indices.data()[x];
           x1 = m_vertices.data()[vert1*3];
           y1 = m_vertices.data()[vert1*3 + 1];
           z1 =  m_vertices.data()[vert1*3 + 2];
 
           x2 = m_vertices.data()[vert2*3];
           y2 = m_vertices.data()[vert2*3 + 1];
           z2 =  m_vertices.data()[vert2*3 + 2];
 
           x3 = m_vertices.data()[vert3*3];
           y3 = m_vertices.data()[vert3*3 + 1];
           z3 =  m_vertices.data()[vert3*3 + 2];
 
           //distance between 1st and 2nd vertice
           float d1 = edgeLength(x1, y1, z1, x2, y2, z2);
           //distance between 2nd and 3rd vertice
           float d2 = edgeLength(x2, y2, z2, x3, y3, z3);
           //distance between 1st and 3rd vertice
           float d3 = edgeLength(x1, y1, z1, x3, y3, z3);
 
           tracker = 0;
 
           if(x == 2){
               shortestLength = d1;
               vertSmall1 = vert1;
               vertSmall2 = vert2;
           }

           if( d1 < shortestLength && d1 < d2 && d1 < d3 && vert1 != vert2 ){
               shortestLength = d1;
               vertSmall1 = vert1;
               vertSmall2 = vert2;
           }
           else if(d2 < shortestLength && d2 < d1 && d2 < d3 && vert2 != vert3) {
               shortestLength = d2;
               vertSmall1 = vert2;
               vertSmall2 = vert3;
           }
           else if(d3 < shortestLength && d3 < d1 && d3 < d2 && vert3 != vert1) {
               shortestLength = d3;
               vertSmall1 = vert3;
               vertSmall2 = vert1;
           }
       }
       else {
           tracker += 1;
       }
   }
   output.push_back(vertSmall1);
   output.push_back(vertSmall2);
   return output;
}

//Find smallest edge and return its index
std::vector<int> Object::findShortestEdgeGreedy(){
   float shortestLength;
   int tracker = 0;
   int vert1, vert2, vert3;
   float x1, y1, z1, x2, y2, z2, x3, y3, z3;
 
   int vertSmall1, vertSmall2;
 
   std::vector<int> output;
   for(int x = 0; x < m_indices.size(); x++){
       if(tracker == 2){
           vert1 = m_indices.data()[x-2];
           vert2 = m_indices.data()[x-1];
           vert3 = m_indices.data()[x];
           x1 = m_vertices.data()[vert1*3];
           y1 = m_vertices.data()[vert1*3 + 1];
           z1 =  m_vertices.data()[vert1*3 + 2];
 
           x2 = m_vertices.data()[vert2*3];
           y2 = m_vertices.data()[vert2*3 + 1];
           z2 =  m_vertices.data()[vert2*3 + 2];
 
           x3 = m_vertices.data()[vert3*3];
           y3 = m_vertices.data()[vert3*3 + 1];
           z3 =  m_vertices.data()[vert3*3 + 2];
 
           //distance between 1st and 2nd vertice
           float d1 = edgeLength(x1, y1, z1, x2, y2, z2);
           //distance between 2nd and 3rd vertice
           float d2 = edgeLength(x2, y2, z2, x3, y3, z3);
           //distance between 1st and 3rd vertice
           float d3 = edgeLength(x1, y1, z1, x3, y3, z3);
 
           tracker = 0;
 
           if(x == 2){
               shortestLength = d1;
               vertSmall1 = vert1;
               vertSmall2 = vert2;
           }

           if( d1 < tolerance && d1 < d2 && d1 < d3 && vert1 != vert2 ){
               shortestLength = d1;
               vertSmall1 = vert1;
               vertSmall2 = vert2;
               output.push_back(vertSmall1);
                output.push_back(vertSmall2);
                return output;
           }
           else if(d3 < tolerance && d2 < d1 && d2 < d3 && vert2 != vert3) {
               shortestLength = d2;
               vertSmall1 = vert2;
               vertSmall2 = vert3;
               output.push_back(vertSmall1);
                output.push_back(vertSmall2);
                return output;
           }
           else if(d3 < tolerance && d3 < d1 && d3 < d2 && vert3 != vert1) {
               shortestLength = d3;
               vertSmall1 = vert3;
               vertSmall2 = vert1;output.push_back(vertSmall1);
                output.push_back(vertSmall2);
                return output;
           }
       }
       else {
           tracker += 1;
       }
   }
   if(shortestLength < tolerance){
       output.push_back(vertSmall1);
    output.push_back(vertSmall2);
    return output;
   }
    return output;
}


void Object::cleanIndiceData(){
    //remove from indice data if a face has 2 of the same vertices in a face
    int counter = 0;
    std::vector<GLuint> newIndices;
    for(int i = 0; i < m_indices.size(); i++){
        if(counter == 2){
             if(!(m_indices.data()[i] == m_indices.data()[i - 1] ||
            m_indices.data()[i - 1] == m_indices.data()[i - 2] ||
            m_indices.data()[i] == m_indices.data()[i - 2])){
                newIndices.push_back(m_indices.data()[i - 2]);
                newIndices.push_back(m_indices.data()[i -1]);
                newIndices.push_back(m_indices.data()[i]);
            }
            counter = 0;
          }  
            else {
                counter += 1;
            }
    }
    m_indices = newIndices;
}

//decimate by a percentage of the original mesh
void Object::decimatePercentage(float n){
    float d = 1.0-n;
    if(m_indices.size()/3 >= d*static_cast<float>(getOriginalSize())){
    while(m_indices.size()/3 >= d*static_cast<float>(getOriginalSize())){
        decimate();
    }
     m_vertexBufferLayout.CreateTextureBufferLayout(m_vertices.size(),m_indices.size(),m_vertices.data(),m_indices.data());
      m_vertexBufferLayout.Bind();

      std::cout << "Number of triangles in mesh: " << m_indices.size()/3 << "\n";
    }
}

//Find longest edge and return its length
float Object::findLongestEdge(){
   float longestLength;
   int tracker = 0;
   int vert1, vert2, vert3;
   float x1, y1, z1, x2, y2, z2, x3, y3, z3;
 
   int vertSmall1, vertSmall2;
 
   std::vector<int> output;
   for(int x = 0; x < m_indices.size(); x++){
       if(tracker == 2){
           vert1 = m_indices.data()[x-2];
           vert2 = m_indices.data()[x-1];
           vert3 = m_indices.data()[x];
           x1 = m_vertices.data()[vert1*3];
           y1 = m_vertices.data()[vert1*3 + 1];
           z1 =  m_vertices.data()[vert1*3 + 2];
 
           x2 = m_vertices.data()[vert2*3];
           y2 = m_vertices.data()[vert2*3 + 1];
           z2 =  m_vertices.data()[vert2*3 + 2];
 
           x3 = m_vertices.data()[vert3*3];
           y3 = m_vertices.data()[vert3*3 + 1];
           z3 =  m_vertices.data()[vert3*3 + 2];
 
           //distance between 1st and 2nd vertice
           float d1 = edgeLength(x1, y1, z1, x2, y2, z2);
           //distance between 2nd and 3rd vertice
           float d2 = edgeLength(x2, y2, z2, x3, y3, z3);
           //distance between 1st and 3rd vertice
           float d3 = edgeLength(x1, y1, z1, x3, y3, z3);
 
           tracker = 0;
 
           if(x == 2){
               longestLength = d1;
           }

           if( d1 > longestLength && d1 > d2 && d1 > d3 && vert1 != vert2 ){
               longestLength = d1;
           }
           else if(d2 > longestLength && d2 > d1 && d2 > d3 && vert2 != vert3) {
               longestLength = d2;
           }
           else if(d3 > longestLength && d3 > d1 && d3 > d2 && vert3 != vert1) {
               longestLength = d3;
           }
       }
       else {
           tracker += 1;
       }
   }
   return longestLength;
}
