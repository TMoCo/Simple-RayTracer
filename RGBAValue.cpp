//////////////////////////////////////////////////////////////////////
//
//  University of Leeds
//  COMP 5812M Foundations of Modelling & Rendering
//  User Interface for Coursework
//
//  September, 2020
//
//  ------------------------
//  RGBAValue.h
//  ------------------------
//  
//  A minimal class for a colour in RGB space
//  
///////////////////////////////////////////////////

#include "RGBAValue.h"

// default constructor
RGBAValue::RGBAValue()
    :
    red(0),
    green(0),
    blue(0),
    alpha(0)
    { // constructor
    } // constructor

// value constructor
RGBAValue::RGBAValue(unsigned char Red, unsigned char Green, unsigned char Blue, unsigned char Alpha)
    :
    red(Red),
    green(Green),
    blue(Blue),
    alpha(Alpha)
    { // constructor
    } // constructor

// value constructor with floats
// values outside 0.0-255.0 are clamped
RGBAValue::RGBAValue(float Red, float Green, float Blue, float Alpha)
    { // constructor
    if (Red > 255.0) Red = 255.0;
    if (Red < 0.0) Red = 0.0;
    // assign and let the compiler convert to unsigned char
    red = Red;

    if (Green > 255.0) Green = 255.0;
    if (Green < 0.0) Green = 0.0;
    green = Green;

    if (Blue > 255.0) Blue = 255.0;
    if (Blue < 0.0) Blue = 0.0;
    blue = Blue;

    if (Alpha > 255.0) Alpha = 255.0;
    if (Alpha < 0.0) Alpha = 0.0;
    alpha = Alpha;
    } // constructor

// copy constructor
RGBAValue::RGBAValue(const RGBAValue &other)
    :
    red(other.red),
    green(other.green),
    blue(other.blue),
    alpha(other.alpha)
    { // copy constructor
    } // copy constructor

RGBAValue& RGBAValue::operator=(const RGBAValue& copy) {
    red = copy.red;
    green = copy.green;
    blue = copy.blue;
    alpha = copy.alpha;
    return *this;
}


// convenience routines for scalar multiplication and addition
RGBAValue operator *(float scalar, const RGBAValue &colour)
    { // operator * ()
    // feed the new values into a constructor & return
    return RGBAValue(   scalar * (float) colour.red, 
                        scalar * (float) colour.green, 
                        scalar * (float) colour.blue, 
                        scalar * (float) colour.alpha);
    } // operator * ()

RGBAValue operator /(const RGBAValue &colour, float scalar)
    { // operator * ()
    // feed the new values into a constructor & return
    return RGBAValue(   (float) colour.red / scalar, 
                        (float) colour.green / scalar, 
                        (float) colour.blue / scalar, 
                        (float) colour.alpha / scalar);
    } 

RGBAValue operator +(const RGBAValue &left, const RGBAValue &right)
    { // operator + ()
    // compute new values, then use constructor to scale & clamp
    return RGBAValue(   ((float) left.red   + (float) right.red), 
                        ((float) left.green + (float) right.green),
                        ((float) left.blue  + (float) right.blue),
                        ((float) left.alpha + (float) right.alpha));
    } // operator + ()

// colour modulation routine:
// NB: this routine scales each component by 1/255.0, multiplies then inverts
RGBAValue RGBAValue::modulate(const RGBAValue &right) const
    { // modulate()
    float leftRed = (float) red / 255.0, leftGreen = (float) green / 255.0;
    float leftBlue = (float) blue / 255.0, leftAlpha = (float) alpha / 255.0;
    float rightRed = (float) right.red / 255.0, rightGreen = (float) right.green / 255.0;
    float rightBlue = (float) right.blue / 255.0, rightAlpha = (float) right.alpha / 255.0;

    // modulate them & convert back to 0..255
    return RGBAValue((float) 255.0 * leftRed * rightRed, (float) 255.0 * leftGreen * rightGreen, (float) 255.0 * leftBlue * rightBlue, (float) 255.0 * leftAlpha * rightAlpha);
    } // modulate()

// stream input
// WARNING - always sets alpha to 255
std::istream & operator >> (std::istream &inStream, RGBAValue &value)
    { // stream input
    // we want to read integers, not characters, so we create a temporary variable
    int component;
    inStream >> component;
    value.red = component;
    inStream >> component;
    value.green = component;
    inStream >> component;
    value.blue = component;
    return inStream;
    } // stream input

// stream output
// WARNING - does not output alpha
std::ostream & operator << (std::ostream &outStream, const RGBAValue &value)
    { // stream output
    // we want to output as integers, not characters, so:
    int component = 0;
    component = value.red;
    outStream << component << " ";
    component = value.green;
    outStream << component << " ";
    component = value.blue;
    outStream << component;
    return outStream;
    } // stream output

