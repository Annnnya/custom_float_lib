#include <iostream>
#include "my_float.h"

int main() {
    // Test constructors and conversion operators
    my_float<16, 8> a(18.8);
    my_float<16, 8> b = 2.5;
    my_float<16, 8> c = 1.5_mf;

    std::cout << "a: " << a.to_double() << std::endl;  // Expected output: 3.140625
    std::cout << "b: " << b.to_double() << std::endl;  // Expected output: 2.5
    std::cout << "c: " << c.to_double() << std::endl;  // Expected output: 1.5

    std::cout << "sum: " << (a + b).to_double() << std::endl;       // Expected output: 5.640625
    std::cout << "diff: " << (a - b).to_double() << std::endl;     // Expected output: 0.640625
    std::cout << "prod: " << (a * b).to_double() << std::endl;     // Expected output: 7.875
    std::cout << "quotient: " << (a / b).to_double() << std::endl;  // Expected output: 1.25625


    // Test comparison operators
    bool less = a < b;
    bool greater = a > b;
    bool equal = a == b;
    bool notEqual = a != b;

    std::cout << "less a < b: " << less << std::endl;
    std::cout << "greater a > b: " << greater << std::endl;
    std::cout << "equal a == b: " << equal << std::endl;
    std::cout << "notEqual a != b: " << notEqual << std::endl;

    less = a < 10.0;
    greater = a > 1.0;
    equal = a == 18.8;
    notEqual = a != 0.0;

    std::cout << "less a < 10.0: " << less << std::endl;
    std::cout << "greater a > 1.0: " << greater << std::endl;
    std::cout << "equal a == 18.8: " << equal << std::endl;
    std::cout << "notEqual a != 0.0: " << notEqual << std::endl;

    my_float<16, 8> zero_float;
    my_float<16, 8> nonzero_float(10.0);

    std::cout << zero_float.to_double() << " is zero: " << zero_float.is_zero() << std::endl;         // Expected output: 1 (true)
    std::cout << nonzero_float.to_double() << " is zero: " << nonzero_float.is_zero() << std::endl;

    a+=10.0;
    std::cout << "a+=10.0: " << a.to_double() << std::endl;

    std::cout << "a to b powResult: " << a.pow(b).to_double() << std::endl;
    std::cout << "a sinResult: " << a.sin().to_double() << std::endl;
    std::cout << "a cosResult: " << a.cos().to_double() << std::endl;
    std::cout << "a tanResult: " << a.tan().to_double() << std::endl;

    return 0;
}
