//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  Cartesian3.h
//  ------------------------
//  
//  A minimal class for a point in Cartesian space
//  
///////////////////////////////////////////////////

#ifndef CARTESIAN3_H
#define CARTESIAN3_H

#include <iostream>

// the class - we will rely on POD for sending to GPU
class Cartesian3
    { // Cartesian3
    public:
    // the coordinates
    float x, y, z;

    // constructors
    Cartesian3();
    Cartesian3(float X, float Y, float Z);
    Cartesian3(const Cartesian3 &other);
    
    // equality operator
    bool operator ==(const Cartesian3 &other) const;

    // addition operator
    Cartesian3 operator +(const Cartesian3 &other) const;

    // subtraction operator
    Cartesian3 operator -(const Cartesian3 &other) const;
    
    // multiplication operator
    Cartesian3 operator *(float factor) const;

    // division operator
    Cartesian3 operator /(float factor) const;

    // unary minus operator
    Cartesian3 operator-() const;

    // dot product routine
    float dot(const Cartesian3 &other) const;

    // cross product routine
    Cartesian3 cross(const Cartesian3 &other) const;
    
    // routine to find the length
    float length() const;
    
    // normalisation routine
    Cartesian3 unit() const;
    
    // operator that allows us to use array indexing instead of variable names
    float &operator [] (const int index);
    const float &operator [] (const int index) const;

    }; // Cartesian3

// multiplication operator
Cartesian3 operator *(float factor, const Cartesian3 &right);

// stream input
std::istream & operator >> (std::istream &inStream, Cartesian3 &value);

// stream output
std::ostream & operator << (std::ostream &outStream, const Cartesian3 &value);
        
#endif