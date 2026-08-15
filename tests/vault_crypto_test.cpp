#include <iostream>
#include <string>

#include <openssl/rand.h>

#include "KeyDerivation.h"
#include "CryptoManager.h"

using namespace std;

int main()
{
    // -------------------------
    // 1. Master password
    // -------------------------

    string password = "Secure123";

    // -------------------------
    // 2. Generate random salt
    // -------------------------

    unsigned char salt[16];

    if (RAND_bytes(salt, sizeof(salt)) != 1)
    {
        cout << "Failed to generate salt." << endl;
        return 1;
    }

    // -------------------------
    // 3. Derive encryption key
    // -------------------------

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

    cout << "Master password processed successfully!" << endl;

    // -------------------------
    // 4. Create CryptoManager
    // -------------------------

    CryptoManager crypto(key);

    // -------------------------
    // 5. Credential data
    // -------------------------

    string originalData =
        "Github|github@example.com|FakePassword123";

    // -------------------------
    // 6. Encrypt
    // -------------------------

    string encryptedData;

    if (!crypto.encrypt(originalData, encryptedData))
    {
        cout << "Encryption failed." << endl;
        return 1;
    }

    cout << "Credential encrypted successfully!" << endl;

    // -------------------------
    // 7. Decrypt
    // -------------------------

    string decryptedData;

    if (!crypto.decrypt(encryptedData, decryptedData))
    {
        cout << "Decryption failed." << endl;
        return 1;
    }

    cout << "Credential decrypted successfully!" << endl;

    // -------------------------
    // 8. Compare
    // -------------------------

    if (decryptedData == originalData)
    {
        cout << "Data verification successful!" << endl;
        cout << "Decrypted data: "
             << decryptedData
             << endl;
    }
    else
    {
        cout << "Data verification FAILED!" << endl;
    }

    return 0;
}