#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <Windows.h>

#include "../baloker.h"
#include "network.h"
#include "window.h"
#include "game.h"

uint8_t networkVersion = 1;

int serverOpen = 0;
int serverConnectionError = 0;
SOCKET sock;
char buffer[1024];

int _local_LoggedIn = 0;
int _local_accountDataFound = 0;

int _local_Chips = -676767;

char _local_Username[USERNAMEMAXLENGTH];
char _local_Password[PASSWORDMAXLENGTH];

BalokerLobbyHeader lobbyHeaders[10];
int numLobbyHeaders = 0;

void SendPacket(packet_type_t type, void *packet, uint16_t packet_size);

int CreateClient(const char *address, int port)
{
    printf("Connecting to %s:%d\n", address, port);

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 1;

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == INVALID_SOCKET)
        return 1;

    struct addrinfo hints;
    struct addrinfo *result = NULL;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char portS[16];
    snprintf(portS, sizeof(portS), "%d", port);

    if (getaddrinfo(address, portS, &hints, &result) != 0)
        return 1;

    if (connect(sock,
                result->ai_addr,
                (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        freeaddrinfo(result);
        closesocket(sock);
        WSACleanup();
        printf("Unable to connect to server.\n");
        return 1;
    }

    freeaddrinfo(result);

    printf("Connected.\n");
    serverOpen = 1;
    return 0;
}

int recv_full(void *buffer, int len)
{
    int total = 0;

    while (total < len)
    {
        int n = recv(sock, (char *)buffer + total, len - total, 0);

        if (n <= 0)
            return -1;

        total += n;
    }

    return total;
}

void cacheLoginDetails() {
    FILE *fd = fopen("acn.dat", "w+");
    if (fd != NULL) {
        fwrite(_local_Username, sizeof(char), USERNAMEMAXLENGTH, fd);
        fwrite(_local_Password, sizeof(char), PASSWORDMAXLENGTH, fd);
    }
    fclose(fd);
}

void ParseServerPacket()
{
    packet_header_t header;

    // read header
    if (recv_full(&header, sizeof(header)) < 0)
        return;

    if (header.length > 1024)
    {
        printf("Invalid packet size\n");
        return;
    }

    char payload[1024];
    memset(payload, 0, sizeof(payload));

    // read payload
    if (recv_full(payload, header.length) < 0)
        return;

    switch (header.type) {
        case fLoginResponse: {
            packet_fLoginResponse_t *packet = (packet_fLoginResponse_t *)(&payload);

            strncpy(LoginStatus, packet->response, sizeof(LoginStatus));
            loginDebounce = 0;

            if (packet->success == 1) {
                _local_LoggedIn = 1;
                cacheLoginDetails();
            } else {
                _local_accountDataFound = 0;
            }
        break; }

        case fGetUser: {
            packet_fGetUser_t *packet = (packet_fGetUser_t *)(&payload);

            if (packet->status == 0)
                break;

            if (strncmp(packet->username, _local_Username, USERNAMEMAXLENGTH) == 0) {
                // local user
                _local_Chips = packet->userChips;
            }
        break; }

        case fbYoureInALobby: {
            packet_fbYoureInALobby_t *packet = (packet_fbYoureInALobby_t *)(&payload);

            if (strncmp(packet->header.ownerName, _local_Username, USERNAMEMAXLENGTH) == 0)
                lobbyOwnerIsMe = 1;

            memcpy(&currentLobby, &packet->header, sizeof(BalokerLobbyHeader));
            Menu = LobbyWaiting;
        break; }

        case fbUserJoinedLobby: {
            packet_fbUserJoinedLobby_t *packet = (packet_fbUserJoinedLobby_t *)(&payload);

            strncpy(playerNames[gameState->playerCount], packet->username, USERNAMEMAXLENGTH);
            Players[gameState->playerCount].Chips = packet->chips;

            if (strncmp(packet->username, _local_Username, USERNAMEMAXLENGTH) == 0)
                LocalPlayer = gameState->playerCount;
            else
                currentLobby.Users++;

            gameState->playerCount++;
        break; }

        case fbGetLobbiesResp: {
            packet_fbGetLobbiesResponse_t *packet = (packet_fbGetLobbiesResponse_t *)(&payload);
            memcpy(lobbyHeaders, packet->results, sizeof(BalokerLobbyHeader) * packet->Lobbies);
            numLobbyHeaders = packet->Lobbies;
            loadLobbyDebounce = 0;

            printf("Loaded %d lobbies.\n", packet->Lobbies);
        break; }

        case fbJoinLobby: { 
            joinLobbyDebounce = 0;
        break; }

        case fbStartLobby: { 
            Menu = InGame;
        break; }
    }
}

void _sendHeader(packet_type_t type, uint16_t length)
{
    packet_header_t header = {type, length};
    send(sock, (char *)&header, sizeof(packet_header_t), 0);
}

void SendPacket(packet_type_t type, void *packet, uint16_t packet_size)
{
    _sendHeader(type, packet_size);
    send(sock, packet, packet_size, 0);
}

void AttemptLogin(char *username, char *password)
{
    packet_auth_t p = {0};

    strncpy(p.username, username, USERNAMEMAXLENGTH);
    strncpy(_local_Username, username, USERNAMEMAXLENGTH);

    strncpy(p.password, password, PASSWORDMAXLENGTH);
    strncpy(_local_Password, password, PASSWORDMAXLENGTH);

    SendPacket(AUTHENTICATION, &p, sizeof(p));
}

void AttemptRegister(char *username, char *password)
{
    packet_reg_t p = {0};

    strncpy(p.username, username, USERNAMEMAXLENGTH);
    strncpy(_local_Username, username, USERNAMEMAXLENGTH);

    strncpy(p.password, password, PASSWORDMAXLENGTH);
    strncpy(_local_Password, password, PASSWORDMAXLENGTH);

    SendPacket(REGISTER, &p, sizeof(p));
}

DWORD WINAPI NetworkThread(void *arg)
{
    // Check for local account data
    FILE *fd = fopen("acn.dat", "r");
    if (fd != NULL) {
        _local_accountDataFound = 1;
        fread(_local_Username, sizeof(char), USERNAMEMAXLENGTH, fd);
        fread(_local_Password, sizeof(char), PASSWORDMAXLENGTH, fd);
        fclose(fd);
    }

    // Connect to server
    int err = CreateClient(ADDRESS, PORT);

    if (err)
        err = CreateClient(ADDRESS, PORT2);     // Attempt 2
        // On debug PORT is local, on release PORT is on the pi

    if (err)
        serverConnectionError = 1;

    // Request Baloker
    send(sock, "2", 1, 0);

    // Get server version
    uint8_t ver;
    int n = recv(sock, &ver, sizeof(ver), 0);

    if (n <= 0) {
        serverConnectionError = 1;
        DestroyClient();
        serverOpen = 0;
    }

    if (ver != networkVersion) {
        serverConnectionError = 2;
        DestroyClient();
        serverOpen = 0;
    }

    if (_local_accountDataFound)  // auto login
        AttemptLogin(_local_Username, _local_Password);

    while (serverOpen)
    {
        ParseServerPacket();
    }

    return 0;
}

void DestroyClient()
{
    if (!serverOpen)
        return;

    closesocket(sock);
    WSACleanup();
}
