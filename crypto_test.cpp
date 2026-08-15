#include <iostream>
#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

int main()
{
    string password = "Secure123";

    unsigned char salt[16];

    // Generate random salt
    if (RAND_bytes(salt, sizeof(salt)) != 1)
    {
        cout << "Failed to generate salt." << endl;
        return 1;
    }

    unsigned char key[32];

    // Derive a 256-bit key using PBKDF2
    if (PKCS5_PBKDF2_HMAC(
            password.c_str(),
            password.length(),
            salt,
            sizeof(salt),
            100000,
            EVP_sha256(),
            sizeof(key),
            key) != 1)
    {
        cout << "Key derivation failed." << endl;
        return 1;
    }

    cout << "Password-based key generated successfully!" << endl;

    return 0;
}