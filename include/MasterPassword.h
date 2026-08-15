#ifndef MASTERPASSWORD_H
#define MASTERPASSWORD_H

#include <string>

class MasterPassword
{
public:

    static bool validate(
        const std::string& password,
        std::string& errorMessage
    );

    static bool validate(
        const std::string& password
    );

    static bool create(
        const std::string& password,
        unsigned char* salt,
        int saltLength,
        unsigned char* key,
        int keyLength
    );

    static bool deriveKey(
        const std::string& password,
        const unsigned char* salt,
        int saltLength,
        unsigned char* key,
        int keyLength
    );
};

#endif