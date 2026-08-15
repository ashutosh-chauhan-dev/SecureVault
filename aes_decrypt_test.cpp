#include <iostream>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

int main()
{
    string plaintext = "GitHub|student@example.com|FakePassword123";

    // Generate a random 256-bit key
    unsigned char key[32];

    if (RAND_bytes(key, sizeof(key)) != 1)
    {
        cout << "Failed to generate key." << endl;
        return 1;
    }

    // Generate a random 96-bit IV
    unsigned char iv[12];

    if (RAND_bytes(iv, sizeof(iv)) != 1)
    {
        cout << "Failed to generate IV." << endl;
        return 1;
    }

    // =========================
    // ENCRYPT
    // =========================

    EVP_CIPHER_CTX* encryptCtx = EVP_CIPHER_CTX_new();

    if (encryptCtx == nullptr)
    {
        cout << "Failed to create encryption context." << endl;
        return 1;
    }

    EVP_EncryptInit_ex(
        encryptCtx,
        EVP_aes_256_gcm(),
        nullptr,
        key,
        iv
    );

    unsigned char ciphertext[1024];

    int ciphertextLength = 0;
    int finalLength = 0;

    EVP_EncryptUpdate(
        encryptCtx,
        ciphertext,
        &ciphertextLength,
        reinterpret_cast<const unsigned char*>(plaintext.c_str()),
        plaintext.length()
    );

    EVP_EncryptFinal_ex(
        encryptCtx,
        ciphertext + ciphertextLength,
        &finalLength
    );

    ciphertextLength += finalLength;

    // Get authentication tag
    unsigned char tag[16];

    EVP_CIPHER_CTX_ctrl(
        encryptCtx,
        EVP_CTRL_GCM_GET_TAG,
        sizeof(tag),
        tag
    );

    EVP_CIPHER_CTX_free(encryptCtx);

    cout << "Encryption successful!" << endl;

    // =========================
    // DECRYPT
    // =========================

    EVP_CIPHER_CTX* decryptCtx = EVP_CIPHER_CTX_new();

    if (decryptCtx == nullptr)
    {
        cout << "Failed to create decryption context." << endl;
        return 1;
    }

    EVP_DecryptInit_ex(
        decryptCtx,
        EVP_aes_256_gcm(),
        nullptr,
        key,
        iv
    );

    unsigned char decrypted[1024];

    int decryptedLength = 0;
    int finalDecryptedLength = 0;

    EVP_DecryptUpdate(
        decryptCtx,
        decrypted,
        &decryptedLength,
        ciphertext,
        ciphertextLength
    );

    // Give the authentication tag to OpenSSL
    EVP_CIPHER_CTX_ctrl(
        decryptCtx,
        EVP_CTRL_GCM_SET_TAG,
        sizeof(tag),
        tag
    );

    int result = EVP_DecryptFinal_ex(
        decryptCtx,
        decrypted + decryptedLength,
        &finalDecryptedLength
    );

    if (result > 0)
    {
        decryptedLength += finalDecryptedLength;

        decrypted[decryptedLength] = '\0';

        cout << "Decryption successful!" << endl;
        cout << "Decrypted data: "
             << decrypted
             << endl;
    }
    else
    {
        cout << "Decryption failed!" << endl;
    }

    EVP_CIPHER_CTX_free(decryptCtx);

    return 0;
}