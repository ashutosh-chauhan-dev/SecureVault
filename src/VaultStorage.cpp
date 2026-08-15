#include "VaultStorage.h"

#include <fstream>
#include <cstring>

namespace
{
    const char MAGIC[] = "SVLT001";
    const int VERSION = 1;
}

bool VaultStorage::save(
    const std::string& filename,
    const unsigned char* salt,
    int saltLength,
    const std::string& encryptedData
)
{
    if (saltLength != 16)
    {
        return false;
    }

    std::ofstream file(
        filename,
        std::ios::binary
    );

    if (!file)
    {
        return false;
    }

    // Write magic
    file.write(
        MAGIC,
        sizeof(MAGIC)
    );

    // Write version
    file.write(
        reinterpret_cast<const char*>(&VERSION),
        sizeof(VERSION)
    );

    // Write salt
    file.write(
        reinterpret_cast<const char*>(salt),
        saltLength
    );

    // Write encrypted data length
    int dataLength =
        static_cast<int>(encryptedData.size());

    file.write(
        reinterpret_cast<const char*>(&dataLength),
        sizeof(dataLength)
    );

    // Write encrypted data
    file.write(
        encryptedData.data(),
        dataLength
    );

    return file.good();
}

bool VaultStorage::load(
    const std::string& filename,
    unsigned char* salt,
    int saltLength,
    std::string& encryptedData
)
{
    if (saltLength != 16)
    {
        return false;
    }

    std::ifstream file(
        filename,
        std::ios::binary
    );

    if (!file)
    {
        return false;
    }

    // Read magic
    char magic[sizeof(MAGIC)];

    file.read(
        magic,
        sizeof(MAGIC)
    );

    if (std::memcmp(
            magic,
            MAGIC,
            sizeof(MAGIC)) != 0)
    {
        return false;
    }

    // Read version
    int version;

    file.read(
        reinterpret_cast<char*>(&version),
        sizeof(version)
    );

    if (version != VERSION)
    {
        return false;
    }

    // Read salt
    file.read(
        reinterpret_cast<char*>(salt),
        saltLength
    );

    // Read encrypted data length
    int dataLength;

    file.read(
        reinterpret_cast<char*>(&dataLength),
        sizeof(dataLength)
    );

    if (!file || dataLength < 0)
    {
        return false;
    }

    // Read encrypted data
    encryptedData.resize(dataLength);

    if (dataLength > 0)
    {
        file.read(
            &encryptedData[0],
            dataLength
        );
    }

    return file.good() || file.eof();
}