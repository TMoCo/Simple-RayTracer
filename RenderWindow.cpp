/////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  -----------------------------
//  Render Window
//  -----------------------------
//  
//  The render window class is really just a container
//  for tracking the visual hierarchy.  While it is
//  entirely possible to use Qt Creator, I try to avoid
//  over-commitment to it because I need to write code in
//  multiple environments, some of which are not well-suited
//  to IDEs in general, let alone Qt Creator
//
//  Also, by providing sample code, the didactic purpose of 
//  showing how things fit together is better served.
//
/////////////////////////////////////////////////////////////////

#include "RenderWindow.h"
#include "RenderParameters.h"

// constructor
RenderWindow::RenderWindow
        (
        // the object to be rendered
        TexturedObject          *newTexturedObject, 
        // the model object storing render parameters
        RenderParameters        *newRenderParameters,
        // the title for the window (with default value)
        const char              *windowName
        )
    // call the inherited constructor
    // NULL indicates that this widget has no parent
    // i.e. that it is a top-level window
    :
    // member instantiation
    QWidget(NULL),
    texturedObject(newTexturedObject),
    renderParameters(newRenderParameters)
    { // RenderWindow::RenderWindow()
    // set the window's title
    setWindowTitle(QString(windowName));
    
    // initialise the grid layout
    windowLayout = new QGridLayout(this);
    
    // create all of the widgets, starting with the custom render widgets
    renderWidget                = new RenderWidget              (newTexturedObject,     newRenderParameters,        this);
    raytraceRenderWidget        = new RaytraceRenderWidget      (newTexturedObject,     newRenderParameters,        this);

    // construct custom arcball Widgets
    lightRotator                = new ArcBallWidget             (                       this);
    modelRotator                = new ArcBallWidget             (                       this);

    // construct standard QT widgets
    // check boxes
    lightingBox                 = new QCheckBox                 ("Lighting",            this);
    texturedRenderingBox        = new QCheckBox                 ("Textures",            this);
    textureModulationBox        = new QCheckBox                 ("Modulation",          this);
    
    // modelling options  
    showObjectBox               = new QCheckBox                 ("Object",              this);  
    centreObjectBox             = new QCheckBox                 ("Centre",              this);
    scaleObjectBox              = new QCheckBox                 ("Scale",               this);
    
    // spatial sliders
    xTranslateSlider            = new QSlider                   (Qt::Horizontal,        this);
    secondXTranslateSlider      = new QSlider                   (Qt::Horizontal,        this);
    yTranslateSlider            = new QSlider                   (Qt::Vertical,          this);
    zoomSlider                  = new QSlider                   (Qt::Vertical,          this);
    
    // lighting sliders
    emissiveLightSlider         = new QSlider                   (Qt::Horizontal,        this);
    ambientLightSlider          = new QSlider                   (Qt::Horizontal,        this);
    diffuseLightSlider          = new QSlider                   (Qt::Horizontal,        this);
    specularLightSlider         = new QSlider                   (Qt::Horizontal,        this);
    specularExponentSlider      = new QSlider                   (Qt::Horizontal,        this);
    samplesNbSlider             = new QSlider                   (Qt::Horizontal,        this);

    // labels for sliders and arcballs
    modelRotatorLabel           = new QLabel                    ("Model",               this);
    lightRotatorLabel           = new QLabel                    ("Light",               this);
    yTranslateLabel             = new QLabel                    ("Y",                   this);
    zoomLabel                   = new QLabel                    ("Zm",                  this);
    emissiveLightLabel          = new QLabel                    ("Samples                      Emissive",            this);
    ambientLightLabel           = new QLabel                    ("Ambient                      Diffuse",            this);
    specularLightLabel          = new QLabel                    ("Specular                     Exponent",           this);

    // button for raytracing
    rayTraceImageButton         = new QPushButton               ("Render Image",        this);
    
    // add all of the widgets to the grid               Row         Column      Row Span    Column Span
    
    // the top two widgets have to fit to the widgets stack between them
    int nStacked = 10;
    
    windowLayout->addWidget(renderWidget,               0,          1,          nStacked,   1           );
    windowLayout->addWidget(yTranslateSlider,           0,          2,          nStacked,   1           );
    windowLayout->addWidget(zoomSlider,                 0,          4,          nStacked,   1           );
    windowLayout->addWidget(raytraceRenderWidget,       0,          5,          nStacked,   1           );

    // the stack in the middle
    windowLayout->addWidget(lightRotator,               0,          3,          1,          1           );
    windowLayout->addWidget(lightRotatorLabel,          1,          3,          1,          1           );
    windowLayout->addWidget(modelRotator,               2,          3,          1,          1           );
    windowLayout->addWidget(modelRotatorLabel,          3,          3,          1,          1           );
    windowLayout->addWidget(showObjectBox,              4,          3,          1,          1           );
    windowLayout->addWidget(centreObjectBox,            5,          3,          1,          1           );
    windowLayout->addWidget(scaleObjectBox,             6,          3,          1,          1           );
    windowLayout->addWidget(lightingBox,                7,         3,          1,          1           );
    windowLayout->addWidget(texturedRenderingBox,       8,         3,          1,          1           );
    windowLayout->addWidget(textureModulationBox,       9,         3,          1,          1           );
    windowLayout->addWidget(rayTraceImageButton,	 	10,         3,          1,          1           );

    // Translate Slider Row
    windowLayout->addWidget(xTranslateSlider,           nStacked,   1,          1,          1           );
    windowLayout->addWidget(yTranslateLabel,            nStacked,   2,          1,          1           );
    // nothing in column 3
    windowLayout->addWidget(zoomLabel,                  nStacked,   4,          1,          1           );
    windowLayout->addWidget(secondXTranslateSlider,     nStacked,   5,          1,          1           );
    
    // Samples Row
    windowLayout->addWidget(samplesNbSlider,            nStacked+1, 1,          1,          1           );
    // Emissive Row
    // label covers three columns
    windowLayout->addWidget(emissiveLightLabel,         nStacked+1, 2,          1,          3           );
    windowLayout->addWidget(emissiveLightSlider,        nStacked+1, 5,          1,          1           );
    
    // Ambient Row
    // label covers three columns
    windowLayout->addWidget(ambientLightLabel,          nStacked+2, 2,          1,          3           );
    windowLayout->addWidget(ambientLightSlider,         nStacked+2, 1,          1,          1           );
    
    // Diffuse Row
    // label covers three columns
    windowLayout->addWidget(diffuseLightSlider,         nStacked+2, 5,          1,          1           );
    
    // Specular Row
    // label covers three columns
    windowLayout->addWidget(specularLightLabel,         nStacked+3, 2,          1,          3           );
    windowLayout->addWidget(specularLightSlider,        nStacked+3, 1,          1,          1           );
    
    // Exponent Row
    // label covers three columns
    windowLayout->addWidget(specularExponentSlider,     nStacked+3, 5,          1,          1           );

    // now reset all of the control elements to match the render parameters passed in
    ResetInterface();
    } // RenderWindow::RenderWindow()

// routine to reset interface
// setsQButton every visual control to match the model
// gets called by the controller after each change in the model
void RenderWindow::ResetInterface()
    { // RenderWindow::QButtonQButton()
    // set check boxes
    lightingBox             ->setChecked        (renderParameters   ->  useLighting);
    texturedRenderingBox    ->setChecked        (renderParameters   ->  texturedRendering);
    textureModulationBox    ->setChecked        (renderParameters   ->  textureModulation);
    showObjectBox           ->setChecked        (renderParameters   ->  showObject);
    centreObjectBox         ->setChecked        (renderParameters   ->  centreObject);
    scaleObjectBox          ->setChecked        (renderParameters   ->  scaleObject);
    
    // set sliders
    // x & y translate are scaled to notional unit sphere in render widgets
    // but because the slider is defined as integer, we multiply by a 100 for all sliders
    xTranslateSlider        ->setMinimum        ((int) (TRANSLATE_MIN                               * PARAMETER_SCALING));
    xTranslateSlider        ->setMaximum        ((int) (TRANSLATE_MAX                               * PARAMETER_SCALING));
    xTranslateSlider        ->setValue          ((int) (renderParameters -> xTranslate              * PARAMETER_SCALING));
    
    secondXTranslateSlider  ->setMinimum        ((int) (TRANSLATE_MIN                               * PARAMETER_SCALING));
    secondXTranslateSlider  ->setMaximum        ((int) (TRANSLATE_MAX                               * PARAMETER_SCALING));
    secondXTranslateSlider  ->setValue          ((int) (renderParameters -> xTranslate              * PARAMETER_SCALING));
    
    yTranslateSlider        ->setMinimum        ((int) (TRANSLATE_MIN                               * PARAMETER_SCALING));
    yTranslateSlider        ->setMaximum        ((int) (TRANSLATE_MAX                               * PARAMETER_SCALING));
    yTranslateSlider        ->setValue          ((int) (renderParameters -> yTranslate              * PARAMETER_SCALING));

    // zoom slider is a logarithmic scale, so we want a narrow range
    zoomSlider              ->setMinimum        ((int) (ZOOM_SCALE_LOG_MIN                          * PARAMETER_SCALING));
    zoomSlider              ->setMaximum        ((int) (ZOOM_SCALE_LOG_MAX                          * PARAMETER_SCALING));
    zoomSlider              ->setValue          ((int) (log10(renderParameters -> zoomScale)        * PARAMETER_SCALING));

    // main lighting parameters are simple 0.0-1.0
    samplesNbSlider         ->setMinimum        (SAMPLES_MIN               );        
    samplesNbSlider         ->setMaximum        (SAMPLES_MAX               );        
    samplesNbSlider         ->setValue          (renderParameters->samples_);        

    emissiveLightSlider     ->setMinimum        ((int) (LIGHTING_MIN                                * PARAMETER_SCALING));
    emissiveLightSlider     ->setMaximum        ((int) (LIGHTING_MAX                                * PARAMETER_SCALING));
    emissiveLightSlider     ->setValue          ((int) (renderParameters -> emissiveLight           * PARAMETER_SCALING));
    
    ambientLightSlider      ->setMinimum        ((int) (LIGHTING_MIN                                * PARAMETER_SCALING));
    ambientLightSlider      ->setMaximum        ((int) (LIGHTING_MAX                                * PARAMETER_SCALING));
    ambientLightSlider      ->setValue          ((int) (renderParameters -> ambientLight            * PARAMETER_SCALING));
    
    diffuseLightSlider      ->setMinimum        ((int) (LIGHTING_MIN                                * PARAMETER_SCALING));
    diffuseLightSlider      ->setMaximum        ((int) (LIGHTING_MAX                                * PARAMETER_SCALING));
    diffuseLightSlider      ->setValue          ((int) (renderParameters -> diffuseLight            * PARAMETER_SCALING));
    
    specularLightSlider ->setMinimum            ((int) (LIGHTING_MIN                                * PARAMETER_SCALING));
    specularLightSlider ->setMaximum            ((int) (LIGHTING_MAX                                * PARAMETER_SCALING));
    specularLightSlider ->setValue              ((int) (renderParameters -> specularLight           * PARAMETER_SCALING));
    
    // specular exponent slider is a logarithmic scale, so we want a narrow range
    specularExponentSlider  ->setMinimum        ((int) (SPECULAR_EXPONENT_LOG_MIN                   * PARAMETER_SCALING));
    specularExponentSlider  ->setMaximum        ((int) (SPECULAR_EXPONENT_LOG_MAX                   * PARAMETER_SCALING));
    specularExponentSlider  ->setValue          ((int) (log10(renderParameters -> specularExponent) * PARAMETER_SCALING));
    
    // now flag them all for update 
    renderWidget            ->update();
    raytraceRenderWidget    ->update();
    modelRotator            ->update();
    lightRotator            ->update();
    xTranslateSlider        ->update();
    secondXTranslateSlider  ->update();
    yTranslateSlider        ->update();
    zoomSlider              ->update();
    emissiveLightSlider     ->update(); 
    ambientLightSlider      ->update();
    diffuseLightSlider      ->update();
    specularLightSlider 	->update();
    specularExponentSlider  ->update();
    lightingBox             ->update();
    texturedRenderingBox    ->update();
    textureModulationBox    ->update();
    showObjectBox           ->update();
    centreObjectBox         ->update();
    scaleObjectBox          ->update();
    } // RenderWindow::ResetInterface()
