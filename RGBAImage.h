//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  RGBAImage.h
//  ------------------------
//  
//  A minimal class for an image in single-byte RGBA format
//  Optimized for simplicity, not speed or memory
//  With read/write for ASCII RGBA files
//  
///////////////////////////////////////////////////

#ifndef RGBAIMAGE_H
#define RGBAIMAGE_H

#include <iostream>

#include "RGBAValue.h"

// the class itself
class RGBAImage
    { // class RGBAImage
    public:
    //  the raw data
    RGBAValue *block;

    // dimensions of the image
    long width, height;

    // constructor
    RGBAImage();

    // copy constructor
    RGBAImage(const RGBAImage &other);

    // destructor
    ~RGBAImage();
    
    // resizes the image, destroying any contents
    bool Resize(long Width, long Height);

    // indexing - retrieves the beginning of a line
    // array indexing will then retrieve an element
    RGBAValue * operator [](const int rowIndex);
    
    // similar routine for const pointers
    const RGBAValue * operator [](const int rowIndex) const;

    // routine to retrieve an interpolated texel value
    // assumes that u,v coordinates are in range of [0..1]
    // if the flag is not set, it will use nearest neighbour
    RGBAValue GetTexel(float u, float v, bool bilinearFiltering);

    // routines for stream read & write
    bool ReadPPM(std::istream &inStream);
    void WritePPM(std::ostream &outStream);
    
    }; // class RGBAImage




#endif