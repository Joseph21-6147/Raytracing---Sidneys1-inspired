// Raytracing! (by Sidneys1.com)
// =============================
// part 1: setup PixelGameEngine

// article: https://sidneys1.com/programming/2022/03/23/raytracing.html

// Implementation by Joseph21 - february 8, 2024

// got upto and including: see the arrow (<<====) in below TOC

/* Table of Contents
 * =================
 * What is Raytracing?
 * How Do We Begin?
    - Creating a new olc::PixelGameEngine Project    <<====
 * Setting the Scene
    - Add basic Shapes and a vector of shapes to render
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
constexpr int WIDTH  = 256;
constexpr int HEIGHT = 240;
// pixel width and height
constexpr int PIXEL_X = 2;
constexpr int PIXEL_Y = 2;

class RayTracer : public olc::PixelGameEngine {

public:
    RayTracer() {
        sAppName = "RayTracer";
    }

public:
    bool OnUserCreate() override {

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {
		// Called once per frame

		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));

		return true;
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

