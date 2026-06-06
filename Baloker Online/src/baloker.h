#pragma once

#define width 1920
#define height 1080

#define Target_FPS 60
#define Target_MS 1000 / Target_FPS

#define cardTextureCacheSize 100
#define maxPlayers 8

int collideRect(float px, float py, float x, float y, float w, float h);
