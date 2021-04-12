/////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  -----------------------------
//  Render Controller
//  -----------------------------
//  
//  We're using the Model-View-Controller pattern
//  so most of the control logic goes here
//  which means we need a slot for substantially
//  every possible UI manipulation
//
/////////////////////////////////////////////////////////////////

#include "RenderController.h"
#include <stdio.h>

// constructor
RenderController::RenderController
        (
        // the geometric object to show
        TexturedObject      *newTexturedObject,
        // the render parameters to use
        RenderParameters    *newRenderParameters,
        // the render window that it controls
        RenderWindow        *newRenderWindow
        )
    :
    texturedObject  (newTexturedObject),
    renderParameters(newRenderParameters),
    renderWindow    (newRenderWindow),
    dragButton      (Qt::NoButton)
    { // RenderController::RenderController()
    
    // connect up signals to slots

    // signals for arcballs
    QObject::connect(   renderWindow->modelRotator,                 SIGNAL(RotationChanged()),
                        this,                                       SLOT(objectRotationChanged()));

    // signals for main widget to control arcball
    QObject::connect(   renderWindow->renderWidget,                 SIGNAL(BeginScaledDrag(int, float, float)),
                        this,                                       SLOT(BeginScaledDrag(int, float, float)));
    QObject::connect(   renderWindow->renderWidget,                 SIGNAL(ContinueScaledDrag(float, float)),
                        this,                                       SLOT(ContinueScaledDrag(float, float)));
    QObject::connect(   renderWindow->renderWidget,                 SIGNAL(EndScaledDrag(float, float)),
                        this,                                       SLOT(EndScaledDrag(float, float)));

    // signals for raytrace widget to control arcball
    QObject::connect(   renderWindow->raytraceRenderWidget,         SIGNAL(BeginScaledDrag(int, float, float)),
                        this,                                       SLOT(BeginScaledDrag(int, float, float)));
    QObject::connect(   renderWindow->raytraceRenderWidget,         SIGNAL(ContinueScaledDrag(float, float)),
                        this,                                       SLOT(ContinueScaledDrag(float, float)));
    QObject::connect(   renderWindow->raytraceRenderWidget,         SIGNAL(EndScaledDrag(float, float)),
                        this,                                       SLOT(EndScaledDrag(float, float)));

    // signal for zoom slider
    QObject::connect(   renderWindow->zoomSlider,                   SIGNAL(valueChanged(int)),
                        this,                                       SLOT(zoomChanged(int)));

    // signal for x translate sliders
    QObject::connect(   renderWindow->xTranslateSlider,             SIGNAL(valueChanged(int)),
                        this,                                       SLOT(xTranslateChanged(int)));
    QObject::connect(   renderWindow->secondXTranslateSlider,       SIGNAL(valueChanged(int)),
                        this,                                       SLOT(xTranslateChanged(int)));

    // signal for y translate slider
    QObject::connect(   renderWindow->yTranslateSlider,             SIGNAL(valueChanged(int)),
                        this,                                       SLOT(yTranslateChanged(int)));
    // signal for check box for lighting
    QObject::connect(   renderWindow->lightingBox,                  SIGNAL(stateChanged(int)),
                        this,                                       SLOT(useLightingCheckChanged(int)));

    // signal for check box for textured rendering
    QObject::connect(   renderWindow->texturedRenderingBox,         SIGNAL(stateChanged(int)),
                        this,                                       SLOT(texturedRenderingCheckChanged(int)));

    // signal for check box for texture modulation
    QObject::connect(   renderWindow->textureModulationBox,         SIGNAL(stateChanged(int)),
                        this,                                       SLOT(textureModulationCheckChanged(int)));

    // signal for check box for objects
    QObject::connect(   renderWindow->showObjectBox,                SIGNAL(stateChanged(int)),
                        this,                                       SLOT(showObjectCheckChanged(int)));

    // signal for check box for centring
    QObject::connect(   renderWindow->centreObjectBox,              SIGNAL(stateChanged(int)),
                        this,                                       SLOT(centreObjectCheckChanged(int)));

    // signal for check box for scaling
    QObject::connect(   renderWindow->scaleObjectBox,               SIGNAL(stateChanged(int)),
                        this,                                       SLOT(scaleObjectCheckChanged(int)));

    // signal for rendering a ray traced image
    QObject::connect(   renderWindow->rayTraceImageButton,          SIGNAL(pressed()),
                        this,                                       SLOT(raytraceButtonPressed()));

    QObject::connect(   renderWindow->samplesNbSlider,              SIGNAL(valueChanged(int)),
                        this,                                       SLOT(sampleNumberChanged(int)));

    // copy the rotation matrix from the widgets to the model
    renderParameters->rotationMatrix = renderWindow->modelRotator->RotationMatrix();
    } // RenderController::RenderController()

// slot for responding to arcball rotation for object
void RenderController::objectRotationChanged()
    { // RenderController::objectRotationChanged()
    // copy the rotation matrix from the widget to the model
    renderParameters->rotationMatrix = renderWindow->modelRotator->RotationMatrix();
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::objectRotationChanged()

// slot for responding to zoom slider
void RenderController::zoomChanged(int value)
    { // RenderController::zoomChanged()
    // compute the new scale
    float newZoomScale = pow(10.0, (float) value / 100.0);

    // clamp it
    if (newZoomScale < ZOOM_SCALE_MIN)
        newZoomScale = ZOOM_SCALE_MIN;
    else if (newZoomScale > ZOOM_SCALE_MAX)
        newZoomScale = ZOOM_SCALE_MAX;

    // and reset the value  
    renderParameters->zoomScale = newZoomScale;
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::zoomChanged()

// slot for responding to x translate sliders
void RenderController::xTranslateChanged(int value)
    { // RenderController::xTranslateChanged()
    // reset the model's x translation (slider ticks are 1/100 each)
    renderParameters->xTranslate = (float) value / 100.0;

    // clamp it
    if (renderParameters->xTranslate < TRANSLATE_MIN)
        renderParameters->xTranslate = TRANSLATE_MIN;
    else if (renderParameters->xTranslate > TRANSLATE_MAX)
        renderParameters->xTranslate = TRANSLATE_MAX;
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::xTranslateChanged()

// slot for responding to y translate slider
void RenderController::yTranslateChanged(int value)
    { // RenderController::tTranslateChanged()
    // reset the model's y translation (slider ticks are 1/100 each)
    renderParameters->yTranslate =  (float) value / 100.0;

    // clamp it
    if (renderParameters->yTranslate < TRANSLATE_MIN)
        renderParameters->yTranslate = TRANSLATE_MIN;
    else if (renderParameters->yTranslate > TRANSLATE_MAX)
        renderParameters->yTranslate = TRANSLATE_MAX;
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::yTranslateChanged()

// slot for toggling lighting
void RenderController::useLightingCheckChanged(int state)
    { // RenderController::useLightingCheckChanged()
    // reset the model's flag
    renderParameters->useLighting = (state == Qt::Checked); 

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::useLightingCheckChanged()

// slot for toggling textures
void RenderController::texturedRenderingCheckChanged(int state)
    { // RenderController::texturedRenderingCheckChanged()
    // reset the model's flag
    renderParameters->texturedRendering = (state == Qt::Checked); 

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::texturedRenderingCheckChanged()
    
// slot for toggling texture modulation
void RenderController::textureModulationCheckChanged(int state)
    { // RenderController::textureModulationCheckChanged()
    // reset the model's flag
    renderParameters->textureModulation = (state == Qt::Checked); 

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::textureModulationCheckChanged()
    
// slot for toggling object
void RenderController::showObjectCheckChanged(int state)
    { // RenderController::showObjectCheckChanged()
    // reset the model's flag
    renderParameters->showObject = (state == Qt::Checked); 

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::showObjectCheckChanged()
    
// slot for toggling object centring
void RenderController::centreObjectCheckChanged(int state)
    { // RenderController::centreObjectCheckChanged()
    // reset the model's flag
    renderParameters->centreObject = (state == Qt::Checked); 

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::centreObjectCheckChanged()
    
// slot for toggling object scaling
void RenderController::scaleObjectCheckChanged(int state)
    { // RenderController::scaleObjectCheckChanged()
    // reset the model's flag
    renderParameters->scaleObject = (state == Qt::Checked); 

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::scaleObjectCheckChanged()

// slot for raytracing
void RenderController::raytraceButtonPressed() {
    renderWindow->raytraceRenderWidget->Raytrace();
    renderWindow->ResetInterface();
}

// receive a value from the samples slider and update parameters
void RenderController::sampleNumberChanged(int value) {
    renderParameters->samples_ = value;
    renderWindow->ResetInterface();
}


// slots for responding to arcball manipulations
// these are general purpose signals which pass the mouse moves to the controller
// after scaling to the notional unit sphere
void RenderController::BeginScaledDrag(int whichButton, float x, float y)
    { // RenderController::BeginScaledDrag()
    // depends on which button was depressed, so save that for the duration
    dragButton = whichButton;

    // now switch on it to determine behaviour
    switch (dragButton)
        { // switch on the drag button
        // left button drags the model
        case Qt::LeftButton:
            renderWindow->modelRotator->BeginDrag(x, y);
            break;
        // middle button drags visually
        case Qt::MiddleButton:
            break;
        } // switch on the drag button

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::BeginScaledDrag()
    
// note that Continue & End assume the button has already been set
void RenderController::ContinueScaledDrag(float x, float y)
    { // RenderController::ContinueScaledDrag()
    // switch on the drag button to determine behaviour
    switch (dragButton)
        { // switch on the drag button
        // left button drags the model
        case Qt::LeftButton:
            renderWindow->modelRotator->ContinueDrag(x, y);
            break;
        // middle button drags visually
        case Qt::MiddleButton:
            break;
        } // switch on the drag button

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::ContinueScaledDrag()

void RenderController::EndScaledDrag(float x, float y)
    { // RenderController::EndScaledDrag()
    // now switch on it to determine behaviour
    switch (dragButton)
        { // switch on the drag button
        // left button drags the model
        case Qt::LeftButton:
            renderWindow->modelRotator->EndDrag(x, y);
            break;
        // middle button drags visually
        case Qt::MiddleButton:
            break;
        } // switch on the drag button

    // and reset the drag button
    dragButton = Qt::NoButton;

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::EndScaledDrag()

