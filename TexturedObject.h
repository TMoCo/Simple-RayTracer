///////////////////////////////////////////////////
//
//  Hamish Carr
//  September, 2020
//
//  ------------------------
//  TexturedObject.h
//  ------------------------
//  
//  Base code for rendering assignments.
//
//  Minimalist (non-optimised) code for reading and 
//  rendering an object file
//  
//  We will make some hard assumptions about input file
//  quality. We will not check for manifoldness or 
//  normal direction, &c.  And if it doesn't work on 
//  all object files, that's fine.
//
//  While I could set it up to use QImage for textures,
//  I want this code to be reusable without Qt, so I 
//  shall make a hard assumption that textures are in 
//  ASCII PPM and use my own code to read them
//  
///////////////////////////////////////////////////

// include guard for TexturedObject
#ifndef _TEXTURED_OBJECT_H
#define _TEXTURED_OBJECT_H

// include the C++ standard libraries we need for the header
#include <vector>
#include <iostream>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// include the unit with Cartesian 3-vectors
#include "Cartesian3.h"
// the render parameters
#include "RenderParameters.h"
// the image class for a texture
#include "RGBAImage.h"
// the header containing the Lights definition
#include "Utils.h"

class TexturedObject
    { // class TexturedObject
    public:
    // vector of vertices
    std::vector<Cartesian3> vertices;

    // vector of normals
    std::vector<Cartesian3> normals;
    
    // vector of texture coordinates (stored as triple to simplify code)
    std::vector<Cartesian3> textureCoords;

    // vector of faces as triangles
    std::vector<Triangle*> faces;

    // vector of materials used in the object
    std::vector<Material*> materials;

    // vector of lights present in the scene
    std::vector<Light*> lights;

    // vector containing the colours used by in the scene
    std::vector<RGBAValue> colours;

    // vector of triangles per original face
    std::vector<unsigned int> faceTriangles;

    std::vector<RGBAImage*> textures;

    // RGBA Image for storing a texture
    RGBAImage texture;

    // a variable to store the texture's ID on the GPU
    GLuint textureID;

    // centre of gravity - computed after reading
    Cartesian3 centreOfGravity;

    // size of object - i.e. radius of circumscribing sphere centred at centre of gravity
    float objectSize;

    // constructor will initialise to safe values
    TexturedObject();
    // destructor will erase the dynamically created vector of triangles
    ~TexturedObject();
    
    // read routine returns true on success, failure otherwise
    bool ReadObjectStream(std::istream &geometryStream, std::istream &textureStream);

    // write routine
    void WriteObjectStream(std::ostream &geometryStream, std::ostream &textureStream);

    // routine to transfer assets to GPU
    void TransferAssetsToGPU();
    
    // routine to render
    void Render(RenderParameters *renderParameters);

    }; // class TexturedObject

// end of include guard for TexturedObject
#endif
