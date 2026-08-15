#include <iostream>

#include "VaultManager.h"

using namespace std;

int main()
{
    const string vaultFile = "manager_test.dat";

    // --------------------------------
    // Create vault
    // --------------------------------

    VaultManager vault(vaultFile);

    if (!vault.createVault("Secure123"))
    {
        cout << "Vault creation failed!" << endl;
        return 1;
    }

    cout << "Vault created successfully!" << endl;

    // --------------------------------
    // Add credentials
    // --------------------------------

    Credential github;

    github.website = "Github";
    github.username = "github@example.com";
    github.password = "FakeGithub123";

    vault.addCredential(github);

    Credential gmail;

    gmail.website = "Gmail";
    gmail.username = "gmail@example.com";
    gmail.password = "FakeGmail456";

    vault.addCredential(gmail);

    cout << "Credentials added successfully!" << endl;

    // --------------------------------
    // Save
    // --------------------------------

    if (!vault.saveVault())
    {
        cout << "Vault save failed!" << endl;
        return 1;
    }

    cout << "Vault saved successfully!" << endl;

    // --------------------------------
    // Create a new VaultManager
    // --------------------------------

    VaultManager secondVault(vaultFile);

    // --------------------------------
    // Unlock
    // --------------------------------

    if (!secondVault.unlockVault("Secure123"))
    {
        cout << "Vault unlock failed!" << endl;
        return 1;
    }

    cout << "Vault unlocked successfully!" << endl;

    // --------------------------------
    // Display credentials
    // --------------------------------

    const vector<Credential>& saved =
        secondVault.getCredentials();

    cout << endl;
    cout << "===== Recovered Credentials ====="
         << endl;

    for (const Credential& credential : saved)
    {
        cout << "Website: "
             << credential.website
             << endl;

        cout << "Username: "
             << credential.username
             << endl;

        cout << "Password: "
             << credential.password
             << endl;

        cout << "----------------------"
             << endl;
    }

    // --------------------------------
    // Search
    // --------------------------------

    Credential result;

    if (secondVault.searchCredential(
            "Github",
            result))
    {
        cout << endl;
        cout << "Search test PASSED!" << endl;
    }
    else
    {
        cout << endl;
        cout << "Search test FAILED!" << endl;
    }

    // --------------------------------
    // Edit
    // --------------------------------

    if (secondVault.editCredential(
            "Github",
            "newgithubuser",
            "NewFakePassword123"))
    {
        cout << "Edit test PASSED!" << endl;
    }
    else
    {
        cout << "Edit test FAILED!" << endl;
    }

    // --------------------------------
    // Delete
    // --------------------------------

    if (secondVault.deleteCredential(
            "Gmail"))
    {
        cout << "Delete test PASSED!" << endl;
    }
    else
    {
        cout << "Delete test FAILED!" << endl;
    }

    // --------------------------------
    // Save final state
    // --------------------------------

    if (!secondVault.saveVault())
    {
        cout << "Final save failed!" << endl;
        return 1;
    }

    cout << "Final vault saved successfully!"
         << endl;

    return 0;
}