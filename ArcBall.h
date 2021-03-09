//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  ArcBall.h
//  ------------------------
//  
//  A logical arcball (UI is separate)
//  
///////////////////////////////////////////////////

// include guard
#ifndef ARCBALL_H_
#define ARCBALL_H_

#include "Quaternion.h"
#include "Matrix4.h"

// the arcball class
class ArcBall
    { // class ArcBall
    public:
    // the base rotation
    Quaternion baseRotation;
    // the current rotation
    Quaternion currentRotation;
    // originating point for a rotation
    Quaternion dragFrom;
    
    // constructor - initializes to a zero rotation
    ArcBall();

    // convert an (x,y) point to a quaternion
    Quaternion FindQuat(float x, float y);

    // start the dragging process with a point in the range [-1..1]^2
    void BeginDrag(float x, float y);
    
    // continue the dragging process with another such point
    void ContinueDrag(float x, float y);
    
    // stop dragging
    void EndDrag(float x, float y);
    
    // extract the rotation matrix for rendering purposes
    Matrix4 GetRotation();
    }; // class ArcBall

#endif