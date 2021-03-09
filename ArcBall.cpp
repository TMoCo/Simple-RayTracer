//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  ArcBall.cpp
//  ------------------------
//  
//  A logical arcball (UI is separate)
//  
///////////////////////////////////////////////////


#include "ArcBall.h"
#include <math.h>

// constructor - initializes to a zero rotation
ArcBall::ArcBall()
    { // constructor
    // set to (0, 0, 0, 1) - no rotation
    baseRotation = Quaternion(0, 0, 0, 1);
    // set to (0, 0, 0, 1) - no rotation
    currentRotation = Quaternion(0, 0, 0, 1);
    // set dragLast to something predictable
    dragFrom = Quaternion(0, 0, 1, 0);
    } // constructor

// convert an (x,y) point to a quaternion
Quaternion ArcBall::FindQuat(float x, float y)
    { // FindQuat()
    Quaternion result;
    Cartesian3 resultVector;
    float dSquared = x*x + y*y;
    if (dSquared > 1.0)
        // outside the circle, so
        // constrain to the circle with scaling
        resultVector = Cartesian3(x / sqrt(dSquared), y / sqrt(dSquared), 0.0);
    else
        // inside the circle, so 1-dSquared = z squared
        resultVector = Cartesian3(x, y, sqrt(1.0 - dSquared));
    result = Quaternion(resultVector);
    return result;
    } // FindQuat()

// start the dragging process with a point in the range [-1..1]^2
void ArcBall::BeginDrag(float x, float y)
    { // BeginDrag()
    // convert the initial point to a quaternion
    dragFrom = FindQuat(x, y);
    } // BeginDrag()
    
// continue the dragging process with another such point
void ArcBall::ContinueDrag(float x, float y)
    { // ContinueDrag()
    // find the quaternion representing the point
    Quaternion nextQuat = FindQuat(x, y);
    // invert the previous quaternion
    Quaternion fromInverse = dragFrom.Inverse();
    // use it to compute the current rotation
    currentRotation = nextQuat * fromInverse;
    } // ContinueDrag()
    
// stop dragging
void ArcBall::EndDrag(float x, float y)
    { // EndDrag()
    // find the quaternion representing the point
    Quaternion nextQuat = FindQuat(x, y);
    // invert the previous quaternion
    Quaternion fromInverse = dragFrom.Inverse();
    // use it to compute the current rotation
    currentRotation = nextQuat * fromInverse;

    // now reset the quaternions ready for next time
    dragFrom = Quaternion(0, 0, 1, 0);
    // and reset current and base
    baseRotation = currentRotation * baseRotation;
    currentRotation = Quaternion(0, 0, 0, 1);

    } // EndDrag()
    
// extract the rotation matrix for rendering purposes
Matrix4 ArcBall::GetRotation()
    { // GetRotation()
    return currentRotation.GetMatrix() * baseRotation.GetMatrix();
    } // GetRotation()
