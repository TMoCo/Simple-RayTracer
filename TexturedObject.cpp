///////////////////////////////////////////////////
//
//  Hamish Carr
//  September, 2020
//
//  ------------------------
//  TexturedObject.cpp
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

// include the header file
#include "TexturedObject.h"

// include the C++ standard libraries we want
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>

// include the Cartesian 3- vector class
#include "Cartesian3.h"

#define MAXIMUM_LINE_LENGTH 1024

// constructor will initialise to safe values
TexturedObject::TexturedObject()
    : centreOfGravity(0.0,0.0,0.0)
    { // TexturedObject()
    // force arrays to size 0
    vertices.resize(0);
    normals.resize(0);
    textureCoords.resize(0);
    faces.resize(0);
    faceTriangles.resize(0);
    colours.resize(1);
    // set a default colour
    colours[0] = RGBAValue(178.5F, 178.5F, 178.5F, 255.0F);
    // resize materials with a default material
    materials.resize(1);
    materials[0] = new Material();
    lights.resize(0);
    textures.resize(0);
    } // TexturedObject()

TexturedObject::~TexturedObject() {
    //faces.clear();
}

// read routine returns true on success, failure otherwise
bool TexturedObject::ReadObjectStream(std::istream &geometryStream, std::istream &textureStream)
    { // ReadObjectStream()
    
    // create a read buffer
    char readBuffer[MAXIMUM_LINE_LENGTH];

    // triangle id
    unsigned int id = 0;
    // default area light intensity
    RGBRadiance areaLightIntensity;
    // current colour
    unsigned int currentColour = 0;
    // current material is set to the default starting one
    unsigned int currentMaterial = 0;
    // current texture, 0 means "don't use texture"
    unsigned int currentTexture = 0;
    // current light id
    unsigned int lightId = 1;

    // the rest of this is a loop reading lines & adding them in appropriate places
    while (true)
        { // not eof
        // character to read
        char firstChar = geometryStream.get();
        
        // check for eof() in case we've run out
        if (geometryStream.eof())
            break;

        // otherwise, switch on the character we read
        switch (firstChar)
            { // switch on first character
            case '#':       // comment line
                // read and discard the line
                geometryStream.getline(readBuffer, MAXIMUM_LINE_LENGTH);
                break;

            case 'c': {     // colour line
                RGBAValue newColour;
                geometryStream >> newColour;
                newColour.alpha = 255;
                colours.push_back(newColour);
                currentColour += 1;
                break;
            }

            // texture entry
            case 't': {
                // get the next char to determine how to process line further
                char secondChar = geometryStream.get();
                
                // bail if we ran out of file
                if (geometryStream.eof())
                    break;

                switch(secondChar) {
                    // make a new texture object
                    case 'm': {
                        // chomp the first space
                        geometryStream.get();
                        // read in the texture location
                        geometryStream.getline(readBuffer, MAXIMUM_LINE_LENGTH);
                        // initialise the texture object
                        std::ifstream textureFile(readBuffer);
                        //std::cout << readBuffer << std::endl;
                        // create a texture object
                        RGBAImage* newTexture = new RGBAImage();
                        // if we can read the texture, then we add it to the textures
                        // list
                        if (textureFile.good() && newTexture->ReadPPM(textureFile)) {
                            //std::cout << "read succesful" << std::endl;
                            //std::cout << newTexture->width << std::endl;
                            //std::cout << newTexture->height << std::endl;
                            textures.push_back(newTexture);
                        }
                        break;
                    }
                    // use a texture
                    case 'u': {
                        // cast to signed int to make sure we detect negative input
                        int texture;
                        geometryStream >> texture;
                        //std::cout << "use " << texture << std::endl;
                        // which is caught here to avoid indexing errors
                        if ((texture-1 < 0) || (texture > textures.size()))
                            currentTexture = 0;
                        else
                            currentTexture = texture;
                        break;
                    }
                    // stop using a texture
                    case 's': 
                        //std::cout << "stop using " << currentTexture << std::endl;
                        currentTexture = 0;
                        break;
                }
                break;
            }

            // light section
            case 'l': {
                // get the next char to determine how to process line further
                char secondChar = geometryStream.get();
                // bail if we ran out of file
                if (geometryStream.eof())
                    break;

                switch (secondChar) {
                    case 'p': {
                        // create a light object and stream in light data depending on next character
                        Light* light = new Light();
                        // start with position
                        geometryStream >> light->position.x;
                        geometryStream >> light->position.y;
                        geometryStream >> light->position.z;
                        // follow with intensity
                        float intensity;
                        geometryStream >> intensity;
                        RGBRadiance lightIntensity = RGBRadiance(intensity, intensity, intensity);
                        light->intensity = lightIntensity;
                        // end with light type
                        geometryStream >> light->atInfinity;
                        light->isAreaLight = false;
                        // then push back the light
                        lights.push_back(light);
                        break;
                    }
                    // a triangle that is part of an area light
                    case 'f': {
                        // create a light object and stream in light data depending on next character
                        Light* light = new Light();
                        // set boolean flags
                        light->atInfinity = false;
                        light->isAreaLight = true; 
                        // stream in the face that constitutes an area light
                        geometryStream.getline(readBuffer, MAXIMUM_LINE_LENGTH);
                        // turn into a C++ string
                        std::string lineString = std::string(readBuffer);
                        // create a string stream
                        std::stringstream lineParse(lineString); 
                        // parse in the face
                        Triangle* triangle = new Triangle();
                        unsigned int v = 0;
                        unsigned int vertexID;
                        while (!lineParse.eof()) {
                            lineParse >> vertexID;
                            triangle->vertices[v] = vertexID-1;
                            triangle->texCoords[v] = vertexID-1;
                            triangle->normals[v++] = vertexID-1;
                        }
                        // set the colour to the current one
                        triangle->colour = currentColour;
                        // set the triangle material to the current one
                        triangle->material = currentMaterial;
                        // set the triangle material to the current one
                        triangle->texID = currentTexture;
                        // set the triangle's id
                        triangle->id = id++;
                        // set the triangle to the light
                        faces.push_back(triangle);
                        light->triangle = triangle;
                        light->intensity = areaLightIntensity;
                        // keep track of area light ids
                        triangle->lightId = lightId++;
                        std::cout << "new light with id " << + triangle->lightId << '\n';
                        lights.push_back(light);
                        break;
                    }
                    // set the new area light intensity, where xyz are RGB
                    case 'a': 
                        geometryStream >> areaLightIntensity.red_;
                        geometryStream >> areaLightIntensity.green_;
                        geometryStream >> areaLightIntensity.blue_;
                }
                break;                
            }

            // material section
            case 'm' :{
                // get the next char to determine how to process line further
                char secondChar = geometryStream.get();
                
                // bail if we ran out of file
                if (geometryStream.eof())
                    break;

                switch (secondChar) {
                    // create a new material object 
                    case 'c': {
                        Material* material = new Material();
                        materials.push_back(material);
                        // also increment count so setting 
                        // material properties is for right material
                        currentMaterial += 1;
                        break;
                    }
                    // set current material emissive property
                    case 'e': 
                        geometryStream >> materials[currentMaterial]->emmisive[0];
                        geometryStream >> materials[currentMaterial]->emmisive[1];
                        geometryStream >> materials[currentMaterial]->emmisive[2];
                        break;
                    // set material lambertian property
                    case 'l': 
                        geometryStream >> materials[currentMaterial]->lambertian[0];
                        geometryStream >> materials[currentMaterial]->lambertian[1];
                        geometryStream >> materials[currentMaterial]->lambertian[2];
                        break;
                    // set material glossy property
                    case 'g': 
                        geometryStream >> materials[currentMaterial]->glossy[0];
                        geometryStream >> materials[currentMaterial]->glossy[1];
                        geometryStream >> materials[currentMaterial]->glossy[2];
                        geometryStream >> materials[currentMaterial]->glossy[3];
                        break;
                    case 'i': 
                        geometryStream >> materials[currentMaterial]->albedo[0];
                        geometryStream >> materials[currentMaterial]->albedo[1];
                        geometryStream >> materials[currentMaterial]->albedo[2];
                        break;
                    // set the material extinction coefficient
                    case 'x':
                        geometryStream >> materials[currentMaterial]->extinction;
                        break;
                    case 'I':
                        geometryStream >> materials[currentMaterial]->extinction;
                        break;
                    
                    // set current material
                    case 'u': {
                        // cast to signed int to make sure we detect negative input
                        int material;
                        geometryStream >> material;
                        // which is caught here to avoid indexing errors
                        if ((material-1 < 0) || (material > materials.size()))
                            currentMaterial = 0;
                        else
                            currentMaterial = material;
                        break;
                    }
                }
                break;
            }
                
            case 'v':       // vertex data of some type
                { // some sort of vertex data
                // retrieve another character
                char secondChar = geometryStream.get();
                
                // bail if we ran out of file
                if (geometryStream.eof())
                    break;

                // now use the second character to choose branch
                switch (secondChar)
                    { // switch on second character
                    case ' ':       // space - indicates a vertex
                        { // vertex read
                        Cartesian3 vertex;
                        geometryStream >> vertex;
                        vertices.push_back(vertex);
                        break;
                        } // vertex read
                    case 'n':       // n indicates normal vector
                        { // normal read
                        Cartesian3 normal;
                        geometryStream >> normal;
                        normals.push_back(normal);
                        break;
                        } // normal read
                    case 't':       // t indicates texture coords
                        { // tex coord
                        Cartesian3 texCoord;
                        geometryStream >> texCoord;
                        textureCoords.push_back(texCoord);
                        break;                  
                        } // tex coord
                    default:
                        break;
                    } // switch on second character 
                break;
                } // some sort of vertex data
                
            case 'f':       // face data
                { // face
                // a face can have an arbitrary number of vertices, which is a pain
                // so we will create a separate buffer to read from
                geometryStream.getline(readBuffer, MAXIMUM_LINE_LENGTH);

                // turn into a C++ string
                std::string lineString = std::string(readBuffer);

                // create a string stream
                std::stringstream lineParse(lineString); 

                // create vectors for the IDs (with different names from the master arrays)
                std::vector<unsigned int> faceVertexSet;
                std::vector<unsigned int> faceNormalSet;
                std::vector<unsigned int> faceTexCoordSet;

                // now loop through the line
                while (!lineParse.eof())
                    { // lineParse isn't done
                    // the triple of vertex, normal, tex coord IDs
                    unsigned int vertexID;
                    unsigned int normalID;
                    unsigned int texCoordID;

                    // try reading them in, breaking if we hit eof
                    lineParse >> vertexID;
                    // retrieve & discard a slash
                    lineParse.get();
                    // check eof
                    if (lineParse.eof())
                        break;
                    
                    // and the tex coord
                    lineParse >> texCoordID;
                    lineParse.get();
                    if (lineParse.eof())
                        break;
                        
                    // read normal likewise
                    lineParse >> normalID;
                        
                    // if we got this far, we presumably have three valid numbers, so add them
                    // but notice that .obj uses 1-based numbering, where our arrays use 0-based
                    faceVertexSet.push_back(vertexID-1);
                    faceNormalSet.push_back(normalID-1);
                    faceTexCoordSet.push_back(texCoordID-1);
                    } // lineParse isn't done

                // as long as the face has at least three vertices, add to the master list
                if (faceVertexSet.size() > 2)
                    { // at least 3

                    // get the number of triangles we can divide the face up in to
                    unsigned int nTriangles = faceVertexSet.size() - 2;
                    // loop over the triangles and create a triangle for each one
                    for (unsigned int tri = 0; tri < nTriangles; tri++) {
                        Triangle* triangle = new Triangle();
                        // fan out starting at the first vertex
                        // v1
                        triangle->vertices[0] = faceVertexSet[0];
                        triangle->texCoords[0] = faceTexCoordSet[0];
                        triangle->normals[0] = faceNormalSet[0];
                        // then set the next two vertices, incrementing for each 
                        // new triangle
                        // v2
                        triangle->vertices[1] = faceVertexSet[tri + 1];
                        triangle->texCoords[1] = faceTexCoordSet[tri + 1];
                        triangle->normals[1] = faceNormalSet[tri + 1];
                        // v3
                        triangle->vertices[2] = faceVertexSet[tri + 2];
                        triangle->texCoords[2] = faceTexCoordSet[tri + 2];
                        triangle->normals[2] = faceNormalSet[tri + 2];
                        // set the colour to the current one
                        triangle->colour = currentColour;
                        // set the triangle material to the current one
                        triangle->material = currentMaterial;
                        // set the triangle material to the current one
                        triangle->texID = currentTexture;
                        // set the triangle's id
                        triangle->id = id++;
                        // append the triangle to the triangles vector
                        faces.push_back(triangle);
                    }
                    // keep the number of triangles in a face for writing object
                    // to file
                    faceTriangles.push_back(nTriangles);
                    } // at least 3
                
                break;
                } // face
                
            // default processing: do nothing
            default:
                break;

            } // switch on first character

        } // not eof

    // compute centre of gravity
    // note that very large files may have numerical problems with this
    centreOfGravity = Cartesian3(0.0, 0.0, 0.0);

    // if there are any vertices at all
    if (vertices.size() != 0)
        { // non-empty vertex set
        // sum up all of the vertex positions
        for (unsigned int vertex = 0; vertex < vertices.size(); vertex++)
            centreOfGravity = centreOfGravity + vertices[vertex];
        
        // and divide through by the number to get the average position
        // also known as the barycentre
        centreOfGravity = centreOfGravity / vertices.size();

        // start with 0 radius
        objectSize = 0.0;

        // now compute the largest distance from the origin to a vertex
        for (unsigned int vertex = 0; vertex < vertices.size(); vertex++)
            { // per vertex
            // compute the distance from the barycentre
            float distance = (vertices[vertex] - centreOfGravity).length();         
            
            // now test for maximality
            if (distance > objectSize)
                objectSize = distance;
            } // per vertex
        } // non-empty vertex set

    // now read in the texture file
    texture.ReadPPM(textureStream);

    // return a success code
    return true;
    } // ReadObjectStream()

// write routine
void TexturedObject::WriteObjectStream(std::ostream &geometryStream, std::ostream &textureStream)
    { // WriteObjectStream()
    // output the vertex coordinates
    for (unsigned int vertex = 0; vertex < vertices.size(); vertex++)
        geometryStream << "v  " << std::fixed << vertices[vertex] << std::endl;
    geometryStream << "# " << vertices.size() << " vertices" << std::endl;
    geometryStream << std::endl;

    // and the normal vectors
    for (unsigned int normal = 0; normal < normals.size(); normal++)
        geometryStream << "vn " << std::fixed << normals[normal] << std::endl;
    geometryStream << "# " << normals.size() << " vertex normals" << std::endl;
    geometryStream << std::endl;

    // and the texture coordinates
    for (unsigned int texCoord = 0; texCoord < textureCoords.size(); texCoord++)
        geometryStream << "vt " << std::fixed << textureCoords[texCoord] << std::endl;
    geometryStream << "# " << textureCoords.size() << " texture coords" << std::endl;
    geometryStream << std::endl;


    // the face triangles vector's size is the original number of faces, so we
    // loop over it when writing, accessing face triangle data accordingly
    unsigned int currTriangle = 0;
    for (unsigned int face = 0; face < faceTriangles.size(); face++) {
        geometryStream << "f ";
        // add the first two vertices vertex of the face
        for (unsigned int i = 0; i < 2; i++)
            geometryStream 
                << faces[currTriangle]->vertices[i]+1 << "/" 
                << faces[currTriangle]->texCoords[i]+1 << "/" 
                << faces[currTriangle]->normals[i]+1 << " ";
        // loop for each triangle in the face, add the third vertex
        for (unsigned int tri = 0; tri < faceTriangles[face]; tri++) {
            geometryStream 
                << faces[currTriangle]->vertices[2]+1 << "/" 
                << faces[currTriangle]->texCoords[2]+1 << "/" 
                << faces[currTriangle]->normals[2]+1 << " ";
            // increment current triangle counter
            currTriangle++;
        }
        geometryStream << std::endl;
    }
    geometryStream << "# " << faceTriangles.size() << " polygons" << std::endl;
    
    // now output the texture
    texture.WritePPM(textureStream);
    } // WriteObjectStream()

// routine to transfer assets to GPU
void TexturedObject::TransferAssetsToGPU()
    { // TransferAssetsToGPU()
    // when this is called, it transfers assets to the GPU.
    // for now, it will only be to transfer the texture
    // this may not be efficient, but it supports arbitrary sizes best
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // create a texture ID (essentially a pointer)
    glGenTextures(1, &textureID);
    // now bind to it - i.e. all following code addresses this one
    glBindTexture(GL_TEXTURE_2D, textureID);
    // set these parameters to avoid dealing with mipmaps 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // now transfer the image
    glTexImage2D(   
        GL_TEXTURE_2D,      // it's a 2D texture
        0,                  // mipmap level of 0 (ie the largest one)
        GL_RGBA,            // we want the data stored as RGBA on GPU
        texture.width,      // width of the image
        texture.height,     // height of the image
        0,                  // width of border (in texels)
        GL_RGBA,            // format the data is stored in on CPU
        GL_UNSIGNED_BYTE,   // data type
        texture.block       // and a pointer to the data
        );
    } // TransferAssetsToGPU()

// routine to render
void TexturedObject::Render(RenderParameters *renderParameters)
    { // Render()
    // Ideally, we would apply a global transformation to the object, but sadly that breaks down
    // when we want to scale things, as unless we normalise the normal vectors, we end up affecting
    // the illumination.  Known solutions include:
    // 1.   Normalising the normal vectors
    // 2.   Explicitly dividing the normal vectors by the scale to balance
    // 3.   Scaling only the vertex position (slower, but safer)
    // 4.   Not allowing spatial zoom (note: sniper scopes are a modified projection matrix)
    //
    // Inside a game engine, zoom usually doesn't apply. Normalisation of normal vectors is expensive,
    // so we will choose option 2.  

    // if we have texturing enabled . . . 
    if (renderParameters->texturedRendering)
        { // textures enabled
        // enable textures
        glEnable(GL_TEXTURE_2D);
        // use our other flag to specify replace or modulate
        if (renderParameters->textureModulation)
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        else
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        // now bind the texture ID
        glBindTexture(GL_TEXTURE_2D, textureID);
        } // textures enabled
    else
        { // textures disabled
        // make sure that they are disabled
        glDisable(GL_TEXTURE_2D);
        } // textures disabled

    // Scale defaults to the zoom setting
    float scale = renderParameters->zoomScale;
    
    // if object scaling is requested, apply it as well 
    if (renderParameters->scaleObject)
        scale /= objectSize;
        
    //  now scale everything
//     glScalef(scale, scale, scale);

    // apply the translation to the centre of the object if requested
    if (renderParameters->centreObject)
        glTranslatef(-centreOfGravity.x * scale, -centreOfGravity.y * scale, -centreOfGravity.z * scale);

    // emissive glow from object
    float emissiveColour[4];
    // default ambient / diffuse / specular colour
    float surfaceColour[4] = { 0.7, 0.7, 0.7, 1.0 };
    // specular shininess
    float shininess[4];
    // copy the intensity into RGB channels
    emissiveColour[0]   = emissiveColour[1] = emissiveColour[2] = renderParameters->emissiveLight;
    emissiveColour[3]   = 1.0; // don't forget alpha
    
    // set the shininess from the specular exponent
    shininess[0]        = shininess[1]      = shininess[2]      = renderParameters->specularExponent;
    shininess[3]        = 1.0; // alpha

    // start rendering
    glBegin(GL_TRIANGLES);

    // we assume a single material for the entire object
    glMaterialfv(GL_FRONT, GL_EMISSION, emissiveColour);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, surfaceColour);
    glMaterialfv(GL_FRONT, GL_SPECULAR, surfaceColour);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    
    // repeat this for colour - extra call, but saves if statements

    for (unsigned int face = 0; face < faces.size(); face++) {
        // set colour
        glColor3f(
            colours[faces[face]->colour].red, 
            colours[faces[face]->colour].green,
            colours[faces[face]->colour].blue);
        // then set vertices
        for (unsigned int i = 0; i < 3; i++) {
            glNormal3f(
                normals[faces[face]->normals[i]].x, 
                normals[faces[face]->normals[i]].y,
                normals[faces[face]->normals[i]].z);
            /*
            if (renderParameters->mapUVWToRGB) { // set colour and material
                // cast Cartesian3 to float and use pointer to access data
                float *colourPointer = (float *) &(textureCoords[faces[face]->texCoords[i]]);
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colourPointer);
                glMaterialfv(GL_FRONT, GL_SPECULAR, colourPointer);
                glColor3fv(colourPointer);
            }
            */
            glTexCoord2f(
                textureCoords[faces[face]->texCoords[i]].x,
                textureCoords[faces[face]->texCoords[i]].y);
            glVertex3f(
                scale * vertices[faces[face]->vertices[i]].x,
                scale * vertices[faces[face]->vertices[i]].y,
                scale * vertices[faces[face]->vertices[i]].z);
        }
        
    }

    // close off the triangles
    glEnd();

    // if we have texturing enabled, turn texturing back off 
    if (renderParameters->texturedRendering)
        glDisable(GL_TEXTURE_2D);

    } // Render()

