#pragma once

#define true 1
#define false 0

#define TITLE "Baloker Online"
#define VERSION "v0.0.2"

#define WIDTH 1920
#define HEIGHT 1080

#define Target_FPS 60
#define Target_MS 1000 / Target_FPS

#define cardTextureCacheSize 100
#define maxPlayers 8

#define ADDRESS "kingtasaz.duckdns.org"

#ifdef _DEBUG
#define PORT 8081
#define PORT2 8082
#else
#define PORT 8082
#define PORT2 8081
#endif

int collideRect(float px, float py, float x, float y, float w, float h);
