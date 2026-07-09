#pragma once

#include <stdint.h>
#include <Windows.h>

#define USERNAMEMAXLENGTH 20
#define PASSWORDMAXLENGTH 20

typedef enum {
    BANNED = 0,
    USER,
    ADMIN
} user_level_t;

extern int serverOpen;
extern int serverConnectionError;
extern int _local_LoggedIn;
extern int _local_accountDataFound;
extern char _local_Username[USERNAMEMAXLENGTH];

extern int _local_Chips;

int CreateClient(const char *address, int port);
void DestroyClient();
DWORD WINAPI NetworkThread(void *arg);

// KTSS Specifications - DO NOT EDIT
typedef void User;

typedef struct {
    char Code[4];       // Join code
    User *owner;        // Only owner can start / delete
    char ownerName[USERNAMEMAXLENGTH];
    int Users;          // how many people are in the lobby
    int MaxUsers;       // max people in the lobby
    int Private;        // Private lobby?
} BalokerLobbyHeader;       // Header is sent out on GetLobby requests
extern BalokerLobbyHeader lobbyHeaders[10];
extern int numLobbyHeaders;

extern uint8_t networkVersion;

// Packets
typedef enum {      // PACKETS V2
    // Console Packets; Uses PING for statuses to minimize packet types
    AUTHENTICATION,
    PING,
    MSGEND,
    REGISTER,
    CMD,

    // Front Packets (to be used in public applications) are prefixed with "f"
    fGetUser,
    fLoginResponse,

    // Baloker Specific
    fbLobby,
    fbGetLobbiesReq,
    fbGetLobbiesResp,
    fbJoinLobby,
    fbLeaveLobby,
    fbStartLobby,
    fbCreateLobby,
    fbYoureInALobby,
    fbUserJoinedLobby,
    fbUserLeftLobby
} packet_type_t;

#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint16_t length;
} packet_header_t;
#pragma pack(pop)

typedef struct {
    char username[USERNAMEMAXLENGTH];
    char password[PASSWORDMAXLENGTH];
} packet_auth_t;

typedef struct {
    char msg[256];
} packet_ping_t;

typedef struct {
    uint8_t _;
} packet_end_t;

typedef struct {
    char username[USERNAMEMAXLENGTH];
    char password[PASSWORDMAXLENGTH];
} packet_reg_t;

typedef struct {
    uint64_t ID;
    char username[USERNAMEMAXLENGTH];
    int status;     // 0 = no user found; 1 = user found
    user_level_t level;
    int userChips;
} packet_fGetUser_t;

typedef struct {
    uint8_t success;
    char response[30];
} packet_fLoginResponse_t;

typedef struct {
    uint8_t create; // client: 1 = create new; 0 = destroy (via code)
                    // server: 1 = joining lobby; 0 = leaving lobby
    char code[4];
    int MaxPlayers;
    int Private;    // code not shown on public list
} packet_fbLobby_t;

typedef struct {
    uint8_t _;
} packet_fbGetLobbiesRequest_t;

typedef struct {
    int Lobbies;    // i really doubt there will be more than 10 lobbies at once
    BalokerLobbyHeader results[10];
} packet_fbGetLobbiesResponse_t;

typedef struct {
    BalokerLobbyHeader header;
} packet_fbYoureInALobby_t;

typedef struct {
    char code[4];
} packet_fbJoinLobby_t;

typedef struct {
    uint8_t _;
} packet_fbLeaveLobby_t;

typedef struct {
    int MaxUsers;
    int Private;
} packet_fbCreateLobby_t;

typedef struct {
    char username[USERNAMEMAXLENGTH];
    int chips;
} packet_fbUserJoinedLobby_t;

typedef struct {
    char username[USERNAMEMAXLENGTH];
} packet_fbUserLeftLobby_t;


// Utils
void SendPacket(packet_type_t type, void *packet, uint16_t packet_size);
void AttemptLogin(char *username, char *password);
void AttemptRegister(char *username, char *password);
