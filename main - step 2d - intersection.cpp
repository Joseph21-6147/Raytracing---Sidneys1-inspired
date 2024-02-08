// Raytracing! (by Sidneys1.com)
// =============================
// part 2d: intersection

// article: https://sidneys1.com/programming/2022/03/23/raytracing.html

// Implementation by Joseph21 - february 8, 2024

// got upto and including: see the arrow (<<====) in below TOC

/* Table of Contents
 * =================
 * What is Raytracing?
 * How Do We Begin?
    - Creating a new olc::PixelGameEngine Project
 * Setting the Scene
    - Add basic Shapes and a vector of shapes to render
    - Add constants and a way to “Sample” single pixels
    - Add some geometry types, enhance Shape and Sphere
    - Add fog color and a way to sample rays
    - Add intersection and sample methods to Shapes        <<====
 * Rendering Shapes
    - Implement ray-Sphere intersection
    - Add perspective rendering and depth sorting
    - Add a Plane Shape, and apply fog
 * Prettying Up
    - Add reflections
    - Create and use a color3 type
    - Add diffuse lighting
    - Add shadow casting
    - Add multisampling
 * All Done!
 */

#include <optional>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Game width and height (in pixels)
constexpr int WIDTH  = 250;
constexpr int HEIGHT = 250;
// pixel width and height
constexpr int PIXEL_X = 2;
constexpr int PIXEL_Y = 2;
// Half the game width and height (to identify the center of the screen)
constexpr float HALF_WIDTH  = WIDTH  / 2.0f;
constexpr float HALF_HEIGHT = HEIGHT / 2.0f;


// struct to describe a 3D floating point vector
struct vf3d {
    float x, y, z;
    // default constructor
    vf3d() = default;
    // explicit constructor that initializes x, y and z
    constexpr vf3d( float _x, float _y, float _z ) : x(_x), y(_y), z(_z) {}
    // explicit constructor that initializes x, y and z to the same value
    constexpr vf3d( float f ) : x(f), y(f), z(f) {}
};

// struct to describe a 3D floating point ray (vector with origin point)
struct ray {
    vf3d origin, direction;
    // default constructor
    ray() = default;
    // add explicit constructor that initializes origin and direction
    constexpr ray( const vf3d _origin, const vf3d _direction ) : origin( _origin ), direction( _direction ) {}
};


class Shape {
public:
    vf3d origin;
    olc::Pixel fill;
    // delete the default constructor (we'll never have a Shape with a default origin and color)
    Shape() = delete;
    // add explicit constructor that initializes origin and fill
    Shape( vf3d _origin, olc::Pixel _fill ) : origin( _origin ), fill( _fill ) {}

    // get the color of this Shape (when intersecting with a given ray)
    virtual olc::Pixel sample( ray sample_ray ) const { return fill; }

    // determine how far along a given ray this Shape intersects (if at all)
    virtual std::optional<float> intersection( ray r ) const = 0;
};

class Sphere : public Shape {
public:
    float radius;
    // delete the default constructor
    Sphere() = delete;
    // add explicit construtor that initializes Shape::origin, Shape::fill and Shape::radius
    Sphere( vf3d _origin, olc::Pixel _fill, float _radius ) : Shape( _origin, _fill ), radius(_radius ) {}

    // determine how far along a given ray this Sphere intersects (if at all)
    std::optional<float> intersection( ray r ) const override {
        // TODO: implement ray-sphere intersection
        return {};
    }
};


class RayTracer : public olc::PixelGameEngine {

public:
    RayTracer() {
        sAppName = "RayTracer";
    }

private:
    std::vector<std::unique_ptr<Shape>> shapes;

    // a color representing scene fog
    olc::Pixel FOG = olc::Pixel( 128, 128, 128 );

public:
    bool OnUserCreate() override {

        shapes.emplace_back( std::make_unique<Sphere>(vf3d(0, 0, 200), olc::GREY, 100 ));
        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {
		// Called once per frame

		// Iterate over the rows and columns of the scene
        for (int y = 0; y < HEIGHT; y++) {
		    for (int x = 0; x < WIDTH; x++) {
                // Sample this specific pixel (converting screen coordinates to scene coordinates)
                auto color = rtSample( x - HALF_WIDTH, y - HALF_HEIGHT );
				Draw(x, y, color);
		    }
        }

		return true;
    }

    std::optional<olc::Pixel> SampleRay( const ray r ) const {
        // called to get the color produced by a specific ray
        // this will be the color we (eventually) return
        olc::Pixel final_color;

        // store a pointer to the Shape this ray intersects with
        auto intersected_shape_iterator = shapes.end();
        // also store the distance along the ray where the intersection occurs
        float intersection_distance = INFINITY;

        // iterate over all Shapes in our scene
        for (auto it = shapes.begin(); it != shapes.end(); it++) {
            // if the distance is not undefined (meaning no intersection)...
            if (
                std::optional<float> distance = (*it)->intersection(r).value_or(INFINITY);
                distance < intersection_distance
            ) {
                // save the current shape as the intersected shape!
                intersected_shape_iterator = it;
                // also save the distance along the ray that this intersection occurred
                intersection_distance = distance.value();
            }
        }
        // if we didn't intersect with any Shapes, return an empty optional
        if (intersected_shape_iterator == shapes.end())
            return {};
        // else get the shape we discovered
        const Shape &intersected_shape = **intersected_shape_iterator;
        // set our color to the sampled color of the Shape that intersects with this ray
        final_color = intersected_shape.sample(r);

        return final_color;
    }

    olc::Pixel rtSample( float x, float y ) const {
        // create a ray casting into the scene from this "pixel"
		ray sample_ray({ x, y, 0 }, { 0, 0, 1 });
        // sample this ray - if the ray doesn't hit anything, use the color of the fog

        return SampleRay( sample_ray ).value_or( FOG );
    }

    bool OnUserDestroy() override {
        // your clean up code here
        return true;
    }
};

int main()
{
	RayTracer demo;
	if (demo.Construct( WIDTH, HEIGHT, PIXEL_X, PIXEL_Y ))
		demo.Start();

	return 0;
}

