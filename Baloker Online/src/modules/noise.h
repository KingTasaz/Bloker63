#include <stdint.h>

#define NOISE_GRID 16

float lerp(float a, float b, float t);
float smoothstep(float t);
void FillTexture(uint8_t *tex, unsigned int seed);