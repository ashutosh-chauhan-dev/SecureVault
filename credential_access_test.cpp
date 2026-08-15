#include <iostream>
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

    cout << "Count: " << credentials.size() << endl;

    if (!credentials.empty())
    {
        cout << "Website: " << credentials[0].website << endl;
        cout << "Username: " << credentials[0].username << endl;
        cout << "Password: " << credentials[0].password << endl;
    }

    return 0;
}