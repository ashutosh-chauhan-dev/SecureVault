cmake_minimum_required(VERSION 3.16)

project(SecureVault VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Fallback OpenSSL search hint for Windows MSYS2 environment
if(WIN32 AND NOT DEFINED OPENSSL_ROOT_DIR AND EXISTS "C:/msys64/ucrt64")
    set(OPENSSL_ROOT_DIR "C:/msys64/ucrt64" CACHE PATH "Path to OpenSSL root directory")
endif()

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Core backend source files
set(CORE_SOURCES
    src/VaultManager.cpp
    src/VaultStorage.cpp
    src/CryptoManager.cpp
    src/MasterPassword.cpp
    src/KeyDerivation.cpp
)

# Main CLI Application Target
add_executable(SecureVault
    src/main.cpp
    ${CORE_SOURCES}
)

target_include_directories(SecureVault PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SecureVault PRIVATE
    OpenSSL::Crypto
    OpenSSL::SSL
)

# ==============================================================================
# CTest / Unified Test Configuration
# ==============================================================================

enable_testing()

macro(add_securevault_test test_name test_src)
    add_executable(${test_name}
        ${test_src}
        ${ARGN}
    )

    target_include_directories(${test_name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_link_libraries(${test_name} PRIVATE
        OpenSSL::Crypto
        OpenSSL::SSL
    )

    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

# ==============================================================================
# Project Test Targets
# ==============================================================================

add_securevault_test(
    crypto_manager_test
    tests/crypto_manager_test.cpp
    src/CryptoManager.cpp
)

add_securevault_test(
    vault_crypto_test
    tests/vault_crypto_test.cpp
    src/CryptoManager.cpp
    src/MasterPassword.cpp
    src/KeyDerivation.cpp
)

add_securevault_test(
    vault_format_test
    tests/vault_format_test.cpp
    src/CryptoManager.cpp
    src/KeyDerivation.cpp
    src/VaultStorage.cpp
)

add_securevault_test(
    vault_manager_test
    tests/vault_manager_test.cpp
    ${CORE_SOURCES}
)

add_securevault_test(
    vault_wrong_password_test
    tests/vault_wrong_password_test.cpp
    ${CORE_SOURCES}
)

add_securevault_test(
    master_unlock_test
    tests/master_unlock_test.cpp
    src/MasterPassword.cpp
    src/KeyDerivation.cpp
    src/CryptoManager.cpp
)

add_securevault_test(
    master_password_test
    tests/master_password_test.cpp
    src/MasterPassword.cpp
    src/KeyDerivation.cpp
)

add_securevault_test(
    master_password_validation_test
    tests/master_password_validation_test.cpp
    src/MasterPassword.cpp
    src/KeyDerivation.cpp
)

add_securevault_test(
    credential_access_test
    tests/credential_access_test.cpp
    ${CORE_SOURCES}
)