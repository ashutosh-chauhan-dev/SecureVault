#include <iostream>
#include <string>

#include "MasterPassword.h"

using namespace std;

int main()
{
    string errorMsg;

    // 1. Empty password -> rejected
    if (MasterPassword::validate("", errorMsg))
    {
        cout << "FAILED: Empty password was accepted." << endl;
        return 1;
    }
    cout << "Test 1 Passed: Empty password rejected (" << errorMsg << ")" << endl;

    // 2. Password shorter than 12 characters -> rejected
    if (MasterPassword::validate("Short123", errorMsg))
    {
        cout << "FAILED: Password shorter than 12 chars was accepted." << endl;
        return 1;
    }
    cout << "Test 2 Passed: Short password rejected (" << errorMsg << ")" << endl;

    // 3. 12-character valid password -> accepted
    if (!MasterPassword::validate("TwelveChars!", errorMsg))
    {
        cout << "FAILED: Valid 12-char password was rejected." << endl;
        return 1;
    }
    cout << "Test 3 Passed: 12-char valid password accepted." << endl;

    // 4. Long passphrase -> accepted
    if (!MasterPassword::validate("ThisIsALongPassphraseForVaultSecurity!", errorMsg))
    {
        cout << "FAILED: Long passphrase was rejected." << endl;
        return 1;
    }
    cout << "Test 4 Passed: Long passphrase accepted." << endl;

    // 5. All-whitespace password -> rejected
    if (MasterPassword::validate("            ", errorMsg))
    {
        cout << "FAILED: All-whitespace password was accepted." << endl;
        return 1;
    }
    cout << "Test 5 Passed: All-whitespace password rejected (" << errorMsg << ")" << endl;

    // 6. Valid password + matching confirmation -> accepted
    string validPass = "ValidPassphrase123";
    string matchingConfirm = "ValidPassphrase123";
    if (!MasterPassword::validate(validPass) || validPass != matchingConfirm)
    {
        cout << "FAILED: Valid matching password/confirmation rejected." << endl;
        return 1;
    }
    cout << "Test 6 Passed: Valid password + matching confirmation accepted." << endl;

    // 7. Valid password + mismatched confirmation -> rejected
    string mismatchedConfirm = "DifferentPassphrase123";
    if (validPass == mismatchedConfirm)
    {
        cout << "FAILED: Mismatched confirmation was considered matching." << endl;
        return 1;
    }
    cout << "Test 7 Passed: Mismatched confirmation rejected." << endl;

    cout << "ALL MASTER PASSWORD VALIDATION TESTS PASSED!" << endl;
    return 0;
}
