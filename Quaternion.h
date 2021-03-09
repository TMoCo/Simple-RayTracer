//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  Quaternion.h
//  ------------------------
//  
//  A class representing a quaternion
//  
///////////////////////////////////////////////////

#ifndef __QUATERNION_H__
#define __QUATERNION_H__ 1

#include <stdio.h>
#include "Cartesian3.h"
#include "Homogeneous4.h"

// forward declaration
class Quaternion;

#include "Matrix4.h"

class Quaternion
    { // class Quaternion
    public:
    // first three coordinates are imaginary parts 
    // last coordinate is real part
    Homogeneous4 coords;

    // constructor: sets the quaternion to (0, 0, 0, 1)
    Quaternion();

    // constructor: sets the quaternion to (x, y, z, w)
    Quaternion(float x, float y, float z, float w);

    // Set to a pure scalar value
    Quaternion(float scalar);

    // Set to a pure vector value
    Quaternion(const Cartesian3 &vector);
    
    // Set to a homogeneous point
    Quaternion(const Homogeneous4 &point);
    
    // Set to a rotation defined by a rotation matrix
    // WARNING: MATRIX MUST BE A VALID ROTATION MATRIX
    Quaternion(const Matrix4 &matrix);

    // Set to a rotation defined by an axis and angle
    Quaternion(const Cartesian3 &axis, float theta);

    // Copy another Quaternion & return self
    Quaternion &operator = (const Quaternion &other);
    
    // Computes the norm (sum of squares)
    float Norm() const;
    
    // Reduce to unit quaternion
    Quaternion Unit() const;
    
    // Conjugate the quaternion
    Quaternion Conjugate() const;
    
    // Invert the quaternion
    Quaternion Inverse() const;

    // Scalar right-multiplication
    Quaternion operator *(float scalar) const;

    // Scalar right-division
    Quaternion operator /(float scalar) const;

    // Adds two quaternions together
    Quaternion operator +(const Quaternion &other) const;

    // Subtracts one quaternion from another
    Quaternion operator -(const Quaternion &other) const;

    // Multiplies two quaternions together
    Quaternion operator *(const Quaternion &other) const;

    // Acts on a vector
    Cartesian3 Act(const Cartesian3 &vector) const;
    
    // Acts on a homogeneous point
    Homogeneous4 Act(const Homogeneous4 &point) const;
    
    // Returns the angle 2*theta of the action in degrees
    float AngleOfAction() const;
    
    // Returns the axis of rotation
    Cartesian3 AxisOfRotation() const;
    
    // Converts a quaternion to a rotation matrix
    Matrix4 GetMatrix() const;
    
    }; // class Quaternion

// Scalar left-multiplication
Quaternion operator *(float scalar, const Quaternion &quat);

// stream input
std::istream & operator >> (std::istream &inStream, Quaternion &quat);

// stream output
std::ostream & operator << (std::ostream &outStream, const Quaternion &quat);



#endif
