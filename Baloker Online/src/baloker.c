#include "baloker.h"

int collideRect(float px, float py, float x, float y, float w, float h)
{
    if (x < px && px < x + w) {
        if (y < py && py < y + h) {
            return 1;
        }
    }

    return 0;
}
