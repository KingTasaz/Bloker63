#include "noise.h"
#include <stdint.h>
#include <stdlib.h>
#include "cards.h"

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

float smoothstep(float t)
{
    return t * t * (3.0f - 2.0f * t);
}

void FillTexture(uint8_t *tex, unsigned int seed)
{
    srand(seed);

    float grid[NOISE_GRID + 1][NOISE_GRID + 1];

    for (int y = 0; y <= NOISE_GRID; y++)
    {
        for (int x = 0; x <= NOISE_GRID; x++)
        {
            grid[y][x] = (float)rand() / RAND_MAX;
        }
    }

    for (int py = 0; py < cardHeight; py++)
    {
        for (int px = 0; px < cardWidth; px++)
        {
            float fx = (float)px / cardWidth * NOISE_GRID / 1.4;
            float fy = (float)py / cardHeight * NOISE_GRID;

            int x0 = (int)fx;
            int y0 = (int)fy;

            float tx = smoothstep(fx - x0);
            float ty = smoothstep(fy - y0);

            float a = lerp(grid[y0][x0],     grid[y0][x0+1], tx);
            float b = lerp(grid[y0+1][x0],   grid[y0+1][x0+1], tx);

            float value = lerp(a, b, ty);

            tex[py * cardWidth + px] = (uint8_t)(value * 255);
        }
    }
}