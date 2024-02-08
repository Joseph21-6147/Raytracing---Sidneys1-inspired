// Raytracing! (by Sidneys1.com)

// article: https://sidneys1.com/programming/2022/03/23/raytracing.html

// got upto:

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

