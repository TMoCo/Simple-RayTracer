//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  -----------------------------
//  Render Widget
//  -----------------------------
//  
//  Since the render code is in the geometric object class
//  this widget primarily sets up the transformation matrices and 
//  lighting.
//
//  It implements the UI for an arcball controller, but in an
//  abstractable way that allows it to share an arcball with another
//  widget (the arcball controller) - thus you can manipulate the
//  object either directly in the widget or indirectly through the
//  arcball displayed visually.
//  
//  It also supports translation by visual dragging.  This results in
//  needing three mouse buttons, and not all systems have them, so it
//  will be up to the user to make sure that they can use it on their
//  own machine (especially on OSX)
//
//  Since the controls are (potentially) shared with other widgets, 
//  this widget is only responsible for scaling the x,y of mouse events
//  then passing them to the controller
//  
////////////////////////////////////////////////////////////////////////

// include guard
#ifndef _RENDER_WIDGET_H
#define _RENDER_WIDGET_H

// include the relevant QT headers
#include <QOpenGLWidget>
#include <QMouseEvent>

// and include all of our own headers that we need
#include "TexturedObject.h"
#include "RenderParameters.h"

// class for a render widget with arcball linked to an external arcball widget
class RenderWidget : public QOpenGLWidget                                       
    { // class RenderWidget
    Q_OBJECT
    private:    
    // the geometric object to be rendered
    TexturedObject *texturedObject; 

    // the render parameters to use
    RenderParameters *renderParameters;

    public:
    // constructor
    RenderWidget
            (
            // the geometric object to show
            TexturedObject      *newTexturedObject,
            // the render parameters to use
            RenderParameters    *newRenderParameters,
            // parent widget in visual hierarchy
            QWidget             *parent
            );
    
    // destructor
    ~RenderWidget();
            
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

    // these signals are needed to support shared arcball control
    public:
    signals:
    // these are general purpose signals, which scale the drag to 
    // the notional unit sphere and pass it to the controller for handling
    void BeginScaledDrag(int whichButton, float x, float y);
    // note that Continue & End assume the button has already been set
    void ContinueScaledDrag(float x, float y);
    void EndScaledDrag(float x, float y);
    }; // class RenderWidget

#endif
