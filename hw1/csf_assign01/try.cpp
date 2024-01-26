#include <iostream>
#include <cmath>
#include <cstdint>
int main() {
    
    std::cout << "Hexadecimal: " << std::hex << (uint64_t)(pow(2, 64) + 1) << std::endl;

    return 0;
}
