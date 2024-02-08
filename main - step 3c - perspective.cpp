// Raytracing! (by Sidneys1.com)

// article: https://sidneys1.com/programming/2022/03/23/raytracing.html

// got upto:

#include <optional>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Game width and height (in pixels)
constexpr int WIDTH  = 250;
constexpr int HEIGHT = 250;

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

    // dot product (multiplication): vf3d * vf3d = float
    const float operator* (const vf3d right) const {
        return (x * right.x) + (y * right.y) + (z * right.z);
    }
    // subtraction: vf3d - vf3d = vf3d
    const vf3d operator- (const vf3d right) const {
        return { x - right.x, y - right.y, z - right.z };
    }
    // division: vf3d / float = fv3d
    const vf3d operator/ (float divisor) const {
        return { x / divisor, y / divisor, z / divisor };
    }
    // returns the length / magnitude of this vf3d
    const float magnitude() const {
        return sqrtf((*this) * (*this));
    }
    // return a normalized version of this vf3d (magnitude == 1)
    const vf3d normalize() const {
//        return (*this) / sqrtf((*this) * (*this));
        return (*this) / magnitude();
    }
};

// struct to describe a 3D floating point ray (vector with origin point)
struct ray {
    vf3d origin, direction;
    // default constructor
    ray() = default;
    // add explicit constructor that initializes origin and direction
    constexpr ray( const vf3d _origin, const vf3d _direction ) : origin( _origin ), direction( _direction ) {}

    // return a normalized version of this ray (magnitude == 1)
    const ray normalize() const {
        return { origin, direction.normalize() };
    }
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
        vf3d oc = r.origin - origin;
        float a = r.direction * r.direction;
        float b = 2.0f * (oc * r.direction);
        float c = (oc * oc) - (radius * radius);
        float discriminant = (b * b) - 4.0f * a * c;
        if (discriminant < 0.0f)
            return {};
        auto ret = (-b - sqrtf( discriminant )) / (2.0f * a);
        if (ret < 0)
            return {};
        return ret;
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

        shapes.emplace_back( std::make_unique<Sphere>( vf3d(    0,   0,  200 ), olc::GREY,  100.0f ));
        // add some additional Spheres at different positions
        shapes.emplace_back( std::make_unique<Sphere>( vf3d( -150, +75, +300 ), olc::RED,   100.0f ));
        shapes.emplace_back( std::make_unique<Sphere>( vf3d( +150, -75, +100 ), olc::GREEN, 100.0f ));

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
        olc::Pixel final_color;
        final_color = intersected_shape.sample(r);

        return final_color;
    }

    olc::Pixel rtSample( float x, float y ) const {
        // create a ray casting into the scene from this "pixel"
        ray sample_ray({ 0, 0, -800 }, { (x / float(WIDTH)) * 100, (y / float(HEIGHT)) * 100, 200 });
        // sample this ray - if the ray doesn't hit anything, use the color of the fog
        return SampleRay( sample_ray.normalize()).value_or( FOG );
    }


    bool OnUserDestroy() override {
        // your clean up code here
        return true;
    }
};

// keep the screen dimensions constant and vary the resolution by adapting the pixel size
//#define SCREEN_X    256
//#define SCREEN_Y    240
#define PIXEL_X       2
#define PIXEL_Y       2

int main()
{
	RayTracer demo;
	if (demo.Construct( WIDTH, HEIGHT, PIXEL_X, PIXEL_Y ))
		demo.Start();

	return 0;
}

