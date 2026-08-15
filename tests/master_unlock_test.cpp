#include <iostream>
#include <string>

#include "MasterPassword.h"
#include "CryptoManager.h"

using namespace std;

int main()
{
    const string correctPassword = "Secure123";

    // --------------------------------
    // Create vault credentials
    // --------------------------------

    unsigned char salt[16];
    unsigned char key[32];

    if (!MasterPassword::create(
            correctPassword,
            salt,
            sizeof(salt),
            key,
            sizeof(key)))
    {
        cout << "Failed to create master password." << endl;
        return 1;
    }

    CryptoManager crypto(key);

    string originalData =
        "Github|github@example.com|FakePassword123";

    string encryptedData;

    if (!crypto.encrypt(
            originalData,
            encryptedData))
    {
        cout << "Encryption failed." << endl;
        return 1;
    }

    cout << "Vault encrypted successfully!" << endl;

    // --------------------------------
    // Test correct password
    // --------------------------------

    string enteredPassword = "Secure123";

    unsigned char correctKey[32];

    MasterPassword::deriveKey(
        enteredPassword,
        salt,
        sizeof(salt),
        correctKey,
        sizeof(correctKey)
    );

    CryptoManager correctCrypto(correctKey);

    string decryptedData;

    if (correctCrypto.decrypt(
            encryptedData,
            decryptedData))
    {
        cout << "Correct password: Access granted!" << endl;
        cout << "Decrypted data: "
             << decryptedData
             << endl;
    }
    else
    {
        cout << "Correct password: Access denied!" << endl;
    }

    // --------------------------------
    // Test WRONG password
    // --------------------------------

    enteredPassword = "WrongPassword";

    unsigned char wrongKey[32];

    MasterPassword::deriveKey(
        enteredPassword,
        salt,
        sizeof(salt),
        wrongKey,
        sizeof(wrongKey)
    );

    CryptoManager wrongCrypto(wrongKey);

    string wrongDecryptedData;

    if (wrongCrypto.decrypt(
            encryptedData,
            wrongDecryptedData))
    {
        cout << "Wrong password: ERROR - access granted!"
             << endl;
    }
    else
    {
        cout << "Wrong password: Access denied!" << endl;
    }

    return 0;
}