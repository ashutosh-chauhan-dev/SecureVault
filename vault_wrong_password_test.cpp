#include <iostream>

#include "VaultManager.h"

using namespace std;

int main()
{
    const string vaultFile = "manager_test.dat";

    VaultManager vault(vaultFile);

    // Try an intentionally incorrect password
    if (vault.unlockVault("WrongPassword123"))
    {
        cout << "ERROR: Wrong password was accepted!" << endl;
        return 1;
    }

    cout << "Wrong password rejected successfully!" << endl;

    return 0;
}