#ifndef RAYTRACER_H
#define RAYTRACER_H


#include <random>

#include "RenderParameters.h"
#include "RGBAImage.h"
#include "Surfel.h"
#include "TexturedObject.h"
#include "Utils.h"

// the ray tracer class, ray traces an image 
class RayTracer {   
    public:
        RayTracer(RGBAImage* frameBuffer, RenderParameters* renderParameters, 
        TexturedObject* object)
            : frameBuffer_ (frameBuffer), parameters_(renderParameters), 
            object_(object) {}
        ~RayTracer() {}

        // raytrace the image
        void RayTraceImage();
            //RGBAImage* image, TexturedObject* theObject, RenderParameters* params);

    private: 
        // a sub function that ray traces certain sections of the image
        void RayTracePixelsThread(
            const long& begin, const long& rows, const Cartesian3& eyePos);

        // path trace a single ray
        RGBRadiance PathTrace(const Ray& ray, const RGBRadiance& combinedAlbedo, int& depth);

        // get the transformations set through UI
        Matrix4 GetTransform(const bool& inverse, const float& scale);
            
        // return a pointer to a surfel at intersection of ray with object
        bool ClosestTriangleIntersect(const Ray& ray, Surfel* surfel);

        // lighting methods
        RGBRadiance DirectLight(
            const Surfel& surfel, const Cartesian3& outDir, const Light& light);
        RGBRadiance IndirectLight(
            const Surfel& surfel, const Cartesian3& outDir, 
            const RGBRadiance& combinedAlbedo, int& depth);
        
        // Monte Carlo integration, returns a direction vector
        Cartesian3 MonteCarlo3D(const Cartesian3& normal);
        
        // reflects a direction vector around a given normal
        Cartesian3 Reflect(const Cartesian3& dir, const Cartesian3& normal);

        // return a random number between a given range
        float RandomRange(float lower, float upper);

        // gives a random barycentric coordinate for a given triangle
        Cartesian3 GetRandomAreaLightPoint(const Light& light);

    public:
        // the image to write to
        RGBAImage* frameBuffer_;
        // render parameters set in UI
        RenderParameters* parameters_;
        // the objetc in the scene
        TexturedObject* object_;
        // the number of samples for indirect light integration
        float nSamples_;
        // a radiance buffer and its dimensions (from RGBAImage)
        Pixel* pixelBuffer_;
        long height_, width_;
        // randome number generator
        std::default_random_engine generator_;
};



#endif