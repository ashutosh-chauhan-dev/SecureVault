#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <conio.h>

#include "VaultManager.h"
#include "MasterPassword.h"

using namespace std;

void showMenu()
{
    cout << endl;
    cout << "===== SecureVault =====" << endl;
    cout << endl;

    cout << "1. Add Credential" << endl;
    cout << "2. View Credentials" << endl;
    cout << "3. Search Credential" << endl;
    cout << "4. Delete Credential" << endl;
    cout << "5. Edit Credential" << endl;
    cout << "6. Exit" << endl;

    cout << "Enter your choice: ";
}

string getHiddenPassword()
{
    string password;
    char ch;

    while ((ch = _getch()) != '\r')
    {
        if (ch == '\b')
        {
            if (!password.empty())
            {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else if (ch >= 32 && ch <= 126)
        {
            password += ch;
            cout << '*';
        }
    }

    cout << endl;

    return password;
}

void viewCredentials(const VaultManager& vault)
{
    const vector<Credential>& credentials = vault.getCredentials();

    cout << "\n===== Saved Credentials =====\n";
    cout << "Total credentials: " << credentials.size() << "\n";

    if (credentials.empty())
    {
        cout << "No credentials saved.\n";
        return;
    }

    for (size_t i = 0; i < credentials.size(); ++i)
    {
        cout << "\nCredential #" << (i + 1) << "\n";
        cout << "Website: " << credentials[i].website << "\n";
        cout << "Username: " << credentials[i].username << "\n";
        cout << "Password: ********" << endl;
        cout << "----------------------\n";
    }
}

bool vaultExists(const string& filename)
{
    ifstream file(filename);

    return file.good();
}



int main()
{
    const string vaultFile = "vault.dat";

    VaultManager vault(vaultFile);

    // ==========================================
    // FIRST RUN / EXISTING VAULT
    // ==========================================

    if (!vaultExists(vaultFile))
    {
        cout << "===== SecureVault =====" << endl;
        cout << endl;

        cout << "No vault found." << endl;

        string password;
        string confirmPassword;

        while (true)
        {
            cout << "Create a master password (min 12 characters): ";

            password = getHiddenPassword();

            string errorReason;

            if (!MasterPassword::validate(password, errorReason))
            {
                cout << errorReason << endl;
                cout << "Please try again." << endl << endl;

                continue;
            }

            cout << "Confirm master password: ";

            confirmPassword = getHiddenPassword();

            if (password != confirmPassword)
            {
                cout << "Passwords do not match! Please try again." << endl << endl;

                continue;
            }

            break;
        }

        if (!vault.createVault(password))
        {
            cout << endl;
            cout << "Failed to create vault!" << endl;

            return 1;
        }

        cout << endl;
        cout << "Vault created successfully!" << endl;
    }
    else
    {
        cout << "===== SecureVault =====" << endl;
        cout << endl;

        cout << "Enter master password: ";

        string password = getHiddenPassword();

        if (!vault.unlockVault(password))
        {
            cout << endl;
            cout << "Access denied!" << endl;

            return 1;
        }

        cout << endl;
        cout << "Access granted!" << endl;
    }

    // ==========================================
    // MAIN MENU
    // ==========================================

    while (true)
    {
        showMenu();

        int choice;

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');

            cout << "Invalid input. Please enter a number from 1 to 6."
                 << endl;

            continue;
        }

        // ======================================
        // ADD
        // ======================================

        if (choice == 1)
        {
            Credential credential;

            cout << "Enter website: ";
            getline(cin >> ws, credential.website);

            cout << "Enter username: ";
            getline(cin >> ws, credential.username);

            cout << "Enter password: ";
            credential.password = getHiddenPassword();

            if (credential.website.empty() ||
                credential.username.empty() ||
                credential.password.empty())
            {
                cout << "Credential fields cannot be empty."
                     << endl;

                continue;
            }

            Credential existing;

            if (vault.searchCredential(
                    credential.website,
                    existing))
                {
                    cout << "A credential for this website already exists."
                         << endl;

                    continue;
                }

            vault.addCredential(credential);

            if (vault.saveVault())
            {
                cout << "Credential added successfully!"
                     << endl;
            }
            else
            {
                cout << "Failed to save credential!"
                     << endl;
            }
        }

        // ======================================
        // VIEW
        // ======================================

        else if (choice == 2)
        {
            viewCredentials(vault);
        }

        // ======================================
        // SEARCH
        // ======================================

        else if (choice == 3)
        {
            string website;

            cout << "Enter website to search: ";

            cin >> website;

            Credential result;

            if (vault.searchCredential(
                    website,
                    result))
            {
                cout << endl;
                cout << "===== Credential Found ====="
                     << endl;

                cout << "Website: "
                     << result.website
                     << endl;

                cout << "Username: "
                     << result.username
                     << endl;

                cout << "Password: ********" << endl;
            }
            else
            {
                cout << "Credential not found."
                     << endl;
            }
        }

        // ======================================
        // DELETE
        // ======================================

        else if (choice == 4)
        {
            string website;

            cout << "Enter website to delete: ";

            cin >> website;

            if (vault.deleteCredential(website))
            {
                if (vault.saveVault())
                {
                    cout << "Credential deleted successfully!"
                         << endl;
                }
                else
                {
                    cout << "Credential deleted, "
                         << "but saving failed!"
                         << endl;
                }
            }
            else
            {
                cout << "Credential not found."
                     << endl;
            }
        }

        // ======================================
        // EDIT
        // ======================================

        else if (choice == 5)
        {
            string website;
            string newUsername;
            string newPassword;

            cout << "Enter website to edit: ";

            cin >> website;

            cout << "Enter new username: ";

            cin >> newUsername;

            cout << "Enter new password: ";

            newPassword = getHiddenPassword();

            if (vault.editCredential(
                    website,
                    newUsername,
                    newPassword))
            {
                if (vault.saveVault())
                {
                    cout << "Credential updated successfully!"
                         << endl;
                }
                else
                {
                    cout << "Credential updated, "
                         << "but saving failed!"
                         << endl;
                }
            }
            else
            {
                cout << "Credential not found."
                     << endl;
            }
        }

        // ======================================
        // EXIT
        // ======================================

        else if (choice == 6)
        {
            cout << "Goodbye!" << endl;

            break;
        }

        else
        {
            cout << "Invalid choice!"
                 << endl;
        }
    }

    return 0;
}