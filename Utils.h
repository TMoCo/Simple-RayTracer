#ifndef UTILS_H
#define UTILS_H

#include "Cartesian3.h"
#include "Matrix4.h"
#include "RGBAValue.h"
#include "math.h"

// a small constant 
constexpr float EPSILON = 0.001;
// PI for trigonometric functions
constexpr float PI = 3.141592741;
// gamma correction
constexpr float GAMMA = 2;

// my class for RGB radiance values using floating point precision rather RGBAValue 
// with unsigned chars. I'm using this to compute lighting, then convert back to 
// RGBAValue for output to the image
class RGBRadiance {
    public:
        // default parameters are a radiance of 0 (no light) 
        RGBRadiance(float red = 0, float green = 0, float blue = 0) 
            : red_(red), green_(green), blue_(blue) {}
        // construct from a RGBAValue object
        RGBRadiance(const RGBAValue& rgba) 
            : red_((float)rgba.red / 255.0), green_((float)rgba.green / 255.0), 
            blue_((float)rgba.blue / 255.0) {}

        // convert to RGBAValue with gamma correction
        RGBAValue ToRGBAValue() const { 
            return RGBAValue(
                pow(red_, GAMMA) * 255.0F, pow(green_, GAMMA) * 255.0F, 
                pow(blue_, GAMMA) * 255.0F, 255.0F); 
        }

        // returns the sum of the red, green and blue components of the radiance
        float RadianceSum() const {
            return red_ + green_ + blue_;
        }
        // average radiance
        float RadianceAverage() const {
            return (red_ + green_ + blue_) / 3.0;
        }
        // absolute value of radiance
        RGBRadiance absoluteRadiance() const {
            return RGBRadiance(abs(red_), abs(green_), abs(blue_));
        }
        // modulate with an other radiance (like RGBAValue) 
        RGBRadiance modulate(const RGBRadiance& other) const {
            float leftRed = red_, leftGreen = green_, leftBlue = blue_;
            float rightRed = other.red_, rightGreen = other.green_, rightBlue = other.blue_;

            // modulate them & convert back to 0..255
            return RGBRadiance(leftRed * rightRed, leftGreen * rightGreen, leftBlue * rightBlue);
        }

        // useful operator overloads for addition, multiplication, division
        RGBRadiance operator+(const RGBRadiance& other) const {
            return RGBRadiance(
                other.red_ + red_, other.green_ + green_, other.blue_ + blue_); }
        RGBRadiance operator*(const float& scalar) const {
            return RGBRadiance(scalar * red_, scalar * green_, scalar * blue_); }
        RGBRadiance operator*(const RGBRadiance& other) const {
            return RGBRadiance(
                other.red_ * red_, other.green_ * green_, other.blue_ * blue_); }
        RGBRadiance operator/(const float& scalar) const {
            return RGBRadiance(red_ / scalar, green_ / scalar, blue_ / scalar); }

        // values range between 0 and 1 (They can be outside the range but that is 
        // taken care of when converting to RGBAValue, so we allow it)
        float red_, green_, blue_;
};

// simple ray class 
class Ray {
    public:
        Ray() {};
        Ray(const Cartesian3& origin, const Cartesian3& direction)
            : origin_(origin), direction_(direction) {}
        ~Ray() {}

        // accessor methods
        Cartesian3 direction() const { return direction_; } 
        Cartesian3 origin() const { return origin_; }

        // compute a parameterised point on the ray 
        Cartesian3 at(const float& t) const { return origin_ + t * direction_; }

        Cartesian3 origin_;
        Cartesian3 direction_;
};

// convenience triangle class used for rendering, created when reading in an obj
class Triangle {
    public:
    unsigned int vertices[3];
    unsigned int texCoords[3];
    unsigned int normals[3];
    unsigned int texID;
    unsigned int colour;
    unsigned int material;
    unsigned int id;
    unsigned int lightId = 0;
};

// a material class used for raytracer 
class Material {
    public:
    float emmisive[3] = {0, 0, 0};
    float lambertian[3] = {0.6, 0.6, 0.6};
    float glossy[4] = {0.3, 0.3, 0.3, 4};
    float albedo[3] = {0.6, 0.6, 0.6};
    float extinction = 0.5;
    float impulse = 0.5;
};

// a struct for representing a light
struct Light {
    Cartesian3 position;
    RGBRadiance intensity;
    bool atInfinity;
    // area light
    bool isAreaLight;
    // the triangle that forms the area light
    Triangle* triangle;
};

// a structure for representing a pixel
struct Pixel {
    Cartesian3 worldPos;
    RGBRadiance radiance;
};


#endif

