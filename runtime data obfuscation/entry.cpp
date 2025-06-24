#include "encrypted_block.h"
#include <iostream>


__declspec(noinline) void protected_example()
{
    // Secure version — stored encrypted, decrypted only at runtime
    encrypted_block<int> protected_value(1337);

    // Runtime comparison
    if (protected_value == 1337)
        std::cout << "Match (encrypted): " << protected_value.get_decrypted() << std::endl;

}

__declspec(noinline) void unprotected_example()
{
    int plain_value = 1337;

    // Use it somehow
    if (plain_value == 1337)
        std::cout << "Match: " << plain_value << std::endl;

}

int main()
{
    protected_example();
    unprotected_example();
    return 0;
}