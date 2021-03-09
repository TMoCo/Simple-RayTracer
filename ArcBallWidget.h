/////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  -----------------------------
//  ArcBallWidget.h
//  -----------------------------
//  
//  This is the GUI for the arcball
//
//  Although we could keep the quaternions in the render parameters
//  it's tidier here due to the need to synchronise rotations
//
/////////////////////////////////////////////////////////////////

#ifndef _HEADER_ARCBALL_WIDGET_H
#define _HEADER_ARCBALL_WIDGET_H

#include <QOpenGLWidget>
#include <QMouseEvent>
#include "ArcBall.h"
#include "Matrix4.h"

class ArcBallWidget : public QOpenGLWidget
    { // class ArcBallWidget
    Q_OBJECT
    public:
    ArcBall theBall;
    
    //  constructor
    ArcBallWidget(QWidget *parent);

    // routine to return rotation matrix
    Matrix4 RotationMatrix();

    protected:
    // called when OpenGL context is set up
    void initializeGL();
    // called every time the widget is resized
    void resizeGL(int w, int h);
    // called every time the widget needs painting
    void paintGL();
    
    // mouse-handling
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    
    public slots:
    // routines called to allow synchronised rotation with other widget
    // coordinates are assumed to be in range of [-1..1] in x,y
    // if outside that range, will be clamped
    void BeginDrag(float x, float y);
    void ContinueDrag(float x, float y);
    void EndDrag(float x, float y);
    
    signals:
    // slot for calling controller when rotation changes
    void RotationChanged();
    }; // class ArcBallWidget

#endif
