#include "math.h"
#include "Surfel.h"

// interpolate surfel properties from triangle data
void Surfel::InterpolateProperties(
    TexturedObject* object, RenderParameters* params) {
    // we dont normalise interpolated value as interpolation keeps length roughly 1
    normal_ = barycentric_.alpha * object->normals[triangle_->normals[0]] +
        barycentric_.beta * object->normals[triangle_->normals[1]] +
        barycentric_.gamma * object->normals[triangle_->normals[2]];

    // rotate the normal
    normal_ = params->rotationMatrix * normal_;
    
    texCoord_ = barycentric_.alpha * object->textureCoords[triangle_->texCoords[0]] +
        barycentric_.beta * object->textureCoords[triangle_->texCoords[1]] +
        barycentric_.gamma * object->textureCoords[triangle_->texCoords[2]];

    // We will want to change this part of the function later to interpolate vertex
    // data (means changing the input files etc), for now just set to UI values
    emission_ = RGBRadiance(object->materials[triangle_->material]->emmisive[0],
        object->materials[triangle_->material]->emmisive[1],
        object->materials[triangle_->material]->emmisive[2]);
    lambertAlbedo_ = RGBRadiance(object->materials[triangle_->material]->lambertian[0],
        object->materials[triangle_->material]->lambertian[1],
        object->materials[triangle_->material]->lambertian[2]);
    glossyAlbedo_ = RGBRadiance(object->materials[triangle_->material]->glossy[0],
        object->materials[triangle_->material]->glossy[1],
        object->materials[triangle_->material]->glossy[2]);
    glossyExponent_ = object->materials[triangle_->material]->glossy[3];
    impulseAlbedo_ = RGBRadiance(object->materials[triangle_->material]->albedo[0],
        object->materials[triangle_->material]->albedo[1],
        object->materials[triangle_->material]->albedo[2]);
    extinction_ = object->materials[triangle_->material]->extinction;
    impulse_ = object->materials[triangle_->material]->impulse;
}

// surface BRDF at the surfel
RGBRadiance Surfel::BRDF(const Cartesian3& outDir, const Cartesian3& inDir) const {
    // lambertian
    float lambertian = normal_.dot(inDir) / inDir.length();
    // glossy
    Cartesian3 outInDiv2 = (outDir + inDir) / 2.0;
    float glossy = pow(normal_.dot(outInDiv2) / outInDiv2.length(), glossyExponent_);

    // return lambertian + glossy component
    return lambertAlbedo_ * lambertian + glossyAlbedo_ * glossy;
}