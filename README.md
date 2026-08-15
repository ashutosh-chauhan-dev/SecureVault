# SecureVault

SecureVault is an offline C++ password manager designed to securely store user credentials in a local, encrypted vault file.

---

## 1. Project Overview

SecureVault provides a lightweight, command-line password manager for managing credentials locally. All sensitive credential data is encrypted at rest using industry-standard authenticated symmetric encryption and derived key material. The project requires no internet access or cloud services.

---

## 2. Features

* **Master Password Protection:** Vault access is guarded by a master password.
* **Master Password Validation:** New vaults enforce a minimum length policy (at least 12 characters, non-empty, non-whitespace) with password confirmation loops.
* **Console Password Input Masking:** All password inputs (master password login, new vault creation, credential addition, and credential editing) mask keystrokes with asterisks (`*`) in the console.
* **Credential Operations:** Full in-memory search, view, addition, edit, and deletion capabilities.
* **AES-256-GCM Authenticated Encryption:** Protects vault payloads against unauthorized decryption and tampering.
* **PBKDF2-HMAC-SHA256 Key Derivation:** Derives a 256-bit key from the master password and a cryptographically secure random salt.
* **Cryptographically Secure Randomness:** OpenSSL `RAND_bytes()` generates 128-bit salts and 96-bit nonces (IVs).
* **Integrity & Tamper Detection:** AES-GCM 128-bit authentication tags verify file integrity and reject modified vault files.
* **Wrong-Password Rejection:** Decryption and tag verification fail cleanly on invalid master passwords.
* **Versioned Binary Format:** Vault files store magic header bytes and a version identifier for schema compatibility.
* **Out-of-Source CMake Build System:** Supports building with CMake and executing automated tests using CTest.

---

## 3. Security Architecture

```text
Master Password
      ↓
PBKDF2-HMAC-SHA256 (100,000 iterations + 16-byte random salt)
      ↓
256-bit AES Encryption Key
      ↓
AES-256-GCM (12-byte random IV + 16-byte authentication tag)
      ↓
Encrypted Persistent Vault File (vault.dat)
```

### Component Roles:
* **Salt (16 bytes):** Prevents precomputed dictionary and rainbow table attacks. A new random salt is generated for every vault creation.
* **Key Derivation (PBKDF2):** Slows down brute-force and dictionary attacks by repeating HMAC-SHA256 hashing 100,000 times.
* **IV / Nonce (12 bytes):** Ensures every encryption operation produces a distinct ciphertext, even for identical plaintexts under the same key.
* **Authentication Tag (16 bytes):** Provides authenticated encryption with associated data (AEAD). Any bit flip or unauthorized modification to the ciphertext is caught during decryption.

---

## 4. Cryptographic Details

All cryptographic operations are implemented via the OpenSSL 3.x C library (`libcrypto`):

* **Symmetric Cipher:** AES-256-GCM (`EVP_aes_256_gcm()`)
* **Key Size:** 256 bits (32 bytes)
* **Initialization Vector (IV) Size:** 96 bits (12 bytes), generated via OpenSSL `RAND_bytes()`
* **Authentication Tag Size:** 128 bits (16 bytes), retrieved via `EVP_CTRL_GCM_GET_TAG` and verified via `EVP_CTRL_GCM_SET_TAG`
* **Key Derivation Function:** PBKDF2 with HMAC-SHA256 (`PKCS5_PBKDF2_HMAC` using `EVP_sha256()`)
* **PBKDF2 Iteration Count:** 100,000 iterations
* **Salt Size:** 128 bits (16 bytes), generated via OpenSSL `RAND_bytes()`

---

## 5. Vault Format

Vault files (`vault.dat`) are saved as binary structures containing header metadata followed by encrypted payload data.

### Binary Layout (`VaultStorage`):

| Offset / Field | Size | Type / Contents | Description |
| :--- | :--- | :--- | :--- |
| **Magic Header** | 8 bytes | `char[8]` (`SVLT001\0`) | Magic file identifier |
| **Version** | 4 bytes | `int32_t` (`1`) | Binary layout version number |
| **Salt** | 16 bytes | `unsigned char[16]` | PBKDF2 random salt |
| **Data Length** | 4 bytes | `int32_t` | Length of encrypted payload |
| **Encrypted Payload** | Variable | `IV` + `Tag` + `Ciphertext` | See payload layout below |

### Encrypted Payload Layout:
* **IV:** First 12 bytes
* **Authentication Tag:** Next 16 bytes
* **Ciphertext Data:** Remaining bytes

### Serialization Format (`VaultManager`):
Inside the decrypted payload, credentials are stored in `SV2` length-prefixed binary format:
1. Format Magic Header: `"SV2"` (3 bytes)
2. Credential Count: `uint32_t`
3. Credential Records: For each credential:
   - `website` length (`uint32_t`) + UTF-8 bytes
   - `username` length (`uint32_t`) + UTF-8 bytes
   - `password` length (`uint32_t`) + UTF-8 bytes

*Note: A legacy pipe-delimited format (`website|username|password`) is also supported for backward compatibility.*

---

## 6. Project Structure

```text
SecureVault/
├── CMakeLists.txt              # Root CMake build & CTest configuration
├── .gitignore                  # Git repository ignore rules
├── CryptoManager.h / .cpp      # OpenSSL AES-256-GCM encryption wrapper
├── KeyDerivation.h / .cpp      # PBKDF2-HMAC-SHA256 key derivation wrapper
├── MasterPassword.h / .cpp      # Salt generation & master password validation
├── VaultStorage.h / .cpp        # Binary file I/O and header validation
├── VaultManager.h / .cpp        # In-memory credential management & serialization
├── main.cpp                    # Interactive CLI application
└── *_test.cpp                  # Unit and integration test suites
```

---

## 7. Requirements

The project has been built and tested on the following toolchain:

* **Language Standard:** C++17
* **Compiler:** GCC 16.2.0 (MSYS2 MinGW-w64 on Windows)
* **Cryptographic Library:** OpenSSL 3.6.3 (OpenSSL 3.x)
* **Build System:** CMake 3.16+
* **Test Runner:** CTest

---

## 8. Build Instructions

### CMake Build (Recommended)

To configure and build out-of-source:

```powershell
# Configure out-of-source build
cmake -B build

# Compile all targets (executable + unit tests)
cmake --build build
```

### Manual Build

To compile manually with `g++`:

```powershell
g++ -std=c++17 main.cpp VaultManager.cpp VaultStorage.cpp CryptoManager.cpp MasterPassword.cpp KeyDerivation.cpp -lssl -lcrypto -o SecureVault.exe
```

---

## 9. Running SecureVault

Launch the compiled executable:

```powershell
# CMake build executable:
.\build\SecureVault.exe

# Manual build executable:
.\SecureVault.exe
```

### Application Options:
1. **Add Credential:** Save a new website, username, and password entry.
2. **View Credentials:** Display saved websites and usernames (passwords remain hidden as `********`).
3. **Search Credential:** Search for a credential by website domain/name.
4. **Delete Credential:** Remove a stored credential entry from the vault.
5. **Edit Credential:** Update username and password for an existing website record.
6. **Exit:** Save and close the application.

---

## 10. Testing

Automated testing is configured via CTest.

### Running CTest:
```powershell
ctest --test-dir build --output-on-failure
```

### Verified Test Status:
**100% tests passed (9 out of 9 tests passed)**

Included test suites:
1. `crypto_manager_test` — AES-256-GCM roundtrip encryption/decryption.
2. `vault_crypto_test` — End-to-end PBKDF2 + AES-GCM integration.
3. `vault_format_test` — Binary header format & storage validation.
4. `vault_manager_test` — Full CRUD lifecycle, serialization, and re-encryption.
5. `vault_wrong_password_test` — Invalid password rejection & failure handling.
6. `master_unlock_test` — Master key unlock & verification logic.
7. `master_password_test` — Salt generation & PBKDF2 key creation.
8. `master_password_validation_test` — Minimum length, non-empty, and non-whitespace rules.
9. `credential_access_test` — Post-unlock credential vector access.

---

## 11. Security Limitations

* **Memory Zeroization Limits:** `VaultManager` key and salt arrays are explicitly cleansed with `OPENSSL_cleanse()` upon object destruction. However, credential strings use standard C++ `std::string` objects (which are subject to Small String Optimization and standard allocator reallocations), so guaranteed memory zeroization of credential values in RAM is **not** claimed.
* **Direct File Overwrites:** Vault saving currently writes directly to `vault.dat` without an atomic temporary-file swap strategy.
* **Audit Notice:** This software is an educational project and has not undergone a formal third-party security audit.

---

## 12. Threat Model

### Protected Against:
* **Offline Storage Theft:** Attackers obtaining `vault.dat` without the master password cannot read credentials.
* **Accidental Plaintext Leaks:** Plaintext passwords are not saved to disk unencrypted.
* **Vault Data Tampering:** Modifications to `vault.dat` cause authentication tag validation to fail, preventing corruption from being loaded into memory.

### Not Protected Against:
* **Compromised Host Systems:** Keyloggers or malware running on the host OS while credentials are entered.
* **Process RAM Dump Analysis:** Inspection of running process memory while the vault is unlocked.
* **Physical Access to Unlocked Terminal:** Someone accessing an unattended unlocked terminal session.

---

## 13. Future Improvements

* Atomic vault file saving (writing to temporary file + atomic replace).
* Custom secure memory allocators for `std::string` credential fields.
* Built-in secure random password generator.
* Import/export vault functions.
* Cross-platform graphical user interface (GUI).

---

## 14. License

*Note: No license has been designated yet. Select an appropriate license (e.g., MIT or Apache 2.0) before public distribution.*
