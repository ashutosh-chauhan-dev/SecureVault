# SecureVault

**SecureVault** is an offline C++ password manager that securely stores credentials in a local encrypted vault.

## Features

* Master password protection
* Password input masking
* Add, view, search, edit, and delete credentials
* AES-256-GCM authenticated encryption
* PBKDF2-HMAC-SHA256 key derivation
* Cryptographically secure random salt and IV generation
* Vault tamper detection
* Versioned binary vault format
* CMake + CTest automated testing
* No cloud services or internet connection required

## Security Architecture

```text
Master Password
       ↓
PBKDF2-HMAC-SHA256
100,000 iterations + 16-byte salt
       ↓
256-bit Encryption Key
       ↓
AES-256-GCM
12-byte IV + 16-byte authentication tag
       ↓
Encrypted vault.dat
```

### Cryptography

* **Encryption:** AES-256-GCM
* **Key Derivation:** PBKDF2-HMAC-SHA256
* **Key Size:** 256-bit
* **Salt:** 16 bytes
* **IV:** 12 bytes
* **Authentication Tag:** 16 bytes
* **Crypto Library:** OpenSSL 3.x

## Project Structure

```text
SecureVault/
├── CMakeLists.txt
├── CryptoManager.h/.cpp
├── KeyDerivation.h/.cpp
├── MasterPassword.h/.cpp
├── VaultStorage.h/.cpp
├── VaultManager.h/.cpp
├── main.cpp
└── *_test.cpp
```

## Requirements

* C++17
* GCC 16.2.0 / MinGW-w64
* OpenSSL 3.x
* CMake 3.16+
* CTest

## Build

### CMake

```bash
cmake -B build
cmake --build build
```

### Run

```powershell
.\build\SecureVault.exe
```

## Testing

Run all automated tests:

```bash
ctest --test-dir build --output-on-failure
```

**Current status: 9/9 tests passing.**

Tests cover encryption/decryption, key derivation, vault storage, CRUD operations, password validation, wrong-password rejection, and vault format validation.

## Security Limitations

SecureVault is an educational project and has **not undergone a formal security audit**.

It does not protect against:

* Malware or keyloggers on the host system
* RAM inspection while the vault is unlocked
* Physical access to an unlocked terminal

Credential strings in memory are not guaranteed to be securely zeroized.

Vault saving currently does not use atomic file replacement.

## Future Improvements

* Atomic vault file saving
* Secure memory handling for credentials
* Secure random password generator
* Import/export functionality
* Cross-platform GUI

## License

No license has been selected yet. Choose an appropriate license before public distribution.
