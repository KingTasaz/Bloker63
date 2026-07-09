#include <stdio.h>

#include "SDL3/SDL.h"
#include "SDL3/SDL_timer.h"
#include "SDL3_ttf/SDL_ttf.h"

#include "../baloker.h"
#include "window.h"
#include "UI.h"
#include "cards.h"
#include "game.h"
#include "poker.h"
#include "network.h"

#define rect9Size 16, 16, 15, 15, 1

SDL_Time Elapsed;

float FPS = 0;
float Delta = 0;

int typingUsername = 0;
int typingPassword = 0;
char UsernameBox[USERNAMEMAXLENGTH];
char PasswordBox[PASSWORDMAXLENGTH];
char JoinCode[4];
char LoginStatus[50];
const SDL_Color *LoginStatusColor;

int loginDebounce = 0;
int createLobbyDebounce = 0;
int loadLobbyDebounce = 0;
int joinLobbyDebounce = 0;
int startGameDebounce = 0;

WindowState Menu = MainMenu;
int overlay_AccountLogin = 0;
int overlay_JokerEnhance = 0;

// Variables
SDL_Surface *surface = NULL;
SDL_Texture *texture_background = NULL;
SDL_Texture *texture_title = NULL;
SDL_Texture *texture_rect = NULL;
const char *img_background = "assets/Background.png";
const char *img_title = "assets/Title.png";

SDL_Texture *Text_Title = NULL;
SDL_Texture *Text_Version = NULL;

SDL_Texture *button_check = NULL;
SDL_Texture *button_call = NULL;
SDL_Texture *button_call_broke = NULL;
SDL_Texture *button_fold = NULL;
SDL_Texture *button_raise = NULL;

SDL_Texture *button_account = NULL;
SDL_Texture *button_Host = NULL;
SDL_Texture *button_Join = NULL;
SDL_Texture *button_Login = NULL;
SDL_Texture *button_Logout = NULL;
SDL_Texture *button_Regis = NULL;
SDL_Texture *menu_chips = NULL;

SDL_Texture *arrowLeft = NULL;
SDL_Texture *arrowRight = NULL;

SDL_FRect rectOverlay_Account = {WIDTH / 2 - 200, HEIGHT / 2 - 130, 400, 400};

const SDL_Color BLACK = { 0, 0, 0, SDL_ALPHA_OPAQUE };
const SDL_Color WHITE = { 255, 255, 255, SDL_ALPHA_OPAQUE };
const SDL_Color GRAY = { 67, 67, 67, SDL_ALPHA_OPAQUE };
const SDL_Color RED = { 255, 60, 60, SDL_ALPHA_OPAQUE};

packet_fbCreateLobby_t createLobbyPacket = {0};

char fBuf[50];

int Window_Init(Window* w)
{
    createLobbyPacket.MaxUsers = 1;

    // Initialize Window
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        printf("SDL Failed to Initialize\n");
        return 1;
    }

    if (!TTF_Init()) {
        printf("Font Failed to Initialize");
        return 1;
    }

    w->window = SDL_CreateWindow(
        "Baloker Online :D",
        WIDTH, HEIGHT,
        0
    );

    w->renderer = SDL_CreateRenderer(w->window, NULL);
    w->running = 1;

    // Load Images
    surface = SDL_LoadPNG(img_background);  // TODO: check for surface failures
    texture_background = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    surface = SDL_LoadPNG(img_title);
    texture_title = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    surface = SDL_LoadPNG("assets/9Rect_l16_r16_t15_b15.png");
    texture_rect = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    // Initialize UI
    initUI(w->renderer);

    // Load static text
    surface = TTF_RenderText_Blended(BalFontSmall, TITLE, 0, BLACK);
    Text_Title = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    surface = TTF_RenderText_Blended(BalFontSmall, VERSION, 0, BLACK);
    Text_Version = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    // Load buttons
    surface = SDL_LoadPNG("assets/button_check.png");
    button_check = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_call.png");
    button_call = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_raise.png");
    button_raise = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_fold.png");
    button_fold = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_call_fail.png");
    button_call_broke = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    surface = SDL_LoadPNG("assets/button_host.png");
    button_Host = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_join.png");
    button_Join = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/accountButton.png");
    button_account = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_Login.png");
    button_Login = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_Register.png");
    button_Regis = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/chips.png");
    menu_chips = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/button_logout.png");
    button_Logout = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    surface = SDL_LoadPNG("assets/arrowLeft.png");
    arrowLeft = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);
    surface = SDL_LoadPNG("assets/arrowRight.png");
    arrowRight = SDL_CreateTextureFromSurface(w->renderer, surface);
    SDL_DestroySurface(surface);

    return 0;
}

void Window_HandleEvents(Window* w)
{
    float mx, my;
    SDL_GetMouseState(&mx, &my);

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        // MARK: Keypress
        switch(event.type)
        {
            case SDL_EVENT_QUIT:
                w->running = 0;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (typingUsername) {
                    if (event.key.key == SDLK_RETURN) {
                        typingUsername = 0;
                        break;
                    }

                    int len = strnlen(UsernameBox, USERNAMEMAXLENGTH);

                    if (event.key.key == SDLK_BACKSPACE && len > 0) {
                        UsernameBox[len-1] = '\0';
                        break;
                    }

                    if (len + 1 >= USERNAMEMAXLENGTH)
                        break;

                    SDL_Keycode key = SDL_GetKeyFromScancode(event.key.scancode, event.key.mod, false);

                    if (((int)key < -1) || ((int)key > 255))
                        break;

                    if (isalnum(key))
                        UsernameBox[len] = key;

                } else if (typingPassword) {
                    if (event.key.key == SDLK_RETURN) {
                        typingPassword = 0;
                        break;
                    }

                    int len = strnlen(PasswordBox, PASSWORDMAXLENGTH);

                    if (event.key.key == SDLK_BACKSPACE && len > 0) {
                        PasswordBox[len-1] = '\0';
                        break;
                    }

                    if (len + 1 >= PASSWORDMAXLENGTH)
                        break;

                    SDL_Keycode key = SDL_GetKeyFromScancode(event.key.scancode, event.key.mod, false);
                    if (((int)key < -1) || ((int)key > 255))
                        break;

                    if (isalnum(key))
                        PasswordBox[len] = key;
                } else if (Menu == JoinLobby) {
                    int len = strnlen(JoinCode, 4);

                    if (event.key.key == SDLK_RETURN && len == 4) {
                        joinLobbyDebounce = 1;
                        packet_fbJoinLobby_t p;
                        strncpy(p.code, JoinCode, 4);
                        SendPacket(fbJoinLobby, &p, sizeof(p));
                        return;
                    }

                    if (event.key.key == SDLK_BACKSPACE && len > 0) {
                        JoinCode[len-1] = '\0';
                        break;
                    }

                    if (len + 1 >= 5)
                        break;

                    SDL_Keycode key = SDL_GetKeyFromScancode(event.key.scancode, event.key.mod, false);

                    if (((int)key < -1) || ((int)key > 255))
                        break;

                    if (isalpha(key))
                        JoinCode[len] = toupper(key);
                } else {
                    switch (event.key.key) {
                        case SDLK_G:
                            if (Menu != HostSetup || createLobbyDebounce)
                                break;

                            SendPacket(fbCreateLobby, &createLobbyPacket, sizeof(createLobbyPacket));
                            createLobbyDebounce = 1;
                            break;
                        case SDLK_N:
                            if (Menu != LobbyWaiting || startGameDebounce)
                                break;

                            SendPacket(fbStartLobby, &(packet_fbLeaveLobby_t){0}, sizeof(packet_fbLeaveLobby_t));
                            startGameDebounce = 1;
                            break;
                    }
                } break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // MARK: Click Event
                    switch (Menu) {
                        case MainMenu:
                            typingUsername = 0;
                            typingPassword = 0;

                            // MARK: . Login
                            if (overlay_AccountLogin) {
                                if (!collideRect(mx, my,
                                                rectOverlay_Account.x, rectOverlay_Account.y,
                                                rectOverlay_Account.w, rectOverlay_Account.h)
                                ) {
                                    overlay_AccountLogin = 0;
                                    break;
                                }

                                if (collideRect(mx, my, 
                                rectOverlay_Account.x + 10, rectOverlay_Account.y + 50,
                                rectOverlay_Account.w - 20, 50)) {
                                    typingUsername = 1;
                                    break;
                                }

                                if (collideRect(mx, my, 
                                rectOverlay_Account.x + 10, rectOverlay_Account.y + 160,
                                rectOverlay_Account.w - 20, 50)) {
                                    typingPassword = 1;
                                    break;
                                }

                                if (loginDebounce)
                                    break;

                                if (strnlen(UsernameBox, 2) == 0 || strnlen(PasswordBox, 2) == 0)
                                    break;

                                if (collideRect(mx, my,
                                rectOverlay_Account.x + 100, rectOverlay_Account.y + 270,
                                rectOverlay_Account.w - 200, 50)) {
                                    // login
                                    AttemptLogin(UsernameBox, PasswordBox);
                                    strcpy(LoginStatus, "Logging In...");
                                    loginDebounce = 1;
                                    break;
                                }

                                if (collideRect(mx, my,
                                rectOverlay_Account.x + 100, rectOverlay_Account.y + 330,
                                rectOverlay_Account.w - 200, 50)) {
                                    // register
                                    AttemptRegister(UsernameBox, PasswordBox);
                                    strcpy(LoginStatus, "Making Account...");
                                    loginDebounce = 1;
                                    break;
                                }

                            }

                            if (collideRect(
                                mx, my,
                                WIDTH - button_account->w - 15, HEIGHT - button_account->h - 15,
                                button_account->h, button_account->w
                            )) {
                                overlay_AccountLogin = 1;
                                memset(UsernameBox, 0, sizeof(UsernameBox));
                                memset(PasswordBox, 0, sizeof(PasswordBox));
                                LoginStatusColor = &BLACK;
                                break;
                            }

                            if (!_local_LoggedIn)
                                break;

                            // MARK: . host and join
                            if (collideRect(
                                mx, my,
                                WIDTH / 2 - button_Host->w / 4, HEIGHT / 2 - button_Host->h / 4 + 100,
                                button_Host->w * 0.5 , button_Host->h * 0.5
                            )) {
                                Menu = HostSetup;
                                break;
                            }

                            if (collideRect(
                                mx, my,
                                WIDTH / 2 - button_Host->w / 4, HEIGHT / 2 - button_Host->h / 4 + 200,
                                button_Host->w * 0.5 , button_Host->h * 0.5
                            ) && !loadLobbyDebounce) {
                                Menu = JoinLobby;

                                packet_fbGetLobbiesRequest_t p;
                                SendPacket(fbGetLobbiesReq, &p, sizeof(p));
                                loadLobbyDebounce = 1;

                                break;
                            }

                            break;

                        case HostSetup:
                            if (collideRect(mx, my, WIDTH / 2 + 30, 295, 40, 40)) // left arrow
                                if (createLobbyPacket.MaxUsers > 1)
                                    createLobbyPacket.MaxUsers--;
                            if (collideRect(mx, my, WIDTH / 2 + 130, 295.0, 40.0, 40.0)) // right arrow
                                if (createLobbyPacket.MaxUsers < maxPlayers)
                                    createLobbyPacket.MaxUsers++;
                            if (collideRect(mx, my, WIDTH / 2 + 30, 400, 130, 40)) // visiblity
                                createLobbyPacket.Private = 1 - createLobbyPacket.Private;

                            break;

                        case JoinLobby:
                            break;

                        case LobbyWaiting:
                            break;

                        case InGame:
                            break;
                    }
                }
                break;
        }
    }
}

void Window_StartFrame()
{
    Elapsed = SDL_GetTicks();
}

void Window_EndFrame()
{
    Elapsed = SDL_GetTicks() - Elapsed;

    SDL_Time remaining = Elapsed - Target_MS;

    if (remaining > 0) {
        SDL_Delay(remaining);
    }

    Delta = (float)Elapsed;
    FPS = 1.0 / (float)Elapsed;

    if (Delta > 100) {
        Delta = 100;
    }
}

void Window_Debug()
{
    printf("FPS: %f\n", FPS);
}

void Window_Update(Window* w)
{
    float mx, my;
    SDL_GetMouseState(&mx, &my);

    if (Menu != InGame)
        return;

    // Update Main Deck Cards
    for (int i = 0; i < mainDeck->cardCount; i++) {
        Card *card = &mainDeck->Cards[i];

        card->target_scale = card->default_scale;

        card->x += (card->tx - card->x) * Delta / 100;
        card->y += (card->ty - card->y) * Delta / 100;
        card->scale += (card->target_scale - card->scale) * Delta / 100;
    }

    // Update Player Hands Hand
    for (int p = 0; p < gameState->playerCount; p++)
    {
        PlayerHand *Hand = GetPlayer(p)->Hand;

        for (int i = 0; i < Hand->handCount; i++) {
            Card *card = &Hand->Hand[i];

            if (mouseCollideCard(mx, my, Hand->Hand[i])) {
                card->target_scale = card->default_scale * cardHighlightScale;
            } else {
                card->target_scale = card->default_scale;
            }

            card->x += (card->tx - card->x) * Delta / 100;
            card->y += (card->ty - card->y) * Delta / 100;
            card->scale += (card->target_scale - card->scale) * Delta / 100;
            if (card->burning) { card->burnProgress += Delta / 500; }
        }
    }

    // Update my river
    PlayerHand *myHand = GetLocalPlayer()->Hand;
    for (int i = 0; i < myHand->riverCount; i++) {
        Card *card = &myHand->River[i];

        if (mouseCollideCard(mx, my, myHand->River[i])) {
            card->target_scale = card->default_scale * cardHighlightScale;
        } else {
            card->target_scale = card->default_scale;
        }

        card->x += (card->tx - card->x) * Delta / 100;
        card->y += (card->ty - card->y) * Delta / 100;
        card->scale += (card->target_scale - card->scale) * Delta / 100;
        if (card->burning) { card->burnProgress += Delta / 1000; }
    }

    // Update Chips
    bigBlindChip.x += (bigBlindChip.tx - bigBlindChip.x) * Delta / 100;
    bigBlindChip.y += (bigBlindChip.ty - bigBlindChip.y) * Delta / 100;

    smallBlindChip.x += (smallBlindChip.tx - smallBlindChip.x) * Delta / 100;
    smallBlindChip.y += (smallBlindChip.ty - smallBlindChip.y) * Delta / 100;

    turnOrderChip.x += (turnOrderChip.tx - turnOrderChip.x) * Delta / 100;
    turnOrderChip.y += (turnOrderChip.ty - turnOrderChip.y) * Delta / 100;
}

// MARK: Main Menu
void _renderMainMenu(Window *w)
{
    SDL_FRect dst_rect;

    dst_rect.x = 0.0f;
    dst_rect.y = 0.0f;
    dst_rect.w = WIDTH;
    dst_rect.h = HEIGHT;
    SDL_RenderTexture(w->renderer, texture_title, NULL, &dst_rect);

    if (!serverOpen) {      // connecting to server
        if (serverConnectionError == 1)
            drawText(w->renderer, BalFontSmall,
                    "Failed to reach Server", RED,
                    WIDTH / 2, HEIGHT * 2 / 3, 1);
        else if (serverConnectionError == 2)
            drawText(w->renderer, BalFontSmall,
                    "Outdated client", RED,
                    WIDTH / 2, HEIGHT * 2 / 3, 1);
        else
            drawText(w->renderer, BalFontSmall,
                    "Connecting to Server...", BLACK,
                    WIDTH / 2, HEIGHT * 2 / 3, 1);

        return;
    }
    
    if (_local_accountDataFound && !_local_LoggedIn) {      // connected, logging in
        drawText(w->renderer, BalFontSmall,
                "Logging in...", BLACK,
                WIDTH / 2, HEIGHT * 2 / 3, 1);

        return;
    }

    // Main Menu
    dst_rect.w = button_account->w;
    dst_rect.h = button_account->h;
    dst_rect.x = WIDTH - dst_rect.w - 15;
    dst_rect.y = HEIGHT - dst_rect.h - 15;
    SDL_RenderTexture(w->renderer, button_account, NULL, &dst_rect);

    dst_rect.w = button_Host->w * 0.5;
    dst_rect.h = button_Host->h * 0.5;
    dst_rect.x = WIDTH / 2 - dst_rect.w / 2;
    dst_rect.y = HEIGHT / 2 - dst_rect.h / 2 + 100;
    SDL_RenderTexture(w->renderer, button_Host, NULL, &dst_rect);
    dst_rect.y = HEIGHT / 2 - dst_rect.h / 2 + 200;
    SDL_RenderTexture(w->renderer, button_Join, NULL, &dst_rect);

    if (!_local_LoggedIn)
        drawText(w->renderer, BalFontSmall,
                "You must be logged in to play", BLACK,
                WIDTH / 2, dst_rect.y + 150, 1);
    else {
        snprintf(fBuf, sizeof(fBuf), "Logged in as: %s", _local_Username);
        drawText(w->renderer, BalFontSmall,
                fBuf, BLACK,
                WIDTH / 2, dst_rect.y + 150, 1);

        dst_rect.w = button_Logout->w * 0.25;
        dst_rect.h = button_Logout->h * 0.25;
        dst_rect.x = WIDTH / 2 - dst_rect.w / 2;
        dst_rect.y = HEIGHT / 2 + 335;
        SDL_RenderTexture(w->renderer, button_Logout, NULL, &dst_rect);

        dst_rect.w = menu_chips->w;
        dst_rect.h = menu_chips->h;
        dst_rect.x = 10;
        dst_rect.y = HEIGHT - dst_rect.h - 10;
        SDL_RenderTexture(w->renderer, menu_chips, NULL, &dst_rect);

        if (_local_Chips == -676767) {
            drawText(w->renderer, BalFontSmall, "[Loading]", BLACK, dst_rect.x + dst_rect.w + 10, HEIGHT - 10, 0);
        } else {
            snprintf(fBuf, sizeof(fBuf), "Chips: %d", _local_Chips);
            drawText(w->renderer, BalFontSmall, fBuf, BLACK, dst_rect.x + dst_rect.w + 10, HEIGHT - 50, 0);
        }
    }

    if (overlay_AccountLogin) {
        SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &rectOverlay_Account);

        drawText(w->renderer, BalFontSmall, "Username:", BLACK,
                    rectOverlay_Account.x + rectOverlay_Account.w / 2, rectOverlay_Account.y + 25, 1);
        dst_rect.w = rectOverlay_Account.w - 20;
        dst_rect.h = 50;
        dst_rect.x = rectOverlay_Account.x + 10;
        dst_rect.y = rectOverlay_Account.y + 50;
        SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);
        drawText(w->renderer, BalFontSmall, UsernameBox, BLACK,
                    rectOverlay_Account.x + 20, rectOverlay_Account.y + 60, 0);

        drawText(w->renderer, BalFontSmall, "Password:", BLACK,
                    rectOverlay_Account.x + rectOverlay_Account.w / 2, rectOverlay_Account.y + 135, 1);
        dst_rect.y = rectOverlay_Account.y + 160;
        SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);
        drawText(w->renderer, BalFontSmall, PasswordBox, BLACK,
                    rectOverlay_Account.x + 20, rectOverlay_Account.y + 170, 0);

        drawText(w->renderer, BalFontSmall, LoginStatus, *LoginStatusColor,
                rectOverlay_Account.x + rectOverlay_Account.w / 2, rectOverlay_Account.y + 240, 1); // Login Status

        dst_rect.w = rectOverlay_Account.w - 200;
        dst_rect.h = 50;
        dst_rect.x = rectOverlay_Account.x + 100;
        dst_rect.y = rectOverlay_Account.y + 270;
        SDL_RenderTexture(w->renderer, button_Login, NULL, &dst_rect);
        dst_rect.y = rectOverlay_Account.y + 330;
        SDL_RenderTexture(w->renderer, button_Regis, NULL, &dst_rect);
    }

}

// MARK: Settings
void _renderSettings(Window *w);

// MARK: Create Lobby
void _renderCreateLobby(Window *w)
{
    SDL_FRect r;

    drawText(w->renderer, BalFontLarge, "Create Lobby", BLACK, WIDTH / 2, 200, 1);

    r.x = WIDTH / 2 - 200;
    r.y = 250;
    r.w = 400;
    r.h = 300;
    SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &r);

    // max players
    r.x += 20;
    r.y += 50;
    r.w = 40;
    r.h = 40;
    drawText(w->renderer, BalFontSmall, "Max Players:", BLACK, r.x, r.y, 0);
    snprintf(fBuf, sizeof(fBuf), "%d / %d", createLobbyPacket.MaxUsers, maxPlayers);
    drawText(w->renderer, BalFontSmall, fBuf, BLACK, r.x + 250, r.y, 0);
    r.x += 210;
    r.y -= 5;
    SDL_RenderTexture(w->renderer, arrowLeft, NULL, &r);
    r.x += 100;
    SDL_RenderTexture(w->renderer, arrowRight, NULL, &r);

    r.x = WIDTH / 2 - 200 + 20;
    r.y = 250 + 150;
    drawText(w->renderer, BalFontSmall, "Visibility:", BLACK, r.x, r.y, 0);

    r.x += 210;
    r.w = 130;
    if (createLobbyPacket.Private) {
        SDL_RenderTexture(w->renderer, button_check, NULL, &r);
        drawText(w->renderer, BalFontSmall, "Private", BLACK, r.x + 30, r.y + 5, 0);
    } else {
        SDL_RenderTexture(w->renderer, button_call, NULL, &r);
        drawText(w->renderer, BalFontSmall, "Public", BLACK, r.x + 30, r.y + 5, 0);
    }

    drawText(w->renderer, BalFontSmall, 
            "I am too lazy to code in another button - press G to create the lobby :shrug:", BLACK, 200, 600, 0);
}

// MARK: Join Game
void _renderJoinGame(Window *w)
{
    if (joinLobbyDebounce) {
        drawText(w->renderer, BalFontSmall, "Loading...", BLACK, WIDTH / 2, HEIGHT / 2, 1);
        return;
    }

    SDL_FRect r;
    r.x = 100;
    r.y = 100;
    r.w = 1000;
    r.h = 50;

    // Render shown lobbies
    for (int l = 0; l < numLobbyHeaders; l++) {
        BalokerLobbyHeader *lobby = &lobbyHeaders[l];

        SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &r);

        snprintf(fBuf, sizeof(fBuf), "%s's Lobby", lobby->ownerName);
        drawText(w->renderer, BalFontSmall, fBuf, BLACK, r.x + 10, r.y + 10, 0);

        if (lobby->Private) {
            drawText(w->renderer, BalFontSmall, "(Private)", BLACK, r.x + 410, r.y + 10, 0);
        } else {
            snprintf(fBuf, 5, "%s", lobby->Code);
            fBuf[5] = '\0';
            drawText(w->renderer, BalFontSmall, fBuf, BLACK, r.x + 410, r.y + 10, 0);
        }

        snprintf(fBuf, sizeof(fBuf), "%d/%d", lobby->Users, lobby->MaxUsers);
        drawText(w->renderer, BalFontSmall, fBuf, BLACK, r.x + 910, r.y + 10, 0);

        r.y += 80;
    }

    // Join Code Entry
    r.x = 1200;
    r.y = 100;
    r.w = 500;
    r.h = 150;
    SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &r);

    snprintf(fBuf, 5, "%s", JoinCode);
    drawText(w->renderer, BalFontSmall, "Enter Code:", BLACK, r.x + 10, r.y + 10, 0);
    drawText(w->renderer, BalFontLarge, fBuf, BLACK, r.x + 200, r.y + 10, 0);

    drawText(w->renderer, BalFontSmall, "Press ENTER to join!", BLACK, r.x + 10, r.y + 100, 0);
}

// MARK: Lobby
void _renderGameLobby(Window *w)
{
    SDL_FRect r;

    snprintf(fBuf, sizeof(fBuf), "%s's Lobby:", currentLobby.ownerName);
    drawText(w->renderer, BalFontLarge, fBuf, BLACK, 50, 50, 0);

    for (int i = 0; i < gameState->playerCount; i++) {
        int x = 100;
        int y = 200 + i * 70;

        r.x = x;
        r.y = y;
        r.w = 1000;
        r.h = 50;
        SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &r);

        x += 10;
        y += 10;
        drawText(w->renderer, BalFontSmall, GetPlayerName(i), BLACK, x, y, 0);

        if (i == LocalPlayer)
            drawText(w->renderer, BalFontSmall, "(You)", BLACK, x + 300, y, 0);

        if (strncmp(currentLobby.ownerName, GetPlayerName(i), USERNAMEMAXLENGTH) == 0)
            drawText(w->renderer, BalFontSmall, "(Owner)", BLACK, x + 500, y, 0);

        r.x = 900;
        r.y -= 5;
        r.w = 50;
        r.h = 50;
        SDL_RenderTexture(w->renderer, menu_chips, NULL, &r);
        snprintf(fBuf, sizeof(fBuf), "%d", GetPlayer(i)->Chips);
        drawText(w->renderer, BalFontSmall, fBuf, BLACK, r.x + r.w + 5, y, 0);
    }

    r.x = 1200;
    r.y = 100;
    r.w = 500;
    r.h = 200;
    SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &r);

    snprintf(fBuf, 5, "%s", currentLobby.Code);
    drawText(w->renderer, BalFontSmall, "Join Code:", BLACK, r.x + 10, r.y + 10, 0);
    drawText(w->renderer, BalFontLarge, fBuf, BLACK, r.x + 200, r.y + 10, 0);

    snprintf(fBuf, sizeof(fBuf), "Players:           %d/%d", currentLobby.Users, currentLobby.MaxUsers);
    drawText(w->renderer, BalFontSmall, fBuf, BLACK, r.x + 10, r.y + 100, 0);

    if (lobbyOwnerIsMe)
        drawText(w->renderer, BalFontSmall, "Press N to start!", BLACK, r.x + 10, r.y + 130, 0);
}

// MARK: Ingame
void _renderInGame(Window *w)
{
    SDL_FRect dst_rect;

    // Player Info
    dst_rect.x = 220;
    dst_rect.y = 870;
    dst_rect.w = 220;
    dst_rect.h = 300;
    SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);

    drawText(               // name
        w->renderer,
        BalFontSmall,
        GetPlayerName(LocalPlayer),
        BLACK,
        330, 900,
        1
    );

    snprintf(fBuf, sizeof(fBuf), "$%d", GetLocalPlayer()->Chips);
    drawText(               // money
        w->renderer,
        BalFontSmall,
        fBuf,
        BLACK,
        330, 960,
        1
    );

    // Other Player Info
    for (int p = 0; p < gameState->playerCount; p++) {
        if (p == LocalPlayer)
            continue;

        dst_rect.x = PlayerCX[p] - 100;
        dst_rect.y = PlayerCY[p] - 120;
        dst_rect.w = 200;
        dst_rect.h = 100;
        SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);

        drawText(               // name
            w->renderer,
            BalFontSmall,
            GetPlayerName(p),
            BLACK,
            PlayerCX[GetPlrPosIDFromSlot(p)], PlayerCY[GetPlrPosIDFromSlot(p)] - 100,
            1
        );

        snprintf(fBuf, sizeof(fBuf), "$%d", GetPlayer(p)->Chips);
        drawText(               // money
            w->renderer,
            BalFontSmall,
            fBuf,
            BLACK,
            PlayerCX[GetPlrPosIDFromSlot(p)], PlayerCY[GetPlrPosIDFromSlot(p)] - 70,
            1
        );
    }

    // Corner Text
    dst_rect.w = Text_Title->w;
    dst_rect.h = Text_Title->h;
    dst_rect.x = 5;
    dst_rect.y = 5;
    SDL_RenderTexture(w->renderer, Text_Title, NULL, &dst_rect);

    dst_rect.w = Text_Version->w;
    dst_rect.h = Text_Version->h;
    dst_rect.x = 5;
    dst_rect.y = dst_rect.h + 5;
    SDL_RenderTexture(w->renderer, Text_Version, NULL, &dst_rect);

    // Draw cards in deck
    for (int i = 0; i < mainDeck->cardCount; i++) {
        drawCard(w->renderer, mainDeck->Cards[i]);
    }

    // Draw local hand
    PlayerHand *myHand = GetLocalPlayer()->Hand;
    for (int i = 0; i < myHand->handCount; i++) {
        drawCard(w->renderer, myHand->Hand[i]);
    }
    for (int i = 0; i < myHand->riverCount; i++) {
        drawCard(w->renderer, myHand->River[i]);
    }    
    if (GetLocalPlayer()->folded) {
        drawText(
            w->renderer, BalFontSmall,
            "Folded", RED,
            PlayerCX[GetPlrPosIDFromSlot(LocalPlayer)], PlayerCY[GetPlrPosIDFromSlot(LocalPlayer)] - 150,
            1
        );
    }

    // Draw other players
    for (int p = 0; p < gameState->playerCount; p++)
    {
        if (p == LocalPlayer)
            continue;

        Player *plr = GetPlayer(p);

        for (int i = 0; i < plr->Hand->handCount; i++)
        {
            drawCard(w->renderer, plr->Hand->Hand[i]);
        }

        if (plr->folded) {
            drawText(
                w->renderer, BalFontSmall,
                "Folded", RED,
                PlayerCX[p], PlayerCY[p] - 20,
                1
            );
        }
    }

    // Round Information Screen
    dst_rect.x = WIDTH - 350;
    dst_rect.y = HEIGHT - 500;
    dst_rect.w = 350 - 5;
    dst_rect.h = 550;
    SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);

    int cx = WIDTH - 350 + (350 - 5) / 2;
    drawText(
        w->renderer, BalFontSmall,
        "Round Info", BLACK,
        cx, HEIGHT - 450, 1
    );

    snprintf(fBuf, sizeof(fBuf), "Pot: $%d", getTotalPot());
    drawText(
        w->renderer, BalFontSmall,
        fBuf, BLACK,
        WIDTH - 350 + 10, HEIGHT - 400, 0
    );
    snprintf(fBuf, sizeof(fBuf), "Raise: $%d", gameState->Raise);
    drawText(
        w->renderer, BalFontSmall,
        fBuf, BLACK,
        WIDTH - 350 + 10, HEIGHT - 400 + 30, 0
    );
    snprintf(fBuf, sizeof(fBuf), "Cards in Deck: %d", mainDeck->cardCount);
    drawText(
        w->renderer, BalFontSmall,
        fBuf, BLACK,
        WIDTH - 350 + 10, HEIGHT - 400 + 60, 0
    );

    // Hand Label
    dst_rect.x = WIDTH / 2 - 250;
    dst_rect.y = HEIGHT - 380;
    dst_rect.w = 250 * 2;
    dst_rect.h = 75;
    SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);

    int hand = GetLocalPlayer()->Hand->handType;

    if (hand >= 0 && hand < 16) {
        snprintf(fBuf, sizeof(fBuf), "Your Hand: %s", HandNames[hand]);
        drawText(w->renderer, BalFontSmall, fBuf, BLACK,
                    WIDTH / 2, HEIGHT - 380 + 75 / 2, 1);
    } else {
        drawText(w->renderer, BalFontSmall, "Empty Hand", BLACK,
                    WIDTH / 2, HEIGHT - 380 + 75 / 2, 1);
    }

    // Turn Buttons
    if (gameState->turn == LocalPlayer) { // if (is my turn)
        float mx, my;
        SDL_GetMouseState(&mx, &my);

        dst_rect.x = WIDTH / 2 - 400 - 100;
        dst_rect.y = HEIGHT - 380;
        dst_rect.w = 100 * 2;
        dst_rect.h = 90;

        if (gameState->Raise > GetLocalPlayer()->myRaise) {
            if (collideRect(mx, my, dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h)) {
                drawGlow(w->renderer, dst_rect, 30, 20);
            }

            if (gameState->Raise - GetLocalPlayer()->myRaise > GetLocalPlayer()->Chips) {
                SDL_RenderTexture(w->renderer, button_call, NULL, &dst_rect);
                drawText(
                    w->renderer, BalFontSmall, 
                    "All In", BLACK,
                    dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
                );
            } else {
                SDL_RenderTexture(w->renderer, button_call, NULL, &dst_rect);
                drawText(
                    w->renderer, BalFontSmall, 
                    "Call", BLACK,
                    dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
                );
            }
        } else {
            if (collideRect(mx, my, dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h)) {
                drawGlow(w->renderer, dst_rect, 30, 20);
            }

            SDL_RenderTexture(w->renderer, button_check, NULL, &dst_rect);
            drawText(
                w->renderer, BalFontSmall, 
                "Check", BLACK,
                dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
            );            
        }

        dst_rect.x = WIDTH / 2 + 400 - 100;
        dst_rect.y = HEIGHT - 380;
        dst_rect.w = 100 * 2;
        dst_rect.h = 90;

        if (GetLocalPlayer()->Chips - gameState->Raise + GetLocalPlayer()->myRaise > 0) {
            if (collideRect(mx, my, dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h)) {
                drawGlow(w->renderer, dst_rect, 30, 20);
            }

            SDL_RenderTexture(w->renderer, button_raise, NULL, &dst_rect);
            drawText(
                w->renderer, BalFontSmall, 
                "Raise", BLACK,
                dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
            );
        } else {
            SDL_RenderTexture(w->renderer, button_call_broke, NULL, &dst_rect);
            drawText(
                w->renderer, BalFontSmall, 
                "Broke Ahh", BLACK,
                dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
            );
        }

        dst_rect.x = WIDTH / 2 + 400 - 100;
        dst_rect.y = HEIGHT - 270;
        dst_rect.w = 100 * 2;
        dst_rect.h = 90;

        if (collideRect(mx, my, dst_rect.x, dst_rect.y, dst_rect.w, dst_rect.h)) {
            drawGlow(w->renderer, dst_rect, 30, 20);
        }

        SDL_RenderTexture(w->renderer, button_fold, NULL, &dst_rect);
        drawText(
            w->renderer, BalFontSmall, 
            "Fold", BLACK,
            dst_rect.x + dst_rect.w / 2, dst_rect.y + dst_rect.h / 2, 1
        );
    }

    // Chips
    drawChip(w->renderer, bigBlindChip);
    drawChip(w->renderer, smallBlindChip);
    drawChip(w->renderer, turnOrderChip);

    // Game End
    if (gameState->Winner >= 0)
    {
        dst_rect.x = WIDTH / 2 - 500;
        dst_rect.y = HEIGHT / 2 - 50;
        dst_rect.w = 500 * 2;
        dst_rect.h = 50 * 2;
        SDL_RenderTexture9Grid(w->renderer, texture_rect, NULL, rect9Size, &dst_rect);

        if (gameState->Tie) {
            snprintf(fBuf, sizeof(fBuf), "There was a Tie! (%s)", HandNames[gameState->WinningHand]);
            drawText(w->renderer, BalFontSmall, fBuf, BLACK, WIDTH/2, HEIGHT/2, 1);
        } else {
            snprintf(fBuf, sizeof(fBuf), "%s won with a %s",
                    GetPlayerName(gameState->Winner), HandNames[gameState->WinningHand]);
            drawText(w->renderer, BalFontSmall, fBuf, BLACK, WIDTH/2, HEIGHT/2, 1);
        }
    }
}

// MARK: Base Render
void Window_Render(Window* w)
{
    SDL_SetRenderDrawColor(w->renderer, 0, 0, 0, 255);
    SDL_RenderClear(w->renderer);

    // Draw Title Screen
    SDL_FRect dst_rect;

    dst_rect.x = 0.0f;
    dst_rect.y = 0.0f;
    dst_rect.w = WIDTH;
    dst_rect.h = HEIGHT;
    SDL_RenderTexture(w->renderer, texture_background, NULL, &dst_rect);

    switch (Menu) {
        case MainMenu:
            _renderMainMenu(w);
            break;

        case HostSetup:
            _renderCreateLobby(w);
            break;

        case JoinLobby:
            _renderJoinGame(w);
            break;

        case LobbyWaiting:
            createLobbyDebounce = 0;
            _renderGameLobby(w);
            break;

        case InGame:
            _renderInGame(w);
            break;
    }

    SDL_RenderPresent(w->renderer);
}

void Window_Destroy(Window* w)
{
    // SDL_ShowSimpleMessageBox(
    //     SDL_MESSAGEBOX_ERROR,
    //     "An Error Occured!",
    //     "Standard Error Message.",
    //     NULL
    // );

    SDL_DestroyRenderer(w->renderer);
    SDL_DestroyWindow(w->window);
    SDL_Quit();
}
