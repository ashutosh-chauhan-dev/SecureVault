#ifndef KEYDERIVATION_H
#define KEYDERIVATION_H

#include <string>

class KeyDerivation
{
public:

    static bool deriveKey(
        const std::string& password,
        const unsigned char* salt,
        int saltLength,
        unsigned char* key,
        int keyLength
    );
};

#endif