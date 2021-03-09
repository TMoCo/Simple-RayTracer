//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  Matrix4.h
//  ------------------------
//  
//  A minimal class for a homogeneous 4x4 matrix
//  
///////////////////////////////////////////////////

// include guard
#ifndef MATRIX4_H
#define MATRIX4_H

#include <iostream>
#include "Cartesian3.h"
#include "Homogeneous4.h"

// forward declaration
class Matrix4;

#include "Quaternion.h"

// this allows us to get a matrix in the 
// column-major form preferred by OpenGL
class columnMajorMatrix
    { // class columnMajorMatrix
    public:
    float coordinates[16];
    }; // class columnMajorMatrix
    
// the class itself, stored in row-major form
class Matrix4
    { // Matrix4
    public:
    // the coordinates
    float coordinates[4][4];

    // constructor - default to the zero matrix
    Matrix4();
    // copy constructor
    Matrix4(const Matrix4 &other);
    // constructor from a column major matrix
    Matrix4(const float* columCoordinates);
    
    // equality operator
    bool operator ==(const Matrix4 &other) const;

    // indexing - retrieves the beginning of a line
    // array indexing will then retrieve an element
    float * operator [](const int rowIndex);
    
    // similar routine for const pointers
    const float * operator [](const int rowIndex) const;

    // scalar operations
    // multiplication operator (no division operator)
    Matrix4 operator *(float factor) const;

    // vector operations on homogeneous coordinates
    // multiplication is the only operator we use
    Homogeneous4 operator *(const Homogeneous4 &vector) const;

    // and on Cartesian coordinates
    Cartesian3 operator *(const Cartesian3 &vector) const;

    // matrix operations
    // addition operator
    Matrix4 operator +(const Matrix4 &other) const;
    // subtraction operator
    Matrix4 operator -(const Matrix4 &other) const;
    // multiplication operator
    Matrix4 operator *(const Matrix4 &other) const; 
    
    // matrix transpose
    Matrix4 transpose() const;
    
    // returns a column-major array of 16 values
    // for use with OpenGL
    columnMajorMatrix columnMajor() const;

    // methods that set to particular matrices
    void SetZero();
    // the identity matrix
    void SetIdentity();
    void SetTranslation(const Cartesian3 &vector);
    void SetRotation(const Cartesian3 &axis, float theta);
    void SetScale(float xScale, float yScale, float zScale);
    }; // Matrix4

// scalar operations
// additional scalar multiplication operator
Matrix4 operator *(float factor, const Matrix4 &matrix);

// stream input
std::istream & operator >> (std::istream &inStream, Matrix4 &value);

// stream output
std::ostream & operator << (std::ostream &outStream, const Matrix4 &value);
        
#endif
