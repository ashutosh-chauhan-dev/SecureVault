#include "MasterPassword.h"

#include "KeyDerivation.h"

#include <openssl/rand.h>
#include <cctype>

bool MasterPassword::validate(
    const std::string& password,
    std::string& errorMessage
)
{
    if (password.empty())
    {
        errorMessage = "Master password cannot be empty.";
        return false;
    }

    if (password.length() < 12)
    {
        errorMessage = "Master password must be at least 12 characters long.";
        return false;
    }

    bool hasNonWhitespace = false;

    for (char ch : password)
    {
        if (!std::isspace(static_cast<unsigned char>(ch)))
        {
            hasNonWhitespace = true;
            break;
        }
    }

    if (!hasNonWhitespace)
    {
        errorMessage = "Master password cannot consist entirely of whitespace.";
        return false;
    }

    errorMessage.clear();
    return true;
}

bool MasterPassword::validate(
    const std::string& password
)
{
    std::string dummyError;
    return validate(password, dummyError);
}

bool MasterPassword::create(
    const std::string& password,
    unsigned char* salt,
    int saltLength,
    unsigned char* key,
    int keyLength
)
{
    // Generate a new random salt
    if (RAND_bytes(
            salt,
            saltLength) != 1)
    {
        return false;
    }

    // Derive encryption key
    return KeyDerivation::deriveKey(
        password,
        salt,
        saltLength,
        key,
        keyLength
    );
}

bool MasterPassword::deriveKey(
    const std::string& password,
    const unsigned char* salt,
    int saltLength,
    unsigned char* key,
    int keyLength
)
{
    return KeyDerivation::deriveKey(
        password,
        salt,
        saltLength,
        key,
        keyLength
    );
}