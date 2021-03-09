#ifndef SURFEL_H
#define SURFEL_H

#include "RenderParameters.h"
#include "RGBAValue.h"
#include "TexturedObject.h"
#include "Utils.h"

// struct for barycentric coordinates
struct Barycentric {
    float alpha;
    float beta;
    float gamma;
};

// POD Surfel
class Surfel {
    public:
        // default constructor is for a surfel object that does not exist 
        // (a surfel that does not belong to a triangle is meaningless)
        Surfel() : triangle_(nullptr) {}
        // when taking arguments, we have the information to create a surfel that exists
        Surfel(const Cartesian3& intersection, Triangle* triangle, const Barycentric& barycentric) 
            : triangle_(triangle), barycentric_(barycentric), position_(intersection) {}
        ~Surfel() {}

        // interpolate the values from the triangle data
        void InterpolateProperties(
            TexturedObject* object, RenderParameters* params);

        // surface BRDF at the surfel
        RGBRadiance BRDF(const Cartesian3& inDir, const Cartesian3& outDir) const;

    public:
        // triangle the surfel belongs to
        Triangle* triangle_;
        // surfel barycentric coordinates
        Barycentric barycentric_;

        // surfel position in world space, intersection of a Ray with a Triangle
        Cartesian3 position_;
        // surfel normal, interpolation of triangle vertex normals
        Cartesian3 normal_;
        // surfel uv coordinates, interpolation of triangle vertex texCoords
        Cartesian3 texCoord_;

        // material and lighting properties
        RGBRadiance emission_;
        RGBRadiance ambientAlbedo_;
        RGBRadiance lambertAlbedo_;
        RGBRadiance glossyAlbedo_;
        RGBRadiance impulseAlbedo_;
        float glossyExponent_;
        float distanceToEye;
        float extinction_;    
        float impulse_;
        // to detect if the surfel belongs to a light 
        bool isLight_;
        unsigned int lightId_;
};

#endif