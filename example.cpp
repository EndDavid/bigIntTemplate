#include "bigInt.h"

#include <string>
#include <iostream>
#include <stdexcept>

int main() {
	try {
        BigInt a("123456789012345678901234567890");
        BigInt b("-98765432109876543210987654321");
        BigInt c = a + b;
        
        std::cout << a << " + " << b << " = " << c << std::endl;
        std::cout << a << " - " << b << " = " << a - b << std::endl;
        std::cout << a << " * " << b << " = " << a * b << std::endl;
        std::cout << a << " / " << BigInt(1000) << " = " << a / 1000 << std::endl;
        std::cout << a << " % " << BigInt(1000) << " = " << a % 1000 << std::endl;

        BigInt d = "114";
        std::cout << d << "! = " << BigInt::factorial(d) << std::endl;
		d = 514ll;
		std::cout << d << "! = " << BigInt::factorial(d) << std::endl;
		
        std::string s = a;
        BigInt e = s;
        std::cout << "e (from string) = " << e << std::endl;

        if (a > b) std::cout << a << " > " << b << std::endl;
        if (!(b >= a)) std::cout << b << " is not greater or equal to " << a << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
