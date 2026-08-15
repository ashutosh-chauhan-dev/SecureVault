#ifndef VAULTMANAGER_H
#define VAULTMANAGER_H

#include <string>
#include <vector>

struct Credential
{
    std::string website;
    std::string username;
    std::string password;
};

class VaultManager
{
private:
    std::vector<Credential> credentials;

    unsigned char salt[16];
    unsigned char key[32];

    bool unlocked;

    std::string vaultFile;

public:
    VaultManager(
        const std::string& filename
    );
    ~VaultManager();

    bool createVault(
        const std::string& masterPassword
    );

    bool unlockVault(
        const std::string& masterPassword
    );

    bool saveVault();

    bool isUnlocked() const;

    void addCredential(
        const Credential& credential
    );

    const std::vector<Credential>& getCredentials() const;

    bool searchCredential(
        const std::string& website,
        Credential& result
    ) const;

    bool deleteCredential(
        const std::string& website
    );

    bool editCredential(
        const std::string& website,
        const std::string& newUsername,
        const std::string& newPassword
    );
};

#endif