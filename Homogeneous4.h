//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  Homogeneous4.cpp
//  ------------------------
//  
//  A minimal class for a 3D point in homogeneous coordinates
//  
///////////////////////////////////////////////////

#ifndef HOMOGENEOUS4_H
#define HOMOGENEOUS4_H

#include <iostream>
#include "Cartesian3.h"

// the class - we will rely on POD for sending to GPU
class Homogeneous4
    { // Homogeneous4
    public:
    // the coordinates
    float x, y, z, w;

    // constructors
    Homogeneous4();
    Homogeneous4(float X, float Y, float Z, float W = 1.0);
    Homogeneous4(const Cartesian3 &other);
    Homogeneous4(const Homogeneous4 &other);
    
    // routine to get a point by perspective division
    Cartesian3 Point() const;

    // routine to get a vector by dropping w (assumed to be 0)
    Cartesian3 Vector() const;

    // addition operator
    Homogeneous4 operator +(const Homogeneous4 &other) const;

    // subtraction operator
    Homogeneous4 operator -(const Homogeneous4 &other) const;
    
    // multiplication operator
    Homogeneous4 operator *(float factor) const;

    // division operator
    Homogeneous4 operator /(float factor) const;

    // operator that allows us to use array indexing instead of variable names
    float &operator [] (const int index);
    const float &operator [] (const int index) const;

    }; // Homogeneous4

// multiplication operator
Homogeneous4 operator *(float factor, const Homogeneous4 &right);

// stream input
std::istream & operator >> (std::istream &inStream, Homogeneous4 &value);

// stream output
std::ostream & operator << (std::ostream &outStream, const Homogeneous4 &value);
        
#endif