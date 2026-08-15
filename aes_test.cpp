#include <iostream>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

int main()
{
    string plaintext = "GitHub|student@example.com|FakePassword123";

    // Generate a random 256-bit encryption key
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

    // Create encryption context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (ctx == nullptr)
    {
        cout << "Failed to create encryption context." << endl;
        return 1;
    }

    // Initialize AES-256-GCM
    if (EVP_EncryptInit_ex(
            ctx,
            EVP_aes_256_gcm(),
            nullptr,
            key,
            iv) != 1)
    {
        cout << "Encryption initialization failed." << endl;
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }

    unsigned char ciphertext[1024];
    int ciphertextLength = 0;
    int finalLength = 0;

    // Encrypt the plaintext
    if (EVP_EncryptUpdate(
            ctx,
            ciphertext,
            &ciphertextLength,
            reinterpret_cast<const unsigned char*>(plaintext.c_str()),
            plaintext.length()) != 1)
    {
        cout << "Encryption failed." << endl;
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }

    // Finish encryption
    if (EVP_EncryptFinal_ex(
            ctx,
            ciphertext + ciphertextLength,
            &finalLength) != 1)
    {
        cout << "Final encryption step failed." << endl;
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }

    ciphertextLength += finalLength;

    // Get authentication tag
    unsigned char tag[16];

    if (EVP_CIPHER_CTX_ctrl(
            ctx,
            EVP_CTRL_GCM_GET_TAG,
            sizeof(tag),
            tag) != 1)
    {
        cout << "Failed to get authentication tag." << endl;
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }

    EVP_CIPHER_CTX_free(ctx);

    cout << "AES-256-GCM encryption successful!" << endl;
    cout << "Plaintext: " << plaintext << endl;
    cout << "Encrypted data generated successfully." << endl;

    return 0;
}