/*
    CMPSC 457 Section 001
    Homework 7

    Alexander Petrov
    aop5448

    12/3/25

    Outputs an n_x by n_y ppm image containing three raytraced spheres
    usage: ./template [n_x] [n_y] [filename.ppm]
*/

#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cfloat>
#include <vector>
#include <glm/glm.hpp>


using namespace std;

using vec3 = glm::dvec3;



struct Ray
{
    // origin and direction of this ray
    vec3 o, d;

    // arg d should always be normalized vector
    Ray (vec3 o, vec3 d) : o(o), d(d) {}
};



// types of the surface
// - DIFFuse, SPECular, REFR: reflective
// - only DIFF is used in this assignment
enum Refl_t { DIFF, SPEC, REFR };

// small constant
const double eps = 1e-4;


struct Sphere
{
    double r;      // radius of sphere
    vec3 p;        // position (center) of sphere
    vec3 e;        // emission (not used in this assignment)
    vec3 c;        // color of sphere
    Refl_t refl;   // reflection type (DIFFuse, SPECular, REFRactive)
                   // - in this assignment, only DIFF is used

    Sphere(double r, vec3 p, vec3 e, vec3 c, Refl_t refl)
        : r{r}, p{p}, e{e}, c{c}, refl{refl} {}

    /** Check if a ray intersects with a sphere
     * @param ray raycast ray
     * @return t if ray intersects with t > eps, else 0
     */
    double intersect(const Ray& ray) const {
        /*
            Need to solve (d.d)t^2 + 2d.(o-c)t + (o-c).(o-c) - R^2 = 0
            ax^2 + bx + c = 0

            a = d.d
            b = 2d.(o-c)
            c = (o-c).(o-c) - R^2 = O^2 - R^2 (from ref link)
            x = t

            quadratic eq'n: (-b +- sqrt(delta))/2a
            delta = b^2 - 4ac

            references: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
                        + lecture
            if delta > 0:
                two t's
            if delta = 0:
                one t
            if delta < 0:
                sphere behind ray
        */

        double a = glm::dot(ray.d, ray.d);
        double b = glm::dot(2.0*ray.d, (ray.o - this->p));
        double c = glm::dot(ray.o - this->p, ray.o  - this->p) - this->r * this->r;
        double delta = b*b - 4*a*c;

        if(delta < -eps){
            return 0.0;

        } else if(delta > eps){ //delta > ~0
            double t0 = (-b + sqrt(delta))/(2*a);
            double t1 = (-b - sqrt(delta))/(2*a);
            return min(t0, t1);

        } else { //delta ~= 0
            return -b / (2.0*a);
        }
    }

    /** Retruns normal vector at point v on the sphere
     * @param v point on sphere
     * @returns unit normal vector
     */
    vec3 normal(vec3& v){
        return -glm::normalize(this->p - v);
    }
};


// Three spheres in the scene
vector<Sphere> spheres = {
    Sphere(200, vec3(  0, -300, -1200), vec3(), vec3(.8, .8, .8), DIFF),
    Sphere(200, vec3(-80, -150, -1200), vec3(), vec3(.7, .7, .7), DIFF),
    Sphere(200, vec3( 70, -100, -1200), vec3(), vec3(.9, .9, .9), DIFF)
};


vec3 eye(0, 0, 0);       // camera position
vec3 light(0, 0, 0);     // light source position
vec3 lightColor(1,1,1); // light source color (intensity)



// Does this ray hit any of the spheres?
// if so,
// - return t using the reference parameter
// - return the index of the surface using the reference parameter
// - return true
// if not,
// - return false
/** Detects raytrace hits
 * @param ray
 * @param t set when spheres[surface_idx] was hit
 * @param surface_idx set when spheres[surface_idx] was hit
 * @returns bool something was hit
 */
bool hit(const Ray& ray, double& t, int& surface_idx){
    // You must check if this ray intersect with each of the spheres
    // and keep track of the closest t and the index of the corresponding sphere
    // Return the closest t and the index using the reference arguments
    int bestHitId = -1;
    double bestHitT = INFINITY;

    for(uint i=0; i<spheres.size(); i++){
        double hitT = spheres[i].intersect(ray);
        if(hitT > eps && hitT < bestHitT){
            bestHitId = i;
            bestHitT = hitT;
        }
    }
    if(bestHitId == -1){
        return false;
    }

    t = bestHitT;
    surface_idx = bestHitId;
    return true;
}


/** Calculates the light intensity using Lambert's law
 * ie, dot product of l_hat and n_hat (or 0 if dot product is negative)
 * Note that n_hat is a unit normal vector at the point where the ray hits the surface
 *
 */
double lambert(int surface_idx, Ray& ray, double t){
    vec3 pHit = eye + t*ray.d;

    vec3 n_hat = glm::normalize(pHit - spheres[surface_idx].p);
    vec3 l_hat = glm::normalize(light - pHit);

    return max(glm::dot(n_hat, l_hat), 0.0);
}


// Calculate the color of the ray
// If ray hits any surface,
// - color of the ray is the Lambert's intensity * color at the hit point
// If not
// - color of the ray is the background color (black)
vec3 ray_color(Ray& ray)
{
    double t;
    int surface_idx;

    // If is_hit is true (i.e., the ray hit at least one of the spheres)
    // then, t is the t for the closest sphere hit by the ray
    //       surface_idx is the index of the corresponding sphere
    bool is_hit = hit(ray, t, surface_idx);
    if (is_hit) {
        return spheres[surface_idx].c * lambert(surface_idx, ray, t);
    }
    else {
        return vec3{0.0, 0.0, 0.0};
    }
}


// Simple ray tracer
void tracer(int nx, int ny, int d, double theta, ofstream& fout){
    // Generate a ppm image of size nx x ny
    //
    // 1. calculate h, w, scale_factor based on nx, ny, d, and theta
    // 2. for every pixel p' in nx x ny raster
    // 3.    transform p' = (x', y') in pixel coord to p = (x, y) in world coord
    // 4.    define a ray passing through the pixel
    //           (note: direction vector of the ray must be a unit vector)
    // 5.    calculate the color of the ray, ie, by calling ray_color()
    // 6.    use the color for pixel p'
    // 7. end for

    fout << "P3\n" <<
            nx << " " << ny << "\n" <<
            "255\n";

    double h = 2 * d * tan(theta/2);
    double w = ((double)nx/(double)ny) * h;
    double scalingRatio = w/nx;

    for(int yPix=0; yPix < ny; yPix++){
        for(int xPix=0; xPix < nx; xPix++){
            // Translate (-nx/2, -ny/2, -d)
            // Scale (w/nx, w/nx, 1)
            vec3 p = vec3(
                (xPix - (nx/2)) * scalingRatio,
                (yPix - (ny/2)) * scalingRatio,
                eye.z - d
            );

            Ray newRay = Ray(
                eye,
                glm::normalize(p-eye)
            );

            vec3 pixColor = ray_color(newRay);

            fout << static_cast<int>(pixColor.r * 255) << " " <<
                    static_cast<int>(pixColor.g * 255) << " " <<
                    static_cast<int>(pixColor.b * 255) << " ";
        }
    }

}




int main(int argc, char* argv[])
{
    if (argc != 4) {
	cerr << "Usage:  template nx ny outfile.ppm";
	exit(1);
    }

    int nx = std::stoi(argv[1], nullptr);
    int ny = std::stoi(argv[2], nullptr);
    char *fname = argv[3];

    ofstream fout(fname);
    if (!fout) {
        cerr << "tracer: cannot open input file " << fname << endl;
        exit(1);
    }

    // trace the ray to generate nx x ny image using
    //   the virtual film placed at the distance of 200 in z-axis (negative z direction)
    //   from the eye
    int d = 200;

    //   vfov of 60 degree ( = 60 * pi / 180 radian )
    double theta = 60 * M_PI / 180;

    tracer(nx, ny, d, theta, fout);


    fout.close();

    return 0;
}




