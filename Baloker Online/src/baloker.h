#pragma once

#define true 1
#define false 0

#define TITLE "Baloker Online"
#define VERSION "v0.0.2"

#define width 1920
#define height 1080

#define Target_FPS 60
#define Target_MS 1000 / Target_FPS

#define cardTextureCacheSize 100
#define maxPlayers 8

int collideRect(float px, float py, float x, float y, float w, float h);
