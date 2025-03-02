#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

#include <thread>
#include <future>
#include <mutex>

class camera {
public:
    double aspectRatio = 1.0;
    int imageWidth = 100;
    int samplesPerPixel = 10;
    int maxDepth = 10;

    double vfov = 90.0;
    point3 lookFrom = point3(0, 0, 0);
    point3 lookAt = point3(0, 0, -1);
    point3 vup = point3(0, 1, 0);

    double defocusAngle = 0;
    double focusDist = 10;

    int numThreads = 12;

    camera(hittable& world, int threads) : world(world), numThreads(threads) {}

    void render() {
        initialize();
        
        std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

        std::future<std::vector<shared_ptr<color>>>* threads = new std::future<std::vector<shared_ptr<color>>> [numThreads];
        for (int i = 0; i < numThreads; i++) {
            int start = imageHeight * i / numThreads;
            int end = imageHeight * (i + 1) / numThreads;
            threads[i] = std::async(&camera::threadWorkload, this, start, end);
        }

        for (int i = 0; i < numThreads; i++) {
            std::future<std::vector<shared_ptr<color>>>& t = threads[i];
            std::vector<shared_ptr<color>> cols = t.get();
            
            for (const auto& val : cols) {
                writeColor(std::cout, *val);
            }
        }

        delete[] threads;
        
        std::clog << "\rDone.                 \n";
    }

private:
    int imageHeight;
    double pixelSamplesScale;
    point3 centre, pixel00Loc;
    vec3 pixelDeltaU, pixelDeltaV, u, v, w;
    vec3 defocusDiskU, defocusDiskV;
    hittable& world;

    int progress = 0;
    std::mutex mtx;

    void initialize() {
        imageHeight = int(imageWidth / aspectRatio);
        imageHeight = (imageHeight < 1) ? 1 : imageHeight;

        centre = lookFrom;

        pixelSamplesScale = 1.0 / samplesPerPixel;

        //Viewport dimensions
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2.0);
        auto viewPortHeight = 2.0 * h * focusDist;
        auto viewPortWidth = viewPortHeight * (double(imageWidth) / imageHeight);

        w = unit_vector(lookFrom - lookAt);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        auto viewPortU = viewPortWidth * u;
        auto viewPortV = viewPortHeight * -v;

        pixelDeltaU = viewPortU / imageWidth;
        pixelDeltaV = viewPortV / imageHeight;

        //Calculate position of upper left pixel
        auto viewPortUpperLeft = centre - focusDist * w - viewPortU / 2 - viewPortV / 2;
        pixel00Loc = viewPortUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

        //Calculate defocus disc basis vectors
        auto defocusRadius = focusDist * std::tan(degrees_to_radians(defocusAngle/2));
        defocusDiskU = u * defocusRadius;
        defocusDiskV = v * defocusRadius;
    }

    std::vector<shared_ptr<color>> threadWorkload(int startRow, int endRow) {
        std::vector<shared_ptr<color>> vals;
        for (int i = startRow; i < endRow; i++) {
            for (int j = 0; j < imageWidth; j++) {
                color pixelColor(0, 0, 0);
                for (int sample = 0; sample < samplesPerPixel; sample++) {
                    ray r = getRay(j, i);
                    pixelColor += ray_color(r, maxDepth, world);
                }
                vals.push_back(make_shared<color>(pixelSamplesScale * pixelColor));
            }
            mtx.lock();
            progress += 1;
            std::clog << "\rScanlines remaining: " << (imageHeight - progress) << ' ' << std::flush;
            mtx.unlock();
        }
        return vals;
    }

    ray getRay(int i, int j) const {
        auto offset = sampleSquare();
        auto pixelSample = pixel00Loc + ((i + offset.x()) * pixelDeltaU) + ((j + offset.y()) * pixelDeltaV);

        auto rayOrigin = (defocusAngle <= 0) ? centre : defocusDiskSample();
        auto rayDirection = pixelSample - rayOrigin;

		auto ray_time = random_double();

        return ray(rayOrigin, rayDirection, ray_time);
    }

    point3 defocusDiskSample() const{
        auto p = random_on_unit_disk();
        return centre + (p[0] * defocusDiskU) + (p[1] * defocusDiskV);
    }

    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
    vec3 sampleSquare() const { return vec3(random_double() - 0.5, random_double() - 0.5, 0); }

    color ray_color(const ray& r, int depth, const hittable& world) {
        if (depth <= 0) return color(0, 0, 0);

        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;

            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_color(scattered, depth - 1, world);
            }
            return color(0, 0, 0);
        }

        vec3 unitDirection = unit_vector(r.direction());
        auto a = 0.5 * (unitDirection.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};


#endif // !CAMERA_H

