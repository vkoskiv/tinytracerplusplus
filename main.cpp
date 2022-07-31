#include <iostream>
#include <vector>
#include <assert.h>

#include "real.h"
#include "vec3.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static inline float tosrgb(float c) {
	c = std::max(0.0f, std::min(1.0f, c));
	if (c <= 0.0031308) {
		return 12.92 * c;
	} else {
		return (1.055 * pow(c, 0.4166666667)) - 0.055;
	}
}

vec3f to_srgb(vec3f color) {
	return vec3f(tosrgb(color.x), tosrgb(color.y), tosrgb(color.z));
}

constexpr float waveln_min_nm = 375.0f;
constexpr float waveln_max_nm = 725.0f;

// sRGB D65
// Values from http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
constexpr vec3f xyz_to_srgb_x( 3.2404542f, -1.5371385f, -0.4985314f);
constexpr vec3f xyz_to_srgb_y(-0.9692660f,	1.8760108f,  0.0415560f);
constexpr vec3f xyz_to_srgb_z( 0.0556434f, -0.2040259f,  1.0572252f);

// Inputs: Wavelength in nanometers
float xFit_1931( float wave )
{
	float t1 = (wave-442.0f)*((wave<442.0f)?0.0624f:0.0374f);
	float t2 = (wave-599.8f)*((wave<599.8f)?0.0264f:0.0323f);
	float t3 = (wave-501.1f)*((wave<501.1f)?0.0490f:0.0382f);
	return 0.362f*expf(-0.5f*t1*t1) + 1.056f*expf(-0.5f*t2*t2) - 0.065f*expf(-0.5f*t3*t3);
}
float yFit_1931( float wave )
{
	float t1 = (wave-568.8f)*((wave<568.8f)?0.0213f:0.0247f);
	float t2 = (wave-530.9f)*((wave<530.9f)?0.0613f:0.0322f);
	return 0.821f*exp(-0.5f*t1*t1) + 0.286f*expf(-0.5f*t2*t2);
}
float zFit_1931( float wave )
{
	float t1 = (wave-437.0f)*((wave<437.0f)?0.0845f:0.0278f);
	float t2 = (wave-459.0f)*((wave<459.0f)?0.0385f:0.0725f);
	return 1.217f*exp(-0.5f*t1*t1) + 0.681f*expf(-0.5f*t2*t2);
}

int main() {
	std::cout << "Hello, tinytracer++!" << std::endl;

	std::cout << "done" << std::endl;
	return 0;
}
