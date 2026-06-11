#include <WinSock2.h>
#include <sodium.h>


#define usernameMaxLength 20


typedef struct {
    unsigned int ID;
    int Chips;
    char Username[usernameMaxLength];
    char PassHash[crypto_pwhash_STRBYTES];
} User;

/*
int _ = crypto_pwhash_str(
    hash, password, sizeof(password),
    crypto_pwhash_OPSLIMIT_INTERACTIVE,
    crypto_pwhash_MEMLIMIT_INTERACTIVE
);
*/

/*
if (crypto_pwhash_str_verify(
        stored_hash,
        password,
        strlen(password)) == 0) {
    // correct password
} else {
    // wrong password
}
*/

int main() {
    printf("Loading Baloker Server...\n");

    printf("Initializing Sodium...\n");
    if (sodium_init() < 0) {
        perror("Sodium failed to initialize");
        return 1;
    }

    printf("User Struct Size: %dB", sizeof(User));

    return 0;
}