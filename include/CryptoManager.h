#ifndef CRYPTOMANAGER_H
#define CRYPTOMANAGER_H

#include <string>

class CryptoManager
{
private:
    unsigned char key[32];

public:
    CryptoManager(const unsigned char* encryptionKey);
    ~CryptoManager();

    bool encrypt(
        const std::string& plaintext,
        std::string& ciphertext
    );

    bool decrypt(
        const std::string& ciphertext,
        std::string& plaintext
    );
};

#endif