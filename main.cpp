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
constexpr vec3f xyz_to_srgb_y(-0.9692660f,  1.8760108f,  0.0415560f);
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

struct histogram {
    const size_t x_res;
    const size_t y_res;
    size_t m_captured_samples;
    std::vector<vec3f> data;

    histogram(size_t x_res_, size_t y_res_) : x_res(x_res_), y_res(y_res_) {
        data.resize(x_res * y_res);
        reset();
    }

    void reset() {
        m_captured_samples = 0;
        for (size_t i = 0; i < x_res * y_res; ++i) data[i] = vec3(0.0f);
    }

};

// Hash function by Thomas Wang: https://burtleburtle.net/bob/hash/integer.html
static inline uint32_t hash(uint32_t x) {
    x  = (x ^ 12345391) * 2654435769;
    x ^= (x << 6) ^ (x >> 26);
    x *= 2654435769;
    x += (x << 5) ^ (x >> 12);
    return x;
}

static inline float uintToUnitReal(uint32_t v) {
    // Trick from MTGP: generate an uniformly distributed single precision number in [1,2) and subtract 1
    union {
        uint32_t u;
        float f;
    } x;
    x.u = (v >> 9) | 0x3f800000u;
    return x.f - 1.0f;
}

static inline float wrapAdd(float u, float v) {
    return (u + v < 1.0f) ? u + v : u + v - 1.0f;
}

// By PBRT authors
static inline float radicalInverse(int pass, int base) {
    const float invBase = 1.0f / base;
    int reversedDigits = 0;
    float invBaseN = 1.0f;
    while (pass) {
        const int next = pass / base;
        const int digit = pass - base * next;
        reversedDigits = reversedDigits * base + digit;
        invBaseN *= invBase;
        pass = next;
    }
    return std::min(reversedDigits * invBaseN, 0.99999994f);
}

static const unsigned int primes[] = {2, 3, 5, 7, 11, 13};
static const unsigned int primes_count = 6;


inline float sign(float v) { return (v >= 0.0f) ? 1.0f : -1.0f; }

// Convert uniform distribution into triangle-shaped distribution
// From https://www.shadertoy.com/view/4t2SDh
inline float triDist(float v)
{
    const float orig = v * 2 - 1;
    v = orig / std::sqrt(std::fabs(orig));
    v = std::max(-1.0f, v); // Nerf the NaN generated by 0*rsqrt(0). Thanks @FioraAeterna!
    v = v - sign(orig);

    return v;
}

struct quad {
    vec3f points[4];
};

struct ray {
    vec3f start;
    vec3f direction;
};

bool ray_quad_hit(struct ray ray, struct quad quad) {

    return false;
}
float f(float x, float y, float waveln_nm, int width, int height) {
    float u_want = x / width;
    float w_want = waveln_min_nm + (waveln_max_nm - waveln_min_nm) * u_want;
    float w_dist = std::abs(w_want - waveln_nm);
    float w_response = (w_dist < 10) ? 1 : 0;
    //return w_response;
    return 1.0f;
    //return (length(vec3f(x, y, 0) - vec3f(width, height, 0) * 0.5f) < height * 0.5f) ? w_response : 0;
}

int main() {
    std::cout << "Hello, tinytracer++!" << std::endl;

    std::vector<unsigned char> g_outbuf;
    static constexpr size_t width = 1280;
    static constexpr size_t height = 800;
    histogram histo(width, height);
    g_outbuf.resize(width * height * 4);

    int current_sample_idx = 0;

    for (int pass = 0; pass < 10000; ++pass) {
        int current_dim = 0;
        float x_sample = radicalInverse(current_sample_idx, primes[current_dim++]);
        float y_sample = radicalInverse(current_sample_idx, primes[current_dim++]);
        float w_sample = radicalInverse(current_sample_idx, primes[current_dim++]);

#pragma omp parallel for
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const size_t pix_idx = y * width + x;
                const float x_hash = uintToUnitReal(hash(pix_idx * 3 + 0));
                const float y_hash = uintToUnitReal(hash(pix_idx * 3 + 1));
                const float w_hash = uintToUnitReal(hash(pix_idx * 3 + 2));
                const float x_pixel = x + triDist(wrapAdd(x_sample, x_hash)) + 0.5f;
                const float y_pixel = y + triDist(wrapAdd(y_sample, y_hash)) + 0.5f;
                const float w_pixel =     wrapAdd(w_sample, w_hash);
                const float waveln_nm = waveln_min_nm + (waveln_max_nm - waveln_min_nm) * w_pixel;
                assert(waveln_nm >= waveln_min_nm);
                assert(waveln_nm <= waveln_max_nm);

                // Fit wavelength into CIE 1931 XYZ
                const vec3f xyz_waveln = vec3f(xFit_1931(waveln_nm), yFit_1931(waveln_nm), zFit_1931(waveln_nm));
                const vec3f sample_xyz = xyz_waveln * f(x_pixel, y_pixel, waveln_nm, width, height);
                const vec3f pixel_color_xyz = histo.data[pix_idx] + sample_xyz;
                histo.data[pix_idx] = pixel_color_xyz;

                // Get linear RGB by multiplying xyz vector with the xyz_to_srgb matrix
                vec3f pixel_color_rgb = vec3f(dot(pixel_color_xyz, xyz_to_srgb_x), dot(pixel_color_xyz, xyz_to_srgb_y), dot(pixel_color_xyz, xyz_to_srgb_z)) / (current_sample_idx + 1);

                pixel_color_rgb.x = std::max(0.0f, std::min(1.0f, pixel_color_rgb.x));
                pixel_color_rgb.y = std::max(0.0f, std::min(1.0f, pixel_color_rgb.y));
                pixel_color_rgb.z = std::max(0.0f, std::min(1.0f, pixel_color_rgb.z));

                g_outbuf[pix_idx * 4 + 0] = (unsigned char)std::min(int(tosrgb(pixel_color_rgb.x) * 255.0f), 255);
                g_outbuf[pix_idx * 4 + 1] = (unsigned char)std::min(int(tosrgb(pixel_color_rgb.y) * 255.0f), 255);
                g_outbuf[pix_idx * 4 + 2] = (unsigned char)std::min(int(tosrgb(pixel_color_rgb.z) * 255.0f), 255);
                g_outbuf[pix_idx * 4 + 3] = (unsigned char)255;
            }
        }

        stbi_write_png("/tmp/out.png", width, height, 4, &g_outbuf[0], width * 4);
        printf("current sample: %i\n", current_sample_idx);
        ++current_sample_idx;
    }

    std::cout << "done" << std::endl;

    return 0;
}
