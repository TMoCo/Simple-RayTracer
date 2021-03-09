//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  Quaternion.cpp
//  ------------------------
//  
//  A class representing a quaternion
//  
//  Note: the emphasis here is on clarity, not efficiency
//  A number of the routines could be implemented more
//  efficiently but aren't
//  
///////////////////////////////////////////////////

#include <math.h>
#include "Quaternion.h"

// constructor
Quaternion::Quaternion()
    { // constructor
    coords[0] = coords[1] = coords[2] = 0.0;
    coords[3] = 1.0;
    } // constructor

// constructor: sets the quaternion to (x, y, z, w)
Quaternion::Quaternion(float x, float y, float z, float w)
    { // constructor
    coords[0] = x;
    coords[1] = y;
    coords[2] = z;
    coords[3] = w;
    } // constructor
    
// Set to a pure scalar value
Quaternion::Quaternion(float scalar)
    { // copy scalar
    // set first three coords to 0.0
    for (int i = 0; i < 3; i++)
        coords[i] = 0.0;
    // and the real part to the scalar
    coords[3] = scalar;
    } // copy scalar

// Set to a pure vector value
Quaternion::Quaternion(const Cartesian3 &vector)
    { // copy vector
    // copy vector part
    for (int i = 0; i < 3; i++)
        coords[i] = vector[i];
    // set the real part to 0.0
    coords[3] = 0.0;
    } // copy vector
    
// Set to a homogeneous point
Quaternion::Quaternion(const Homogeneous4 &point)
    { // copy point
    // just copy the coordinates
    for (int i = 0; i < 4; i++)
        coords[i] = point[i];
    } // copy point
    
// Set to a rotation defined by a rotation matrix
// WARNING: MATRIX MUST BE A VALID ROTATION MATRIX
Quaternion::Quaternion(const Matrix4 &matrix)
    { // copy rotation matrix
    // first, compute the trace of the matrix: the sum of the
    // diagonal elements (see Convert() for coefficients)
    float trace = matrix.coordinates[0][0] + matrix.coordinates[1][1]
        + matrix.coordinates[2][2] + matrix.coordinates[3][3];
    // the trace should now contain 4 (1 - x^2 - y^2 - z^2)
    // and IF it is a pure rotation with no scaling, then
    // this is just 4 (w^2) since we will have a unit quaternion 
    float w = sqrt(trace * 0.25);
    // now we can compute the vector component from symmetric
    // pairs of entries
    // (2yz + 2xw) - (2yz - 2xw) = 4 xw 
    float x = 0.25 * (matrix.coordinates[1][2] - matrix.coordinates[2][1]) / w;
    // (2xz + 2yw) - (2xz - 2yw) = 4 yw 
    float y = 0.25 * (matrix.coordinates[2][0] - matrix.coordinates[0][2]) / w;
    // (2xy + 2zw) - (2xy - 2zw) = 4 zw 
    float z = 0.25 * (matrix.coordinates[0][1] - matrix.coordinates[1][0]) / w;
    // now store them in the appropriate locations
    coords[0] = x;
    coords[1] = y;
    coords[2] = z;
    coords[3] = w;
    } // copy rotation matrix
    
// Set to a rotation defined by an axis and angle
Quaternion::Quaternion(const Cartesian3 &axis, float theta)
    { // Quaternion()
    // convert the axis to a unit vector and multiply by sin theta
    // then add cos theta as a scalar
    (*this) = Quaternion(axis.unit() * sin(theta)) + Quaternion(cos(theta));
    } // Quaternion()

// Copy another Quaternion & return self
Quaternion &Quaternion::operator = (const Quaternion &other)
    { // copy quaternion
    for (int i = 0; i < 4; i++)
        coords[i] = other.coords[i];
    return (*this); 
    } // copy quaternion

// Computes the norm (sum of squares)
float Quaternion::Norm() const
    { // Norm()
    return (coords[0]*coords[0]+coords[1]*coords[1]+
        coords[2]*coords[2]+coords[3]*coords[3]);
    } // Norm()
    
// Reduce to unit quaternion
Quaternion Quaternion::Unit() const
    { // Unit()
    Quaternion result;
    // get the square root of the norm
    float sqrtNorm = Norm();
    // now divide by it
    for (int i = 0; i < 4; i++)
        result.coords[i] = coords[i] / sqrtNorm;
    return result;
    } // Unit()
    
// Conjugate the quaternion
Quaternion Quaternion::Conjugate() const
    { // Conjugate()
    Quaternion result;
    for (int i = 0; i < 3; i++)
        result.coords[i] = coords[i] * -1;
    result.coords[3] = coords[3];
    return result;
    } // Conjugate()
    
// Invert the quaternion
Quaternion Quaternion::Inverse() const
    { // Invert()
    Quaternion result = Conjugate() / Norm();
    return result;
    } // Invert()

// Scalar left-multiplication
Quaternion operator *(float scalar, const Quaternion &quat)
    { // scalar left-multiplication
    Quaternion result;
    for (int i = 0; i < 4; i++)
        result.coords[i] = scalar * quat.coords[i];
    return result;
    } // scalar left-multiplication

// Scalar right-multiplication
Quaternion Quaternion::operator *(float scalar) const
    { // scalar right-multiplication
    Quaternion result;
    for (int i = 0; i < 4; i++)
        result.coords[i] = coords[i] * scalar;
    return result;
    } // scalar right-multiplication
    
// Scalar right-division
Quaternion Quaternion::operator /(float scalar) const
    { // scalar right-division
    Quaternion result;
    for (int i = 0; i < 4; i++)
        result.coords[i] = coords[i] / scalar;
    return result;
    } // scalar right-division

// Adds two quaternions together
Quaternion Quaternion::operator +(const Quaternion &other) const
    { // addition
    Quaternion result;
    for (int i = 0; i < 4; i++)
        result.coords[i] = coords[i] + other.coords[i];
    return result;
    } // addition

// Subtracts one quaternion from another
Quaternion Quaternion::operator -(const Quaternion &other) const
    { // subtraction
    Quaternion result;
    for (int i = 0; i < 4; i++)
        result.coords[i] = coords[i] - other.coords[i];
    return result;
    } // subtraction

// Multiplies two quaternions together
Quaternion Quaternion::operator *(const Quaternion &other) const
    { // multiplication
    Quaternion result;
    // and compute each set of coords   
    result.coords[0] =  + coords[0] * other.coords[3]  // i 1
                        + coords[1] * other.coords[2]  // j k
                        - coords[2] * other.coords[1]  // k j 
                        + coords[3] * other.coords[0]; // 1 i
                
    result.coords[1] =  - coords[0] * other.coords[2]  // i k
                        + coords[1] * other.coords[3]  // j 1
                        + coords[2] * other.coords[0]  // k i 
                        + coords[3] * other.coords[1]; // 1 j
    
    result.coords[2] =  + coords[0] * other.coords[1]  // i j
                        - coords[1] * other.coords[0]  // j i
                        + coords[2] * other.coords[3]  // k 1 
                        + coords[3] * other.coords[2]; // 1 k

    result.coords[3] =  - coords[0] * other.coords[0]  // i i
                        - coords[1] * other.coords[1]  // j j
                        - coords[2] * other.coords[2]  // k k 
                        + coords[3] * other.coords[3]; // 1 1
    return result;
    } // multiplication

// Acts on a vector
Cartesian3 Quaternion::Act(const Cartesian3 &vector) const
    { // Act()
    // compute the result
    Quaternion resultQuat = Inverse() * Quaternion(vector) * (*this);
    Cartesian3 resultVector(resultQuat.coords[0], resultQuat.coords[1], 
        resultQuat.coords[2]);
    // and return the vector
    return resultVector;
    } // Act()
    
// Acts on a homogeneous point
Homogeneous4 Quaternion::Act(const Homogeneous4 &point) const
    { // Act()
    Quaternion resultQuat = Inverse() * Quaternion(point) * (*this);
    Homogeneous4 resultPoint(resultQuat.coords[0], resultQuat.coords[1], 
        resultQuat.coords[2], resultQuat.coords[3]);
    // and return the point
    return resultPoint;
    } // Act()
    
// Returns the angle 2*theta of the action in degrees
float Quaternion::AngleOfAction() const
    { // AngleOfAction()
    float sqrtNorm = sqrt(Norm());
    // normalize, compute arc cosine & return twice the angle
    return (2.0 * acos(coords[3] / sqrtNorm));
    } // AngleOfAction()
    
// Returns the axis of rotation
Cartesian3 Quaternion::AxisOfRotation() const
    { // AxisOfRotation()
    Cartesian3 axis;
    // retrieve the angle of action
    float thetaDeg = AngleOfAction();
    float theta = thetaDeg * 2.0 * M_PI / 360.0;
    // and set the axis by dividing by sin theta
    for (int i = 0; i < 3; i++)
        axis[i] = coords[i] / sin(theta);
    if (theta == 0.0)
        { // no rotation at all - axis unknown
        axis[0] = 1.0f;
        axis[1] = axis[2] = 0.0f;
        } // no rotation at all - axis unknown
    return axis;
    } // AxisOfRotation()

// Converts a quaternion to a rotation matrix
Matrix4 Quaternion::GetMatrix() const
    { // GetMatrix()
    Matrix4 result;
    // a quaternion (x y z w) is equivalent to the following matrix
    // | 1 - 2(y^2+z^2)          2(xy-wz)          2(xz+wy)    0 |
    // |       2(xy+wz)    1 - 2(x^2+z^2)          2(yz-wx)    0 |
    // |       2(xz-wy)          2(yz+wx)    1 - 2(x^2+y^2)    0 |
    // |              0                 0                 0    1 |
    float xx      = coords[0] * coords[0];
    float xy      = coords[0] * coords[1];
    float xz      = coords[0] * coords[2];
    float xw      = coords[0] * coords[3];

    float yy      = coords[1] * coords[1];
    float yz      = coords[1] * coords[2];
    float yw      = coords[1] * coords[3];

    float zz      = coords[2] * coords[2];
    float zw      = coords[2] * coords[3];

    result.coordinates[0][0]  = 1 - 2 * ( yy + zz );
    result.coordinates[0][1]  =     2 * ( xy - zw );
    result.coordinates[0][2]  =     2 * ( xz + yw );
    result.coordinates[0][3]  =     0.0;

    result.coordinates[1][0]  =     2 * ( xy + zw );
    result.coordinates[1][1]  = 1 - 2 * ( xx + zz );
    result.coordinates[1][2]  =     2 * ( yz - xw );
    result.coordinates[1][3]  =     0.0;

    result.coordinates[2][0]  =     2 * ( xz - yw );
    result.coordinates[2][1]  =     2 * ( yz + xw );
    result.coordinates[2][2]  = 1 - 2 * ( xx + yy );
    result.coordinates[2][3]  =     0.0;

    result.coordinates[3][0]  =     0.0;
    result.coordinates[3][1]  =     0.0;
    result.coordinates[3][2]  =     0.0;
    result.coordinates[3][3]  =     1.0;

    return result;
    } // GetMatrix()


// stream input
std::istream & operator >> (std::istream &inStream, Quaternion &quat)
    { // stream input
    inStream >> quat.coords[0] >> quat.coords[1] >> quat.coords[2] >> quat.coords[3];
    return inStream;
    } // stream input

// stream output
std::ostream & operator << (std::ostream &outStream, const Quaternion &quat)
    { // stream output
    outStream << quat.coords[0] << " " << quat.coords[1] << " " << quat.coords[2] << " " << quat.coords[3] << std::endl;
    return outStream;
    } // stream output


