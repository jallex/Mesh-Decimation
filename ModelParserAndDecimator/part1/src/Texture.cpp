#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif


#include "Texture.hpp"

#include <stdio.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <memory>

// Default Constructor
Texture::Texture(){

}


// Default Destructor
Texture::~Texture(){
	// Delete our texture from the GPU
	// glDeleteTextures(1,&m_textureID);
	// Delete our pixel data.	
	// Note: We could actually do this sooner
	// in our rendering process.
	if(m_pixelData!=NULL){
		delete[] m_pixelData;
	}
}

void Texture::LoadTexture(const std::string filepath){
	// Set member variable
    m_filepath = filepath;
	// Load our actual image data
	// This method loads .ppm files of pixel data
	LoadPPM(true);
    glEnable(GL_TEXTURE_2D); 
	// Generate a buffer for our texture
    glGenTextures(1,&m_textureID);
    // Similar to our vertex buffers, we now 'select'
    // a texture we want to bind to.
    // Note the type of data is 'GL_TEXTURE_2D'
    glBindTexture(GL_TEXTURE_2D, m_textureID);
	// Now we are going to setup some information about
	// our textures.
	// There are four parameters that must be set.
	// GL_TEXTURE_MIN_FILTER - How texture filters (linearly, etc.)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	// Wrap mode describes what to do if we go outside the boundaries of
	// texture.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	// At this point, we are now ready to load and send some data to OpenGL.
	glTexImage2D(GL_TEXTURE_2D,
							0 ,
						GL_RGB,
						m_width,
						m_height,
							0,
						GL_RGB,
						GL_UNSIGNED_BYTE,
						m_pixelData); // Here is the raw pixel data
	// We are done with our texture data so we can unbind.
	glBindTexture(GL_TEXTURE_2D, 0);
}




// Little function for loading the pixel data
// from a PPM image.
// TODO: Expects a very specific version of PPM!
//
// flip - Will flip the pixels upside down in the data
//        If you use this be consistent.
void Texture::LoadPPM(bool flip){

  // Open an input file stream for reading a file
  std::ifstream ppmFile(m_filepath.c_str());
  // If our file successfully opens, begin to process it.
  if (ppmFile.is_open()){
      // line will store one line of input
      std::string line;
      // Our loop invariant is to continue reading input until
      // we reach the end of the file and it reads in a NULL character
      std::cout << "Reading in ppm file: " << m_filepath << std::endl;
      unsigned int iteration = 0;
      unsigned int pos = 0;
      while ( getline (ppmFile,line) ){
		// Ignore comments in the file
        if (line[0]=='#'){
            continue;
        }else if(line[0]=='P'){
            m_magicNumber = line;
		 }else if(iteration==1){
			// Returns first token 
			char *token = strtok((char*)line.c_str(), " "); 
			m_width = atoi(token);
        	token = strtok(NULL, " ");
			m_height = atoi(token);
			std::cout << "PPM width,height=" << m_width << "," << m_height << "\n";	
			if(m_width > 0 && m_height > 0){
            	m_pixelData = new unsigned char[m_width*m_height*3];
                if(m_pixelData==NULL){
                	std::cout << "Unable to allocate memory for ppm" << std::endl;
                    exit(1);
                 }
             }else{
                std::cout << "PPM not parsed correctly, width and/or height dimensions are 0" << std::endl;
				exit(1);
             }
		 }else if(iteration==2){
			// max color range is stored here
			// TODO: Can be stored optionally	
		 }else{
			m_pixelData[pos] = (unsigned char)atoi(line.c_str());
            ++pos;
		 }
          iteration++;
	}             
    ppmFile.close();
  }
  else{
      std::cout << "Unable to open ppm file:" << m_filepath << std::endl;
  } 

    // Flip all of the pixels
    if(flip){
        // Copy all of the data to a temporary stack-allocated array
        unsigned char copyData[m_width*m_height*3];
        for(int i =0; i < m_width*m_height*3; ++i){
            copyData[i]=m_pixelData[i];
        }
        //memcpy(copyData,m_pixelData,(m_width*m_height*3)*sizeof(unsigned char));
        unsigned int pos = (m_width*m_height*3)-1;
        for(int i =0; i < m_width*m_height*3; i+=3){
            m_pixelData[pos]=copyData[i+2];
            m_pixelData[pos-1]=copyData[i+1];
            m_pixelData[pos-2]=copyData[i];
            pos-=3;
        }
    }


}

/*  ===============================================
Desc: Sets a pixel in our array a specific color
Precondition: 
Postcondition:
=============================================== */ 
void Texture::SetPixel(int x, int y, int r, int g, int b){
  if(x > m_width || y > m_height){
    return;
  }
  else{
    /*std::cout << "modifying pixel at " 
              << x << "," << y << "from (" <<
              (int)color[x*y] << "," << (int)color[x*y+1] << "," <<
(int)color[x*y+2] << ")";*/

    m_pixelData[(x*3)+m_height*(y*3)] = r;
    m_pixelData[(x*3)+m_height*(y*3)+1] = g;
    m_pixelData[(x*3)+m_height*(y*3)+2] = b;

/*    std::cout << " to (" << (int)color[x*y] << "," << (int)color[x*y+1] << ","
<< (int)color[x*y+2] << ")" << std::endl;*/

  }
}

/*  ===============================================
Desc: 
Precondition: 
Postcondition:
=============================================== */ 
void Texture::PrintPixels(){
    for(int x = 0; x <  m_width*m_height*3; ++x){
        std::cout << " " << (int)m_pixelData[x];
    }
    std::cout << "\n";

}
// slot tells us which slot we want to bind to.
// We can have multiple slots. By default, we
// will set our slot to 0 if it is not specified.
void Texture::Bind(unsigned int slot) const{
	// Using OpenGL 'state' machine we set the active texture
	// slot that we want to occupy. Again, there could
	// be multiple at once.
	// At the time of writing, OpenGL supports 8-32 depending
	// on your hardware.
    glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0+slot);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind(){
	glBindTexture(GL_TEXTURE_2D, 0);
}


