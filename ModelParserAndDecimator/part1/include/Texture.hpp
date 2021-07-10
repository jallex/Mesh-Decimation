#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <string>

class Texture{
public:
    // Constructor
    Texture();
    // Destructor
    ~Texture();
	// Loads and sets up an actual texture
    void LoadTexture(const std::string filepath);
	// slot tells us which slot we want to bind to.
    // We can have multiple slots. By default, we
    // will set our slot to 0 if it is not specified.
    void Bind(unsigned int slot=0) const;
    // Be done with our texture
    void Unbind();
	// Loads a PPM from memory.
	void LoadPPM(bool flip);
    // Return the width
    inline int GetWidth(){
        return m_width;
    }
    // Return the height
    inline int GetHeight(){
        return m_height;
    }
    // Bytes per pixel
    inline int GetBPP(){
        return m_BPP;
    }
    // Set a pixel a particular color in our data
    void SetPixel(int x, int y, int r, int g, int b);
    // Display the pixels
    void PrintPixels();

private:
    // Store a unique ID for the texture
    GLuint m_textureID;
	// Filepath to the image loaded
    std::string m_filepath;
	// Raw pixel data
    unsigned char* m_pixelData;
	// Size and format of image
    int m_width{0}; // Width of the image
    int m_height{0}; // Height of the image
    int m_BPP{0};   // Bits per pixel (i.e. how colorful are our pixels)
	std::string m_magicNumber; // magicNumber if any for image format

};



#endif
