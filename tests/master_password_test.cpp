#include <iostream>
#include <string>
#include <cstring>

#include "MasterPassword.h"

using namespace std;

int main()
{
    string password = "Secure123";

    unsigned char salt[16];
    unsigned char key[32];

    // Create master password
    if (!MasterPassword::create(
            password,
            salt,
            sizeof(salt),
            key,
            sizeof(key)))
    {
        cout << "Master password creation failed." << endl;
        return 1;
    }

    cout << "Master password created successfully!" << endl;

    // Derive the key again using the same password and salt
    unsigned char secondKey[32];

    if (!MasterPassword::deriveKey(
            password,
            salt,
            sizeof(salt),
            secondKey,
            sizeof(secondKey)))
    {
        cout << "Key derivation failed." << endl;
        return 1;
    }

    // Compare keys
    if (memcmp(
            key,
            secondKey,
            sizeof(key)) == 0)
    {
        cout << "Master password verification test PASSED!" << endl;
    }
    else
    {
        cout << "Master password verification test FAILED!" << endl;
    }

    return 0;
}