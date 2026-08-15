#include "KeyDerivation.h"

#include <openssl/evp.h>

bool KeyDerivation::deriveKey(
    const std::string& password,
    const unsigned char* salt,
    int saltLength,
    unsigned char* key,
    int keyLength
)
{
    return PKCS5_PBKDF2_HMAC(
        password.c_str(),
        static_cast<int>(password.length()),
        salt,
        saltLength,
        100000,
        EVP_sha256(),
        keyLength,
        key
    ) == 1;
}