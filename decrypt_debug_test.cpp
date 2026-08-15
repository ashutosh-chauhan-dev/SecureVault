#include <iostream>
#include <string>

#include "VaultManager.h"

using namespace std;

int main()
{
    VaultManager vault("vault.dat");

    if (!vault.unlockVault("Secure123"))
    {
        cout << "Unlock failed!" << endl;
        return 1;
    }

    const vector<Credential>& credentials =
        vault.getCredentials();

    cout << "Credential count: "
         << credentials.size()
         << endl;

    for (size_t i = 0; i < credentials.size(); i++)
    {
        cout << endl;

        cout << "Credential #" << i + 1 << endl;

        cout << "Website size: "
             << credentials[i].website.size()
             << endl;

        cout << "Website bytes:" << endl;

        for (unsigned char c :
             credentials[i].website)
        {
            cout << static_cast<int>(c)
                 << " ";
        }

        cout << endl;

        cout << "Username bytes:" << endl;

        for (unsigned char c :
             credentials[i].username)
        {
            cout << static_cast<int>(c)
                 << " ";
        }

        cout << endl;

        cout << "Password bytes:" << endl;

        for (unsigned char c :
             credentials[i].password)
        {
            cout << static_cast<int>(c)
                 << " ";
        }

        cout << endl;
    }

    return 0;
}