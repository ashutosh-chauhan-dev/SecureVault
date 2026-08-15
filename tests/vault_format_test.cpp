#include <iostream>
#include <string>

#include <openssl/rand.h>

#include "KeyDerivation.h"
#include "CryptoManager.h"
#include "VaultStorage.h"

using namespace std;

int main()
{
    const string filename = "vault.dat";

    // -----------------------------
    // Generate random salt
    // -----------------------------

    unsigned char salt[16];

    if (RAND_bytes(salt, sizeof(salt)) != 1)
    {
        cout << "Salt generation failed." << endl;
        return 1;
    }

    // -----------------------------
    // Create encryption key
    // -----------------------------

    string password = "Secure123";

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

    // -----------------------------
    // Create CryptoManager
    // -----------------------------

    CryptoManager crypto(key);

    // -----------------------------
    // Test credential
    // -----------------------------

    string original =
        "Github|github@example.com|FakePassword123";

    // -----------------------------
    // Encrypt
    // -----------------------------

    string encrypted;

    if (!crypto.encrypt(
            original,
            encrypted))
    {
        cout << "Encryption failed." << endl;
        return 1;
    }

    // -----------------------------
    // Save vault
    // -----------------------------

    if (!VaultStorage::save(
            filename,
            salt,
            sizeof(salt),
            encrypted))
    {
        cout << "Vault save failed." << endl;
        return 1;
    }

    cout << "Vault saved successfully!" << endl;

    // -----------------------------
    // Load vault
    // -----------------------------

    unsigned char loadedSalt[16];

    string loadedEncrypted;

    if (!VaultStorage::load(
            filename,
            loadedSalt,
            sizeof(loadedSalt),
            loadedEncrypted))
    {
        cout << "Vault load failed." << endl;
        return 1;
    }

    cout << "Vault loaded successfully!" << endl;

    // -----------------------------
    // Derive key again
    // -----------------------------

    unsigned char loadedKey[32];

    if (!KeyDerivation::deriveKey(
            password,
            loadedSalt,
            sizeof(loadedSalt),
            loadedKey,
            sizeof(loadedKey)))
    {
        cout << "Second key derivation failed." << endl;
        return 1;
    }

    // -----------------------------
    // Decrypt
    // -----------------------------

    CryptoManager loadedCrypto(loadedKey);

    string decrypted;

    if (!loadedCrypto.decrypt(
            loadedEncrypted,
            decrypted))
    {
        cout << "Decryption failed." << endl;
        return 1;
    }

    cout << "Decryption successful!" << endl;

    // -----------------------------
    // Verify
    // -----------------------------

    if (decrypted == original)
    {
        cout << "Vault format test PASSED!" << endl;
        cout << "Recovered data: "
             << decrypted
             << endl;
    }
    else
    {
        cout << "Vault format test FAILED!" << endl;
    }

    return 0;
}