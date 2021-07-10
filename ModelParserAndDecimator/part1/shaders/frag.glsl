// ==================================================================
#version 330 core
out vec4 color;

in vec3 theColor; 

// Take in our previous texture coordinates from a previous stage
// in the pipeline. In this case, texture coordinates are specified
// on a per-vertex level, so these would be coming in from the vertex
// shader.
// in vec2 v_texCoord; 

// If we have texture coordinates, they are stored in a sampler.
// TODO: Add in the sampler2D

// uniform sampler2D u_Texture;

void main()
{
    // vec4 texColor = texture(u_Texture, v_texCoord);
    // Our final color is now based on the texture.
    color = vec4(theColor, 1.0);
}
// ==================================================================
