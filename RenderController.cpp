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
    QObject::connect(   renderWindow->lightRotator,                 SIGNAL(RotationChanged()),
                        this,                                       SLOT(lightRotationChanged()));

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

    // signal for emissive lighting change
    QObject::connect(   renderWindow->emissiveLightSlider,          SIGNAL(valueChanged(int)),
                        this,                                       SLOT(emissiveLightChanged(int)));

    // signal for ambient lighting change
    QObject::connect(   renderWindow->ambientLightSlider,           SIGNAL(valueChanged(int)),
                        this,                                       SLOT(ambientLightChanged(int)));

    // signal for diffuse lighting change
    QObject::connect(   renderWindow->diffuseLightSlider,           SIGNAL(valueChanged(int)),
                        this,                                       SLOT(diffuseLightChanged(int)));

    // signal for specular lighting change
    QObject::connect(   renderWindow->specularLightSlider,          SIGNAL(valueChanged(int)),
                        this,                                       SLOT(specularLightChanged(int)));

    // signal for specular exponent change
    QObject::connect(   renderWindow->specularExponentSlider,       SIGNAL(valueChanged(int)),
                        this,                                       SLOT(specularExponentChanged(int)));

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
    renderParameters->lightMatrix = renderWindow->lightRotator->RotationMatrix();
    } // RenderController::RenderController()

// slot for responding to arcball rotation for object
void RenderController::objectRotationChanged()
    { // RenderController::objectRotationChanged()
    // copy the rotation matrix from the widget to the model
    renderParameters->rotationMatrix = renderWindow->modelRotator->RotationMatrix();
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::objectRotationChanged()

// slot for responding to arcball rotation for light
void RenderController::lightRotationChanged()
    { // RenderController::lightRotationChanged()
    // copy the rotation matrix from the widget to the model
    renderParameters->lightMatrix = renderWindow->lightRotator->RotationMatrix();
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::lightRotationChanged()

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

// slot for responding to emissive light slider
void RenderController::emissiveLightChanged(int value)
    { // RenderController::emissiveLightChanged()
    // reset the value (slider ticks are 1/100 each)
    renderParameters->emissiveLight = (float) value / 100.0;
    
    // clamp it
    if (renderParameters->emissiveLight < LIGHTING_MIN)
        renderParameters->emissiveLight = LIGHTING_MIN;
    else if (renderParameters->emissiveLight > LIGHTING_MAX)
        renderParameters->emissiveLight = LIGHTING_MAX;

    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::emissiveLightChanged()

// slot for responding to ambient albedo slider
void RenderController::ambientLightChanged(int value)
    { // RenderController::ambientLightChanged()
    // reset the value (slider ticks are 1/100 each)
    renderParameters->ambientLight = (float) value / 100.0;
    
    // clamp it
    if (renderParameters->ambientLight < LIGHTING_MIN)
        renderParameters->ambientLight = LIGHTING_MIN;
    else if (renderParameters->ambientLight > LIGHTING_MAX)
        renderParameters->ambientLight = LIGHTING_MAX;
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::ambientLightChanged()

// slot for responding to diffuse albedo slider
void RenderController::diffuseLightChanged(int value)
    { // RenderController::diffuseLightChanged()
    // reset the value (slider ticks are 1/100 each)
    renderParameters->diffuseLight = (float) value / 100.0;
    
    // clamp it
    if (renderParameters->diffuseLight < LIGHTING_MIN)
        renderParameters->diffuseLight = LIGHTING_MIN;
    else if (renderParameters->diffuseLight > LIGHTING_MAX)
        renderParameters->diffuseLight = LIGHTING_MAX;
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::diffuseLightChanged()

// slot for responding to specular albedo slider
void RenderController::specularLightChanged(int value)
    { // RenderController::specularLightChanged()
    // reset the value (slider ticks are 1/100 each)
    renderParameters->specularLight = (float) value / 100.0;
    
    // clamp it
    if (renderParameters->specularLight < LIGHTING_MIN)
        renderParameters->specularLight = LIGHTING_MIN;
    else if (renderParameters->specularLight > LIGHTING_MAX)
        renderParameters->specularLight = LIGHTING_MAX;
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::specularLightChanged()

// slot for responding to specular exponent slider
void RenderController::specularExponentChanged(int value)
    { // RenderController::specularExponentChanged()
    // reset the value (slider ticks are 1/100 each)
    renderParameters->specularExponent = pow(10.0, (float) value / 100.0);
    
    // clamp it
    if (renderParameters->specularExponent < SPECULAR_EXPONENT_MIN)
        renderParameters->specularExponent = SPECULAR_EXPONENT_MIN;
    else if (renderParameters->specularExponent > SPECULAR_EXPONENT_MAX)
        renderParameters->specularExponent = SPECULAR_EXPONENT_MAX;
    
    // reset the interface
    renderWindow->ResetInterface();
    } // RenderController::specularExponentChanged()

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
        // right button drags the light
        case Qt::RightButton:
            renderWindow->lightRotator->BeginDrag(x, y);
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
        // right button drags the light
        case Qt::RightButton:
            renderWindow->lightRotator->ContinueDrag(x, y);
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
        // right button drags the light
        case Qt::RightButton:
            renderWindow->lightRotator->EndDrag(x, y);
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

