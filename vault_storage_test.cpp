#include <iostream>
#include <string>

#include <openssl/rand.h>

#include "KeyDerivation.h"
#include "CryptoManager.h"
#include "VaultStorage.h"

using namespace std;

int main()
{
    string password = "Secure123";

    // Generate salt
    unsigned char salt[16];

    if (RAND_bytes(salt, sizeof(salt)) != 1)
    {
        cout << "Salt generation failed." << endl;
        return 1;
    }

    // Derive key
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

    // Create crypto manager
    CryptoManager crypto(key);

    // Test credential
    string original =
        "Github|github@example.com|FakePassword123";

    // Encrypt
    string encrypted;

    if (!crypto.encrypt(original, encrypted))
    {
        cout << "Encryption failed." << endl;
        return 1;
    }

    // Save
    if (!VaultStorage::save(
            "test_vault.dat",
            encrypted))
    {
        cout << "Failed to save vault." << endl;
        return 1;
    }

    cout << "Encrypted vault saved successfully!" << endl;

    // Load
    string loaded;

    if (!VaultStorage::load(
            "test_vault.dat",
            loaded))
    {
        cout << "Failed to load vault." << endl;
        return 1;
    }

    cout << "Encrypted vault loaded successfully!" << endl;

    // Decrypt
    string decrypted;

    if (!crypto.decrypt(
            loaded,
            decrypted))
    {
        cout << "Decryption failed." << endl;
        return 1;
    }

    cout << "Decryption successful!" << endl;

    if (decrypted == original)
    {
        cout << "File storage test PASSED!" << endl;
        cout << "Recovered data: "
             << decrypted
             << endl;
    }
    else
    {
        cout << "File storage test FAILED!" << endl;
    }

    return 0;
}