#ifndef VAULTSTORAGE_H
#define VAULTSTORAGE_H

#include <string>

class VaultStorage
{
public:

    static bool save(
        const std::string& filename,
        const unsigned char* salt,
        int saltLength,
        const std::string& encryptedData
    );

    static bool load(
        const std::string& filename,
        unsigned char* salt,
        int saltLength,
        std::string& encryptedData
    );
};

#endif