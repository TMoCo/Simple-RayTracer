/////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  -----------------------------
//  ArcBallWidget
//  -----------------------------
//  
//  A QT widget for controlling an arcball
//  NB: we assume that the arcball is somewhere else, and merely
//  emit signals to control it.  That way, the arcball can be shared
//  between multiple widgets, and the controller takes care of the
//  details
//
/////////////////////////////////////////////////////////////////
#define ARCBALL_WIDGET_SIZE 100

#include "ArcBallWidget.h"

GLfloat sphereVert[84][3] = {   {   0,  0,  -1  },  
    {   0.5,    0,  -0.86603    },  
    {   0.86603,    0,  -0.5    },  
    {   1,  0,  0   },  
    {   0.86603,    0,  0.5 },  
    {   0.5,    0,  0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   0.43301,    0.25,   -0.86603    },  
    {   0.75,   0.43301,    -0.5    },  
    {   0.86603,    0.5,    0   },  
    {   0.75,   0.43301,    0.5 },  
    {   0.43301,    0.25,   0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   0.25,   0.43301,    -0.86603    },  
    {   0.43301,    0.75,   -0.5    },  
    {   0.5,    0.86603,    0   },  
    {   0.43301,    0.75,   0.5 },  
    {   0.25,   0.43301,    0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   0,  0.5,    -0.86603    },  
    {   0,  0.86603,    -0.5    },  
    {   0,  1,  0   },  
    {   0,  0.86603,    0.5 },  
    {   0,  0.5,    0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   -0.25,  0.43301,    -0.86603    },  
    {   -0.43301,   0.75,   -0.5    },  
    {   -0.5,   0.86603,    0   },  
    {   -0.43301,   0.75,   0.5 },  
    {   -0.25,  0.43301,    0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   -0.43301,   0.25,   -0.86603    },  
    {   -0.75,  0.43301,    -0.5    },  
    {   -0.86603,   0.5,    0   },  
    {   -0.75,  0.43301,    0.5 },  
    {   -0.43301,   0.25,   0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   -0.5,   0,  -0.86603    },  
    {   -0.86603,   0,  -0.5    },  
    {   -1, 0,  0   },  
    {   -0.86603,   0,  0.5 },  
    {   -0.5,   0,  0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   -0.43301,   -0.25,  -0.86603    },  
    {   -0.75,  -0.43301,   -0.5    },  
    {   -0.86603,   -0.5,   0   },  
    {   -0.75,  -0.43301,   0.5 },  
    {   -0.43301,   -0.25,  0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   -0.25,  -0.43301,   -0.86603    },  
    {   -0.43301,   -0.75,  -0.5    },  
    {   -0.5,   -0.86603,   0   },  
    {   -0.43301,   -0.75,  0.5 },  
    {   -0.25,  -0.43301,   0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   0,  -0.5,   -0.86603    },  
    {   0,  -0.86603,   -0.5    },  
    {   0,  -1, 0   },  
    {   0,  -0.86603,   0.5 },  
    {   0,  -0.5,   0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   0.25,   -0.43301,   -0.86603    },  
    {   0.43301,    -0.75,  -0.5    },  
    {   0.5,    -0.86603,   0   },  
    {   0.43301,    -0.75,  0.5 },  
    {   0.25,   -0.43301,   0.86603 },  
    {   0,  0,  1   },  
    {   0,  0,  -1  },  
    {   0.43301,    -0.25,  -0.86603    },  
    {   0.75,   -0.43301,   -0.5    },  
    {   0.86603,    -0.5,   0   },  
    {   0.75,   -0.43301,   0.5 },  
    {   0.43301,    -0.25,  0.86603 },  
    {   0,  0,  1   }   
    };

ArcBallWidget::ArcBallWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    { // ArcBallWidget()
    // let QT know we are fixed size
    setFixedSize(QSize(ARCBALL_WIDGET_SIZE, ARCBALL_WIDGET_SIZE));
    } // ArcBallWidget()
    
// routine to copy rotation matrix
Matrix4 ArcBallWidget::RotationMatrix()
    { // ArcBallWidget::RotationMatrix()
    return theBall.GetRotation();
    } // ArcBallWidget::RotationMatrix()
        
// called when OpenGL context is set up
void ArcBallWidget::initializeGL()
    { // ArcBallWidget::initializeGL()
    // no lighting, but we need depth test
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    // background is white
    glClearColor(0.9, 0.9, 0.9, 1.0);
    } // ContourTreeWidget::initializeGL()
    
// called every time the widget is resized
void ArcBallWidget::resizeGL(int w, int h)
    { // ArcBallWidget::resizeGL()
    // reset the viewport
    glViewport(0, 0, w, h);
    
    // set projection matrix to have range of -1.0 - 1.0 in x, y, z
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    } // ArcBallWidget::resizeGL()
    
// called every time the widget needs painting
void ArcBallWidget::paintGL()
    { // ArcBallWidget::paintGL()
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set model view matrix based on arcball rotation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw a single white quad to block back half of arcball
    glColor3f(0.92, 0.92, 0.92);
    glBegin(GL_QUADS);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glEnd();

    // set colour to black
    glColor3f(0.0, 0.0, 0.0);
    // draw a circle around the edge of the sphere
    // reusing data from some of the elements of the sphere
    glScalef(0.8, 0.8, 0.8);
    glBegin(GL_LINE_LOOP);
    for (int j = 3; j < 84; j += 7)
        glVertex3f(sphereVert[j][0], sphereVert[j][1], 0.5);
    glEnd();
    
    // retrieve rotation from arcball & apply
    Matrix4 rotMatrix = theBall.GetRotation();
    glMultMatrixf(rotMatrix.columnMajor().coordinates);

    // loop through verticals of sphere
    for (int i = 0; i < 12; i++)
        { // loop drawing verticals
        glBegin(GL_LINE_STRIP);
        for (int j = i*7; j < 7+i*7; j++)
            glVertex3fv(sphereVert[j]);
        glEnd();        
        } // loop drawing verticals
    // and loop through horizontals
    for (int i = 1; i < 6; i++)
        { // loop for horizontals
        glBegin(GL_LINE_LOOP);
        for (int j = i; j < 84; j += 7)
            glVertex3fv(sphereVert[j]);
        glEnd();
        } // loop for horizontals   
    } // ArcBallWidget::paintGL()
                                            
// mouse-handling
void ArcBallWidget::mousePressEvent(QMouseEvent *event)
    { // ArcBallWidget::mousePressEvent()
    float x = event->x();
    float y = event->y();
    float width = this->width(); 
    float height = this->height();
    float scaledX = (2.0 * x - width) / width;
    // this one has to flip from Qt coordinates to Cartesian
    float scaledY = (height - 2.0 * y) / height;
    // set the initial rotation for the drag
    theBall.BeginDrag(scaledX, scaledY);
    // and send a signal that we'ver changed
    emit RotationChanged();
    } // ArcBallWidget::mousePressEvent()

void ArcBallWidget::mouseMoveEvent(QMouseEvent *event)
    { // ArcBallWidget::mouseMoveEvent()
    float x = event->x();
    float y = event->y();
    float width = this->width(); 
    float height = this->height();
    float scaledX = (2.0 * x - width) / width;
    // this one has to flip from Qt coordinates to Cartesian
    float scaledY = (height - 2.0 * y) / height;
    // set the mid point of the drag
    theBall.ContinueDrag(scaledX, scaledY);
    // and send a signal that we'ver changed
    emit RotationChanged();
    } // ArcBallWidget::mouseMoveEvent()

void ArcBallWidget::mouseReleaseEvent(QMouseEvent *event)
    { // ArcBallWidget::mouseReleaseEvent()
    float x = event->x();
    float y = event->y();
    float width = this->width(); 
    float height = this->height();
    float scaledX = (2.0 * x - width) / width;
    // this one has to flip from Qt coordinates to Cartesian
    float scaledY = (height - 2.0 * y) / height;
    // set the final rotation for the drag
    theBall.EndDrag(scaledX, scaledY);
    // and send a signal that we'ver changed
    emit RotationChanged();
    } // ArcBallWidget::mouseReleaseEvent()

// routines called to allow synchronised rotation with other widget
// coordinates are assumed to be in range of [-1..1] in x,y
// if outside that range, will be clamped
void ArcBallWidget::BeginDrag(float x, float y)
    { // ArcBallWidget::BeginDrag()
    // start the drag
    theBall.BeginDrag(x,y);
    // and signal to indicate changed rotation matrix
    emit RotationChanged();
    } // ArcBallWidget::BeginDrag()
    
void ArcBallWidget::ContinueDrag(float x, float y)
    { // ArcBallWidget::ContinueDrag()
    // start the drag
    theBall.ContinueDrag(x,y);
    // and signal to indicate changed rotation matrix
    emit RotationChanged();
    } // ArcBallWidget::ContinueDrag()
    
void ArcBallWidget::EndDrag(float x, float y)
    { // ArcBallWidget::EndDrag()
    // start the drag
    theBall.EndDrag(x,y);
    // and signal to indicate changed rotation matrix
    emit RotationChanged();
    } // ArcBallWidget::EndDrag()


