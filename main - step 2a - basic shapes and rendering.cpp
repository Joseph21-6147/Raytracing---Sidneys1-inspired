// Raytracing! (by Sidneys1.com)
// =============================
// part 2a: basic shapes and rendering

// article: https://sidneys1.com/programming/2022/03/23/raytracing.html

// Implementation by Joseph21 - february 8, 2024

// got upto and including: see the arrow (<<====) in below TOC

/* Table of Contents
 * =================
 * What is Raytracing?
 * How Do We Begin?
    - Creating a new olc::PixelGameEngine Project
 * Setting the Scene
    - Add basic Shapes and a vector of shapes to render    <<====
    - Add constants and a way to “Sample” single pixels
    - Add some geometry types, enhance Shape and Sphere
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

class Shape {
};

class Sphere : public Shape {
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

        shapes.emplace_back( std::make_unique<Sphere>());
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

