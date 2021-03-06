// c++ default libraries
#include <limits>
#include <cmath>
#include <chrono>
#include <thread>

#include "RayTracer.h"
#include "Matrix4.h"


// pointer argument to directly modify the frame buffer in the widget
void RayTracer::RayTraceImage() {
    unsigned int availableThreads = std::thread::hardware_concurrency();
    // arbitrary position for the eye (movable with a slider?)
    Cartesian3 eyePos(0, 0, 3);
    
    // get number of samples from parameters
    nSamples_ = parameters_->samples_;
    
    // initialise the random number generator seed with current time
    generator_ = std::default_random_engine(
        std::chrono::system_clock::now().time_since_epoch().count());

    // scale for vertices
    float scale = parameters_->zoomScale;
    if (parameters_->scaleObject)
        scale /= object_->objectSize;

    // compute the transforms affecting the model
    Matrix4 objectTransform;
    objectTransform = GetTransform(false, scale);
    // then apply it to the model's vertices here once, rather than every time we
    // test a triangle later
    for (unsigned int vertex = 0; vertex < object_->vertices.size(); vertex++) {
        object_->vertices[vertex] = objectTransform * object_->vertices[vertex] * scale;
    }

    // compute aspect ratio from frame buffer dimensions
    height_ = frameBuffer_->height;
    width_ = frameBuffer_->width;
    float aspectRatio = (float)height_ / (float)width_;

    // create a buffer containing the pixels
    pixelBuffer_ = (Pixel*)calloc(height_*width_,sizeof(Pixel));
    
    // loop over the pixel buffer and initialise values
    for (long i = 0; i < height_; i++)
        for (long j = 0; j < width_; j++) {
            pixelBuffer_[i*width_+j].worldPos = Cartesian3 (
                2.0f * j / (float)width_ - 1.0f, 
                2.0f * i / (float)height_ - 1.0f, 1.0f);
            // accomodate for aspect ratio distorsion
            if (aspectRatio > 1.0)
                pixelBuffer_[i*width_+j].worldPos.y *= aspectRatio;
            else
                pixelBuffer_[i*width_+j].worldPos.x *= aspectRatio;
            pixelBuffer_[i*width_+j].radiance = RGBRadiance();
        }
    
    if (parameters_->showObject) {
        // divide up the image (very crudely done here, ideally  should afford 
        // less rows to the thread if ray intersects a lot of geometry, use opengl
        // image as a guide?)
        unsigned long const rowsPerThread = frameBuffer_->height / availableThreads;
        unsigned long firstRow = 0;
        // create a container for the threads
        std::vector<std::thread> threads(availableThreads - 1);

        // start timer
        auto start = std::chrono::high_resolution_clock::now();

        // loop over available threads, minus the current thread
        for (unsigned int thread = 0; thread < availableThreads - 1; thread++) {
            // create a thread and pass a reference to this raytracer class and 
            // call the raytrace pixels function with the assigned pixel rows
            threads[thread] = std::thread(
                &RayTracer::RayTracePixelsThread, this, firstRow, rowsPerThread, eyePos);
            firstRow += rowsPerThread;
        }
        // also use the current thread
        if (frameBuffer_->height % 2)
            RayTracePixelsThread(firstRow, rowsPerThread + 1, eyePos);
        else
            RayTracePixelsThread(firstRow, rowsPerThread, eyePos);

        // join up threads
        for (unsigned int thread = 0; thread < availableThreads - 1; thread++)
            threads[thread].join();

        // now set the RGBImage with radiance buffer values, also divide by samples
        for (long i = 0; i < height_; i++)
            for (long j = 0; j < width_; j++) 
                frameBuffer_->block[i*width_+j] = 
                    (pixelBuffer_[i*width_+j].radiance / nSamples_).ToRGBAValue();

        Ray ray = Ray();
        Surfel surfel;
        ray.origin_ = eyePos;
        // Add a final pass to set the ligh pixels
        for (long i = 0; i < height_; i++)
            for (long j = 0; j < width_; j++) {
                // ray from eye to infinity passing through the pixel in world space
                // compute the radiance of the pixel
                ray.direction_ = (pixelBuffer_[i*width_+j].worldPos - eyePos).unit();
                // create a surfel at the intersection point of the ray with the scene
                if (!ClosestTriangleIntersect(ray, &surfel))
                    continue;
                
                if (surfel.triangle_->lightId != 0)
                    frameBuffer_->block[i*width_+j] = RGBAValue(255.0f, 255.0f, 255.0f);

            }

        // end timer
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Render took: " << 
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
            << "ms." << std::endl;
    }

    // free memory
    free(pixelBuffer_);

    // now compute the inverse transform affecting the model
    objectTransform = GetTransform(true, scale);
    // then apply it to the model's vertices to undo the previous transform
    for (unsigned int vertex = 0; vertex < object_->vertices.size(); vertex++) {
        object_->vertices[vertex] = objectTransform * object_->vertices[vertex] / scale;
    }
}

// a sub function that renders a section of the image
void RayTracer::RayTracePixelsThread(
    const long& begin, const long& rows, const Cartesian3& eyePos) {
    RGBRadiance pixelRadiance;
    Ray ray = Ray();
    ray.origin_    = eyePos;
    // loop as many samples as desired
    for(unsigned int sample = 0; sample < nSamples_; sample++) { 
        // loop over pixels
        for (long i = begin; i < begin + rows; i++)
            for (long j = 0; j < width_; j++) {
                // ray from eye to infinity passing through the pixel in world space
                // compute the radiance of the pixel
                ray.direction_ = (pixelBuffer_[i*width_+j].worldPos - eyePos).unit();
                int depth = 0;
                pixelRadiance = PathTrace(ray, RGBRadiance(1.0f,1.0f,1.0f), ++depth);

                // accumulate radiance at the pixel
                pixelBuffer_[i*width_+j].radiance = 
                    pixelBuffer_[i*width_+j].radiance + pixelRadiance;
            }
    }
}


// returns the radiance for a ray  
RGBRadiance RayTracer::PathTrace(const Ray& ray, const RGBRadiance& combinedAlbedo, int& depth) {
    // initialise with a radiance of 0
    RGBRadiance totalRadiance;
    // initialise the surfel
    Surfel surfel;

    // test for albedo termination
    if (combinedAlbedo.RadianceSum() < EPSILON)
        return totalRadiance;

    // create a surfel at the intersection point of the ray with the scene
    if (!ClosestTriangleIntersect(ray, &surfel)) {
        // returns true if invalid (no triangle, so return 0 radiance)
        return totalRadiance;
    }

    // interpolate surfel properies using barycentric coordinates
    surfel.InterpolateProperties(object_, parameters_);

    // compute lighting 
    for (unsigned int light = 0; light < object_->lights.size(); light++) {
        totalRadiance = totalRadiance + DirectLight(surfel, -ray.direction_, 
            *object_->lights[light]);
    }

    // ambient light
    totalRadiance = totalRadiance + IndirectLight(surfel, -ray.direction_, 
        combinedAlbedo, depth);

    // set textures if triangle is textured
    //if (parameters_->texturedRendering && surfel.triangle_->texID) {
        // get texture and set outgoing radiance to texture colour, should be
        // surfel texture rather than object, but leave it for now
        // NB we subtract 1 since the indices are always 1 to high to allow us
        // to use 0 as the "no texture" value
        /*
        totalRadiance = totalRadiance.modulate(RGBRadiance((*object_->textures[surfel.triangle_->texID-1])
            [(int)(surfel.texCoord_.y * object_->textures[surfel.triangle_->texID-1]->height)]
            [(int)(surfel.texCoord_.x * object_->textures[surfel.triangle_->texID-1]->width)]));
        */
        //return totalRadiance;
    //}
    return totalRadiance;
}

// returns the compound transform that encodes how the object has been transformed
// by the user through the UI
Matrix4 RayTracer::GetTransform(const bool& inverse, const float& scale) {
    // a matrix where all the transformations are stored
    Matrix4 renderTransform;
    renderTransform.SetIdentity();

    if (inverse) {
        // first apply centring translation
        if (parameters_->centreObject) {
            Matrix4 centreTransform;
            centreTransform.SetTranslation(object_->centreOfGravity * scale);
            renderTransform = renderTransform * centreTransform;
        }        
        // then apply transpose of model arcball rotation 
        renderTransform = parameters_->rotationMatrix.transpose() * renderTransform;
        // finally apply x y translations
        Matrix4 translations;
        translations.SetTranslation(
            Cartesian3(parameters_->xTranslate, parameters_->yTranslate, 0.0f));
        renderTransform = translations * renderTransform;
    }
    else {        
        // first apply slider x and y translation
        Matrix4 translations;
        translations.SetTranslation(
            Cartesian3(parameters_->xTranslate, parameters_->yTranslate, 0.0f));
        renderTransform = translations * renderTransform;

        // then apply model arcball rotation 
        renderTransform = parameters_->rotationMatrix * renderTransform;

        // finally apply the centre transform
        if (parameters_->centreObject) {
            Matrix4 centreTransform;
            centreTransform.SetTranslation(object_->centreOfGravity * -scale);
            renderTransform = renderTransform * centreTransform;
        }
    }
    // return compound transformation
    return renderTransform;
}

// computes the closest triangle along the ray and returns the ray's intersection
// with the triangle as a surfel
bool RayTracer::ClosestTriangleIntersect(const Ray& ray, Surfel* surfel) {
    // initialise an empty surfel that does not belong to a triangle
    // start with distance to eye as infinity
    surfel->distanceToEye = std::numeric_limits<float>::infinity();    
    Cartesian3 v0, v1, v2, u, v, w, normal, intersect;
    for (unsigned int tri = 0; tri < object_->faces.size(); tri++) {
        // construct a plane from the triangle
        v0 = object_->vertices[object_->faces[tri]->vertices[0]];
        v1 = object_->vertices[object_->faces[tri]->vertices[1]];
        v2 = object_->vertices[object_->faces[tri]->vertices[2]];
        // vector u from vertex 0 to 1
        u = v1 - v0;
        // vector w from vertex 1 to 2
        v = v2 - v1;
        // vector v from vertex 2 to 0
        w = v0 - v2;
        // cross product of u and -w gives plane normal
        normal = u.cross(-w).unit(); 

        // ray is parallel to plane so definitely no intersection, also skip back facing normals
        float rayDotNormal = ray.direction_.dot(normal);
        if (rayDotNormal > EPSILON)
            continue;

        // compute parameter t for ray intersection on plane and compute it
        intersect = ray.at((v0 - ray.origin_).dot(normal) / rayDotNormal);

        // compute barycentric alpha and beta, which we use to...
        float alpha = normal.dot(v.cross(intersect - v1));
        float beta = normal.dot(w.cross(intersect - v2));
        // ... perform half plane test here
        if ((alpha < 0) || (beta < 0) || (normal.dot(u.cross(intersect - v0)) < 0))
            continue;

        // if we got this far, we found a valid triangle. we create a new surfel
        // if the distance to the eye is smaller than current distance
        float d = (intersect - ray.origin_).length();
        if (d < surfel->distanceToEye) {
            // replace previous surfel with a new one that is closer
            surfel->position_ = intersect;
            surfel->normal_ = normal.unit(); // normal computed for us
            surfel->triangle_ = object_->faces[tri];
            // we know alpha and beta, and by extension, gamma from the half-plane test
            float normalDotNormal = 1.0 / normal.dot(normal) ;
            surfel->barycentric_.alpha =  alpha * normalDotNormal;
            surfel->barycentric_.beta = beta * normalDotNormal;
            surfel->barycentric_.gamma = 1.0 - surfel->barycentric_.alpha - surfel->barycentric_.beta;
            // update distance to eye
            surfel->distanceToEye = d;
            surfel->isValid = true;
            // use the truthiness of the light id the surfel belongs to
            surfel->isLight_ = surfel->triangle_->lightId; // will be 0 if not light
        }
    }
    return surfel->isValid;
}

// method for computing direct light
RGBRadiance RayTracer::DirectLight(
    const Surfel& surfel, const Cartesian3& outDir, const Light& light) {
    // incoming light direction (from light to surfel) and position
    Cartesian3 lightPos;
        
    if (light.isAreaLight) 
        lightPos = GetRandomAreaLightPoint(light);
    else
        lightPos = light.position;

    Cartesian3 inDir = (lightPos - surfel.position_).unit();
    // compute shadow rays
    Surfel shadowSurfel;
    // check if surfel are on the same triangle, if not then light is blocked
    if (ClosestTriangleIntersect(Ray(lightPos, -inDir), &shadowSurfel)
         && shadowSurfel.triangle_->id != surfel.triangle_->id) {
            return RGBRadiance(); // no light
    }

    // check that surfel is on area light, if so return the light
    // compute attenuation
    float distsqr;
    if (light.atInfinity)
        distsqr = 1;
    else
        distsqr = inDir.dot(inDir);
        
    // return intensity
    return surfel.BRDF(outDir, inDir) * light.intensity / distsqr;
}

// method for computing indirect light
RGBRadiance RayTracer::IndirectLight(
    const Surfel& surfel, const Cartesian3& outDir, const RGBRadiance& combinedAlbedo, int& depth) {

    // probabislistic extinction coefficient
    if (RandomRange(0.0f, 1.0f) < surfel.extinction_)
        return RGBRadiance();

    // declare direction and albedo
    Cartesian3 indirectDir;
    RGBRadiance albedo;

    // uniform distribution so if impulse is at 0.6, then there is a 60% chance to
    // go through impulse code path
    if (RandomRange(0.0f, 1.0f) < surfel.impulse_) {
        // 
        //indirectDir = 2.0f * surfel.normal_ - outDir; // perfect reflection
        indirectDir = Reflect(-outDir, surfel.normal_);
        albedo = surfel.impulseAlbedo_;
    }
    else {
        // compute indirect radiance at the pixel
        indirectDir = MonteCarlo3D(surfel.normal_); // random vector on hemisphere
        albedo = surfel.BRDF(outDir, indirectDir);
    }
    
    // compute lighting at point
    RGBRadiance inLight = PathTrace(Ray(surfel.position_, indirectDir), combinedAlbedo * albedo, ++depth);
    // return the albedo scaled by incoming light
    return inLight * albedo;
}


// Monte Carlo integration, always returns a unit direction vector pointing in normal direction
Cartesian3 RayTracer::MonteCarlo3D(const Cartesian3& normal) {
    Cartesian3 direction;
    float u, v, length;
    // loop till we find a valid direction
    while (true) {
        /*
        u = RandomRange(0.0f, 1.0f);
        v = RandomRange(0.0f, 1.0f);
        // compute direction on hemisphere
        direction.x = std::cos(2.0f * PI * u);
        direction.y = v;
        direction.z = std::sin(2.0f * PI * u);
        */
        direction.x = RandomRange(0.0f, 2.0f) - 1.0f;
        direction.y = RandomRange(0.0f, 2.0f) - 1.0f;
        direction.z = RandomRange(0.0f, 2.0f) - 1.0f;
        // compute and compare length
        length = direction.length();
        if ((length < 0.1f) || (length > 1.0f))
            continue;
        // loop again if direction points away from the normal
        if (direction.dot(normal) < EPSILON)
            continue;
        return direction / length;
    }
    // keep compiler happy :-)
    return direction;
}

Cartesian3 RayTracer::Reflect(const Cartesian3& dir, const Cartesian3& normal) {
    // assumes unit normal
    return dir - 2.0f * (dir.dot(normal)) * normal;
}

// random number generator in range [lower, upper]
float RayTracer::RandomRange(float lower, float upper) {
    // initialise the generator with the seed
    std::uniform_real_distribution<float> distribution(lower, upper);
    return distribution(generator_);
}

// returns valid barycentric coordinates for any triangle
Cartesian3 RayTracer::GetRandomAreaLightPoint(const Light& light) {
    float alpha, beta, sum;
    // loop till we get valid barycentric coordinates
    while (true) {
        alpha = RandomRange(0.0f, 1.0f);
        beta = RandomRange(0.0f, 1.0f);
        sum = alpha + beta;
        // loop again if barycentric coordinates are invalid
        if ((sum >= 0.0f) && (sum <= 1.0f))
            break;
    }
    // compute the point on the triangle from light's vertices
    Cartesian3 point = 
        object_->vertices[light.triangle->vertices[0]] * alpha +
        object_->vertices[light.triangle->vertices[1]] * beta + 
        object_->vertices[light.triangle->vertices[2]] * (1.0f - alpha - beta);
    return point;        
}





