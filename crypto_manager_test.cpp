#include <iostream>
#include <string>

#include <openssl/rand.h>

#include "CryptoManager.h"

using namespace std;

int main()
{
    unsigned char key[32];

    if (RAND_bytes(key, sizeof(key)) != 1)
    {
        cout << "Failed to generate key." << endl;
        return 1;
    }

    CryptoManager crypto(key);

    string plaintext = "Hello SecureVault";

    string encrypted;
    string decrypted;

    if (!crypto.encrypt(plaintext, encrypted))
    {
        cout << "Encryption failed." << endl;
        return 1;
    }

    cout << "Encryption successful!" << endl;
    cout << "Encrypted data size: "
         << encrypted.size()
         << " bytes" << endl;

    if (!crypto.decrypt(encrypted, decrypted))
    {
        cout << "Decryption failed." << endl;
        return 1;
    }

    cout << "Decryption successful!" << endl;
    cout << "Decrypted text: "
         << decrypted
         << endl;

    return 0;
}