// Raytracing! (by Sidneys1.com)

// article: https://sidneys1.com/programming/2022/03/23/raytracing.html

// got upto:

#include <optional>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define DEBUG

// Game width and height (in pixels)
constexpr int WIDTH  = 250;
constexpr int HEIGHT = 250;

// Half the game width and height (to identify the center of the screen)
constexpr float HALF_WIDTH  = WIDTH  / 2.0f;
constexpr float HALF_HEIGHT = HEIGHT / 2.0f;

// Fog distance and reciprocal (falloff)
constexpr float FOG_INTENSITY_INVERSE = 3000.0f;
constexpr float FOG_INTENSITY = 1.0f / FOG_INTENSITY_INVERSE;

#ifdef DEBUG
constexpr int BOUNCES = 3;
#else
constexpr int BOUNCES = 7;
#endif // DEBUG

// lighting
constexpr float AMBIENT_LIGHT = 0.5f;

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
    // addition: vf3d + vf3d = vf3d
    const vf3d operator+ (const vf3d right) const {
        return { x + right.x, y + right.y, z + right.z };
    }
    // subtraction: vf3d - vf3d = vf3d
    const vf3d operator- (const vf3d right) const {
        return { x - right.x, y - right.y, z - right.z };
    }
    // multiplication: vf3d * float = fv3d
    const vf3d operator* (float factor) const {
        return { x * factor, y * factor, z * factor };
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
        return (*this) / magnitude();
    }
};

// use a type alias to use vf3d and color3 interchangeably
using color3 = vf3d;

// colors
color3 DARK_GREY(  0.5f );
color3 LIGHT_GREY( 0.8f );
color3 GREY(       0.7f );

color3 RED(   1.0f, 0.0f, 0.0f );
color3 GREEN( 0.0f, 1.0f, 0.0f );

// a color representing scene fog
color3 FOG = DARK_GREY;

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
    // multiplication: ray * float = ray
    const ray operator* (float right) const {
        return { origin, direction * right };
    }
    // return the vf3d at the end of this ray
    const vf3d end() const {
        return origin + direction;
    }
};

class Shape {
public:
    vf3d origin;
    color3 fill;
    float reflectivity;

    /* CONSTRUCTORS */

    // delete the default constructor (we'll never have a Shape with a default origin and color)
    Shape() = delete;
    // add explicit constructor that initializes origin and fill
    Shape( vf3d _origin, color3 _fill, float _reflectivity = 0.0f ) : origin( _origin ), fill( _fill ), reflectivity( _reflectivity ) {}

    // get the color of this Shape (when intersecting with a given ray)
    virtual color3 sample( ray sample_ray ) const { return fill; }

    // determine how far along a given ray this Shape intersects (if at all)
    virtual std::optional<float> intersection( ray r ) const = 0;

    // determine the surface normal of this Shape at a given intersection point
    virtual ray normal( vf3d incident ) const = 0;
};

class Sphere : public Shape {
public:
    float radius;
    // delete the default constructor
    Sphere() = delete;
    // add explicit construtor that initializes Shape::origin, Shape::fill and Shape::radius
    Sphere( vf3d _origin, color3 _fill, float _radius, float _reflectivity = 0.0f ) : Shape( _origin, _fill, _reflectivity ), radius(_radius ) {}

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

    // return the surface normal of this Sphere at a given intersection point
    ray normal( vf3d incident ) const override {
        return { incident, (incident - origin).normalize() };
    }
};

// subclass of Shape that represents a flat Plane
class Plane : public Shape {
public:
    vf3d direction;

    // add explicit constructor that initializes
    Plane( vf3d origin, vf3d _direction, color3 fill ) : Shape( origin, fill ), direction( _direction ) {}

    // determine how far along a give ray this Plane intersects (if at all)
    std::optional<float> intersection( ray sample_ray ) const override {
        auto denom = direction * sample_ray.direction;
        if (fabs( denom ) > 0.001f) {
            auto ret = (origin - sample_ray.origin) * direction / denom;
            if (ret > 0) return ret;
        }
        return {};
    }

    // get the color of this Plane (when intersecting with a given ray)
    // we're overriding this to provide a checkerboard pattern
    color3 sample( ray sample_ray ) const override {
        // get the point of intersection
        auto intersect = (sample_ray * intersection( sample_ray ).value_or( 0.0f )).end();

        // get the distance along the X and Z axis from the origin to the intersection
        float diffX = origin.x - intersect.x;
        float diffZ = origin.z - intersect.z;

        // get the XOR the signedness of the differences along X and Z
        // This allows us to "invert"the +X, -Z and -X, +Z quadrants
        bool color = (diffX < 0) & (diffZ < 0);

        // flip the "color" boolean if diff % 100 < 50 (e.g. flip one half of eacht 100-unit span)
        if (fmod( fabs( diffZ ), 100) < 50) color = !color;
        if (fmod( fabs( diffX ), 100) < 50) color = !color;

        // if we're coloring this pixel, return the fill - otherwise return DARK_GREY
        if (color)
            return fill;
        return DARK_GREY;
    }

    // return the surface normal of this Plane at a given intersection point
    ray normal( vf3d incident ) const override {
        return { incident, direction };
    }
};

class RayTracer : public olc::PixelGameEngine {

public:
    RayTracer() {
        sAppName = "RayTracer";
    }

private:
    std::vector<std::unique_ptr<Shape>> shapes;

    // the position of our point light
    vf3d light_point;

public:
    bool OnUserCreate() override {

        shapes.emplace_back( std::make_unique<Sphere>( vf3d(    0,   0,  200 ), GREY,  100.0f, 0.9f ));
        // add some additional Spheres at different positions
        shapes.emplace_back( std::make_unique<Sphere>( vf3d( -150, +75, +300 ), RED,   100.0f, 0.5f ));
        shapes.emplace_back( std::make_unique<Sphere>( vf3d( +150, -75, +100 ), GREEN, 100.0f ));

        // add a "floor" Plane
        shapes.emplace_back( std::make_unique<Plane>(vf3d( 0, 200, 0 ), vf3d( 0, -1, 0 ), color3( 0.8f )));

        light_point = { 0, -500, -500 };

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {
		// Called once per frame

        // create some static storage to accumulate elapsed time...
        static float accumulated_time = 0.0f;
        // ... and accumulate elapsed time into it
        accumulated_time += fElapsedTime;

        // update the position of our first Sphere evere update
        // sin/cos = easy, cheap motion
        Shape &shape = *shapes.at(0);
        shape.origin.y = sinf( accumulated_time ) * 100 - 100;
        shape.origin.z = cosf( accumulated_time ) * 100 + 100;

		// Iterate over the rows and columns of the scene
        for (int y = 0; y < HEIGHT; y++) {
		    for (int x = 0; x < WIDTH; x++) {
                // Sample this specific pixel (converting screen coordinates to scene coordinates)
                auto color = rtSample( x - HALF_WIDTH, y - HALF_HEIGHT );
				Draw(x, y, olc::PixelF( color.x, color.y, color.z ));
		    }
        }

		return true;
    }

    std::optional<color3> SampleRay( ray r, int bounces ) const {

        bounces -= 1;
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

        // quick check - if the intersection is further away than the furthest Fog point,
        // then we can save some time and not calculate anything further, since it would
        // be obscured by Fog regardless.
        if (intersection_distance >= FOG_INTENSITY_INVERSE)
            return FOG;

        // set our color to the sampled color of the Shape that intersects with this ray
        color3 final_color;
        final_color = intersected_shape.sample(r);

        // determine the point at which our ray intersects the Shape
        vf3d intersection_point = (r * intersection_distance).end();
        // calculate the normal of the given Shape at that point
        ray normal = intersected_shape.normal( intersection_point );

        // apply reflection
        if (bounces != 0 && intersected_shape.reflectivity > 0.0f) {

            // our reflection ray starts out as our normal
            ray reflection = normal;
            // apply a slight offset *along* the normal. This way our reflected ray will start at
            // some slight offset from the surface so that rounding errors don't cause it to collide
            // with the Shape it originated from
            reflection.origin = reflection.origin + (normal.direction * 0.001f);
            // reflect the direction around the normal with some simple geometry
            reflection.direction = (normal.direction * (2 * ((r.direction * -1.0f) * normal.direction)) + r.direction).normalize();
            // recursion! since the SampleRay doesn't care if the ray is coming from the canvas,
            // we can use it to get the color that will be reflected by this Shape
            std::optional<color3> reflected_color = SampleRay( reflection, bounces );

            // finally, mix our Shape's colour with the reflected color (or Fog color, in case of a miss)
            // according to the reflectivity
            final_color = lerp( final_color, reflected_color.value_or( FOG ), intersected_shape.reflectivity );
        }

        // apply diffuse lighting

        // first get the normalized ray from our intersection point to the light source
        ray light_ray = ray( intersection_point, light_point - intersection_point ).normalize();
        // next compute the dot product between surface normal and light ray
        // clamp this to prevent negative values
        // additionally, add in the ambient light so no surfaces are entirely dark
        float dot = std::clamp( AMBIENT_LIGHT + (light_ray.direction * normal.direction), 0.0f, 1.0f );
        // multiplying final color by resulting dot product darkens surfaces pointing away from the light
        final_color = final_color * dot;

        return final_color;
    }

    color3 rtSample( float x, float y ) const {
        // create a ray casting into the scene from this "pixel"
        ray sample_ray({ 0, 0, -800 }, { (x / float(WIDTH)) * 100, (y / float(HEIGHT)) * 100, 200 });
        // sample this ray - if the ray doesn't hit anything, use the color of the fog
        return SampleRay( sample_ray.normalize(), BOUNCES ).value_or( FOG );
    }

    // apply a linear interpolation between two colors
    color3 lerp( color3 from, color3 to, float by ) const {
        if (by <= 0.0f) return from;
        if (by >= 1.0f) return to;
        return color3(
            from.x * (1.0f - by) + to.x * by,
            from.y * (1.0f - by) + to.y * by,
            from.z * (1.0f - by) + to.z * by
        );
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

