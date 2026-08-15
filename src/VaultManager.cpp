#include "VaultManager.h"

#include "MasterPassword.h"
#include "CryptoManager.h"
#include "VaultStorage.h"

#include <openssl/crypto.h>

#include <sstream>
#include <cstdint>
#include <limits>

namespace
{
    // New credential serialization format identifier.
    const char FORMAT_MAGIC[] = "SV2";

    // Maximum size allowed for one credential field.
    const uint32_t MAX_FIELD_SIZE = 1024 * 1024; // 1 MB

    void appendUint32(
        std::string& data,
        uint32_t value
    )
    {
        data.push_back(
            static_cast<char>(value & 0xFF)
        );

        data.push_back(
            static_cast<char>((value >> 8) & 0xFF)
        );

        data.push_back(
            static_cast<char>((value >> 16) & 0xFF)
        );

        data.push_back(
            static_cast<char>((value >> 24) & 0xFF)
        );
    }

    bool readUint32(
        const std::string& data,
        size_t& position,
        uint32_t& value
    )
    {
        if (position + 4 > data.size())
        {
            return false;
        }

        const unsigned char* bytes =
            reinterpret_cast<const unsigned char*>(
                data.data() + position
            );

        value =
            static_cast<uint32_t>(bytes[0]) |
            (static_cast<uint32_t>(bytes[1]) << 8) |
            (static_cast<uint32_t>(bytes[2]) << 16) |
            (static_cast<uint32_t>(bytes[3]) << 24);

        position += 4;

        return true;
    }

    void appendField(
        std::string& data,
        const std::string& field
    )
    {
        appendUint32(
            data,
            static_cast<uint32_t>(field.size())
        );

        data.append(field);
    }

    bool readField(
        const std::string& data,
        size_t& position,
        std::string& field
    )
    {
        uint32_t fieldSize = 0;

        if (!readUint32(
                data,
                position,
                fieldSize))
        {
            return false;
        }

        if (fieldSize > MAX_FIELD_SIZE)
        {
            return false;
        }

        if (position + fieldSize > data.size())
        {
            return false;
        }

        field.assign(
            data.data() + position,
            fieldSize
        );

        position += fieldSize;

        return true;
    }

    bool serializeCredentials(
        const std::vector<Credential>& credentials,
        std::string& output
    )
    {
        output.clear();

        // Format identifier.
        output.append(FORMAT_MAGIC, 3);

        if (credentials.size() >
            std::numeric_limits<uint32_t>::max())
        {
            return false;
        }

        appendUint32(
            output,
            static_cast<uint32_t>(credentials.size())
        );

        for (const Credential& credential : credentials)
        {
            if (credential.website.size() > MAX_FIELD_SIZE ||
                credential.username.size() > MAX_FIELD_SIZE ||
                credential.password.size() > MAX_FIELD_SIZE)
            {
                return false;
            }

            appendField(
                output,
                credential.website
            );

            appendField(
                output,
                credential.username
            );

            appendField(
                output,
                credential.password
            );
        }

        return true;
    }

    bool deserializeCredentials(
        const std::string& data,
        std::vector<Credential>& credentials
    )
    {
        if (data.size() < 7)
        {
            return false;
        }

        if (data.compare(0, 3, FORMAT_MAGIC) != 0)
        {
            return false;
        }

        size_t position = 3;

        uint32_t credentialCount = 0;

        if (!readUint32(
                data,
                position,
                credentialCount))
        {
            return false;
        }

        // Prevent unreasonable allocations.
        if (credentialCount > 100000)
        {
            return false;
        }

        std::vector<Credential> loadedCredentials;

        for (uint32_t i = 0;
             i < credentialCount;
             ++i)
        {
            Credential credential;

            if (!readField(
                    data,
                    position,
                    credential.website))
            {
                return false;
            }

            if (!readField(
                    data,
                    position,
                    credential.username))
            {
                return false;
            }

            if (!readField(
                    data,
                    position,
                    credential.password))
            {
                return false;
            }

            loadedCredentials.push_back(
                credential
            );
        }

        // No unexpected bytes should remain.
        if (position != data.size())
        {
            return false;
        }

        credentials = loadedCredentials;

        return true;
    }

    bool deserializeLegacyCredentials(
        const std::string& data,
        std::vector<Credential>& credentials
    )
    {
        std::stringstream stream(data);

        std::string line;

        std::vector<Credential> loadedCredentials;

        while (std::getline(stream, line))
        {
            if (line.empty())
            {
                continue;
            }

            std::stringstream lineStream(line);

            Credential credential;

            if (!std::getline(
                    lineStream,
                    credential.website,
                    '|'))
            {
                return false;
            }

            if (!std::getline(
                    lineStream,
                    credential.username,
                    '|'))
            {
                return false;
            }

            if (!std::getline(
                    lineStream,
                    credential.password))
            {
                return false;
            }

            loadedCredentials.push_back(
                credential
            );
        }

        credentials = loadedCredentials;

        return true;
    }
}

VaultManager::VaultManager(
    const std::string& filename
)
{
    vaultFile = filename;
    unlocked = false;

    for (int i = 0; i < 16; i++)
    {
        salt[i] = 0;
    }

    for (int i = 0; i < 32; i++)
    {
        key[i] = 0;
    }
}

VaultManager::~VaultManager()
{
    OPENSSL_cleanse(key, sizeof(key));
    OPENSSL_cleanse(salt, sizeof(salt));
}

bool VaultManager::createVault(
    const std::string& masterPassword
)
{
    if (masterPassword.empty())
    {
        return false;
    }

    if (!MasterPassword::create(
            masterPassword,
            salt,
            sizeof(salt),
            key,
            sizeof(key)))
    {
        return false;
    }

    credentials.clear();

    unlocked = true;

    return saveVault();
}

bool VaultManager::unlockVault(
    const std::string& masterPassword
)
{
    unsigned char loadedSalt[16];

    std::string encryptedData;

    if (!VaultStorage::load(
            vaultFile,
            loadedSalt,
            sizeof(loadedSalt),
            encryptedData))
    {
        return false;
    }

    unsigned char derivedKey[32];

    if (!MasterPassword::deriveKey(
            masterPassword,
            loadedSalt,
            sizeof(loadedSalt),
            derivedKey,
            sizeof(derivedKey)))
    {
        return false;
    }

    CryptoManager crypto(derivedKey);

    std::string decryptedData;

    if (!crypto.decrypt(
            encryptedData,
            decryptedData))
    {
        return false;
    }

    std::vector<Credential> loadedCredentials;

    bool parsed = false;

    // New format.
    if (decryptedData.size() >= 3 &&
        decryptedData.compare(
            0,
            3,
            FORMAT_MAGIC) == 0)
    {
        parsed = deserializeCredentials(
            decryptedData,
            loadedCredentials
        );
    }
    else
    {
        // Backward compatibility with the old
        // website|username|password format.
        parsed = deserializeLegacyCredentials(
            decryptedData,
            loadedCredentials
        );
    }

    if (!parsed)
    {
        return false;
    }

    // Copy loaded salt and key.
    for (int i = 0; i < 16; i++)
    {
        salt[i] = loadedSalt[i];
    }

    for (int i = 0; i < 32; i++)
    {
        key[i] = derivedKey[i];
    }

    credentials = loadedCredentials;

    unlocked = true;

    return true;
}

bool VaultManager::saveVault()
{
    if (!unlocked)
    {
        return false;
    }

    std::string plainData;

    if (!serializeCredentials(
            credentials,
            plainData))
    {
        return false;
    }

    CryptoManager crypto(key);

    std::string encryptedData;

    if (!crypto.encrypt(
            plainData,
            encryptedData))
    {
        return false;
    }

    return VaultStorage::save(
        vaultFile,
        salt,
        sizeof(salt),
        encryptedData
    );
}

bool VaultManager::isUnlocked() const
{
    return unlocked;
}

void VaultManager::addCredential(
    const Credential& credential
)
{
    credentials.push_back(credential);
}

const std::vector<Credential>&
VaultManager::getCredentials() const
{
    return credentials;
}

bool VaultManager::searchCredential(
    const std::string& website,
    Credential& result
) const
{
    for (const Credential& credential : credentials)
    {
        if (credential.website == website)
        {
            result = credential;
            return true;
        }
    }

    return false;
}

bool VaultManager::deleteCredential(
    const std::string& website
)
{
    for (auto it = credentials.begin();
         it != credentials.end();
         ++it)
    {
        if (it->website == website)
        {
            credentials.erase(it);
            return true;
        }
    }

    return false;
}

bool VaultManager::editCredential(
    const std::string& website,
    const std::string& newUsername,
    const std::string& newPassword
)
{
    for (Credential& credential : credentials)
    {
        if (credential.website == website)
        {
            credential.username = newUsername;
            credential.password = newPassword;

            return true;
        }
    }

    return false;
}