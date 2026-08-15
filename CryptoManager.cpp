#include "CryptoManager.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

#include <cstring>
#include <vector>

CryptoManager::CryptoManager(const unsigned char* encryptionKey)
{
    std::memcpy(key, encryptionKey, 32);
}

CryptoManager::~CryptoManager()
{
    OPENSSL_cleanse(key, sizeof(key));
}

bool CryptoManager::encrypt(
    const std::string& plaintext,
    std::string& ciphertext
)
{
    // Generate random IV
    unsigned char iv[12];

    if (RAND_bytes(iv, sizeof(iv)) != 1)
    {
        return false;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (ctx == nullptr)
    {
        return false;
    }

    if (EVP_EncryptInit_ex(
            ctx,
            EVP_aes_256_gcm(),
            nullptr,
            key,
            iv) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Allocate enough space for the plaintext.
    // GCM does not add significant padding, but one extra block
    // keeps the buffer safe for EVP operations.
    std::vector<unsigned char> encrypted(
        plaintext.size() + EVP_MAX_BLOCK_LENGTH
    );

    int encryptedLength = 0;
    int finalLength = 0;

    if (EVP_EncryptUpdate(
            ctx,
            encrypted.data(),
            &encryptedLength,
            reinterpret_cast<const unsigned char*>(
                plaintext.data()
            ),
            static_cast<int>(plaintext.size())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (EVP_EncryptFinal_ex(
            ctx,
            encrypted.data() + encryptedLength,
            &finalLength) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    encryptedLength += finalLength;

    // Get authentication tag
    unsigned char tag[16];

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_GET_TAG,
            sizeof(tag),
            tag) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    EVP_CIPHER_CTX_free(ctx);

    /*
        Store:
        IV + TAG + CIPHERTEXT
    */

    ciphertext.clear();

    ciphertext.append(
        reinterpret_cast<char*>(iv),
        sizeof(iv)
    );

    ciphertext.append(
        reinterpret_cast<char*>(tag),
        sizeof(tag)
    );

    ciphertext.append(
        reinterpret_cast<char*>(encrypted.data()),
        encryptedLength
    );

    OPENSSL_cleanse(encrypted.data(), encrypted.size());

    return true;
}

bool CryptoManager::decrypt(
    const std::string& ciphertext,
    std::string& plaintext
)
{
    // Minimum size = IV + TAG
    if (ciphertext.size() < 28)
    {
        return false;
    }

    const unsigned char* data =
        reinterpret_cast<const unsigned char*>(
            ciphertext.data()
        );

    const unsigned char* iv = data;

    const unsigned char* tag = data + 12;

    const unsigned char* encrypted =
        data + 28;

    int encryptedLength =
        static_cast<int>(ciphertext.size() - 28);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (ctx == nullptr)
    {
        return false;
    }

    if (EVP_DecryptInit_ex(
            ctx,
            EVP_aes_256_gcm(),
            nullptr,
            key,
            iv) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    std::vector<unsigned char> decrypted(
        static_cast<size_t>(encryptedLength) +
        EVP_MAX_BLOCK_LENGTH
    );

    int decryptedLength = 0;
    int finalLength = 0;

    if (EVP_DecryptUpdate(
            ctx,
            decrypted.data(),
            &decryptedLength,
            encrypted,
            encryptedLength) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Set authentication tag
    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_SET_TAG,
            16,
            const_cast<unsigned char*>(tag)) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // This verifies the authentication tag
    int result = EVP_DecryptFinal_ex(
        ctx,
        decrypted.data() + decryptedLength,
        &finalLength
    );

    if (result <= 0)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    decryptedLength += finalLength;

    plaintext.assign(
        reinterpret_cast<char*>(decrypted.data()),
        decryptedLength
    );

    OPENSSL_cleanse(decrypted.data(), decrypted.size());

    EVP_CIPHER_CTX_free(ctx);

    return true;
}