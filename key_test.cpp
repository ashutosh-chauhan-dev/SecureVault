#include <iostream>
#include <string>

#include <openssl/rand.h>

#include "KeyDerivation.h"

using namespace std;

int main()
{
    string password = "Secure123";

    unsigned char salt[16];

    if (RAND_bytes(salt, sizeof(salt)) != 1)
    {
        cout << "Failed to generate salt." << endl;
        return 1;
    }

    unsigned char key[32];

    if (!KeyDerivation::deriveKey(
            password,
            salt,
            sizeof(salt),
            key,
            sizeof(key)))
    {
        cout << "Key derivation failed." << endl;
        return 1;
    }

    cout << "Key derivation successful!" << endl;

    return 0;
}