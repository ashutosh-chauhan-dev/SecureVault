#include <iostream>
#include <openssl/evp.h>

using namespace std;

int main()
{
    cout << "OpenSSL is connected successfully!" << endl;

    cout << "OpenSSL version: "
         << OpenSSL_version(OPENSSL_VERSION)
         << endl;

    return 0;
}