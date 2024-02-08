// Raytracing! (by Sidneys1.com)
// =============================
// part 2b: elaborated shapes and rendering

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
    - Add some geometry types, enhance Shape and Sphere    <<====
    - Add fog color and a way to sample rays
    - Add intersection and sample methods to Shapes
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
};

class Sphere : public Shape {
public:
    float radius;
    // delete the default constructor
    Sphere() = delete;
    // add explicit construtor that initializes Shape::origin, Shape::fill and Shape::radius
    Sphere( vf3d _origin, olc::Pixel _fill, float _radius ) : Shape( _origin, _fill ), radius(_radius ) {}
};


class RayTracer : public olc::PixelGameEngine {

public:
    RayTracer() {
        sAppName = "RayTracer";
    }

private:
    std::vector<std::unique_ptr<Shape>> shapes;

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

    olc::Pixel rtSample( float x, float y ) const {
		// Create a ray casting into the scene from this "pixel".
		ray sample_ray({ x, y, 0 }, { 0, 0, 1 });
		// TODO: We now need to test if this ray hits any Shapes and produces
		//       a color.

        // Called to get the color of a specific point on the screen
        // For now we're returning a color based on the screen coordinates.
        return olc::Pixel( std::abs( x * 255 ), std::abs( y * 255 ), 0 );
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

