/*
 * Simple driver to test a few features of the HugeInt class.
 * 
 * Improved version.
 * 
 */

#include <iostream>
#include <iomanip>
#include <limits>
#include "HugeInt.h"

iota::HugeInt read_bounded_hugeint(const iota::HugeInt&, const iota::HugeInt&);
iota::HugeInt factorial_recursive(const iota::HugeInt&);
iota::HugeInt factorial_iterative(const iota::HugeInt&);
iota::HugeInt fibonacci_recursive(const iota::HugeInt&);
iota::HugeInt fibonacci_iterative(const iota::HugeInt&);
void preamble();

// limits to avoid overflow
const iota::HugeInt FACTORIAL_LIMIT{1100LL};
const iota::HugeInt FIBONACCI_LIMIT{13000LL};

int main() {
 
    preamble(); // blah
    
    iota::HugeInt nfac = read_bounded_hugeint(0LL, FACTORIAL_LIMIT);
    
    iota::HugeInt factorial = factorial_iterative(nfac);
    long double factorial_dec = static_cast<long double>(factorial);
    
    std::cout << "\nThe value of " << nfac << "! is:\n";
    std::cout << factorial << '\n';
    std::cout << "\nThis value has " << factorial.numDecimalDigits() 
              << " decimal digits.\n";
    std::cout.precision(std::numeric_limits<long double>::digits10);
    std::cout << "\nIts decimal approximation is: " << factorial_dec << "\n\n";

    
    iota::HugeInt nfib = read_bounded_hugeint(0LL, FIBONACCI_LIMIT);
 
    iota::HugeInt fibonacci = fibonacci_iterative(nfib);
    long double fibonacci_dec = static_cast<long double>(fibonacci);
    
    std::cout << "\nThe " << nfib << "th Fibonacci number is:\n";
    std::cout << fibonacci << '\n';
    std::cout << "\nThis value has " << fibonacci.numDecimalDigits() 
              << " decimal digits.\n";
    std::cout << "\nIts decimal approximation is: " << fibonacci_dec << '\n';

    std::cout << "\nCOMPARING these two values we observe that ";
    if (factorial == fibonacci) {
        std::cout << nfac << "! == Fibonacci_{" << nfib << "}\n";
    }
    
    if (factorial < fibonacci) {
        std::cout << nfac << "! < Fibonacci_{" << nfib << "}\n";
    }
    
    if (factorial > fibonacci) {
        std::cout << nfac << "! > Fibonacci_{" << nfib << "}\n";
    }
    
    iota::HugeInt sum = factorial + fibonacci;
    iota::HugeInt diff = factorial - fibonacci;
    
    std::cout << "\nTheir SUM (factorial + fibonacci) is:\n";
    std::cout << sum << '\n';
    std::cout << "\n\twhich is approximately " << static_cast<long double>(sum);
    std::cout << '\n';
    
    std::cout << "\nTheir DIFFERENCE (factorial - fibonacci) is:\n";
    std::cout << diff << '\n';
    std::cout << "\n\twhich is approximately " << static_cast<long double>(diff);
    std::cout << '\n';
    
    iota::HugeInt quotient = factorial / fibonacci;
    iota::HugeInt remainder = factorial % fibonacci;
    
    std::cout << "\nTheir QUOTIENT (factorial / fibonacci) is:\n";
    std::cout << quotient << '\n';
    std::cout <<"\n\twhich is approximately " 
              << static_cast<long double>(quotient) << '\n';
    std::cout << "\n\twith REMAINDER:\n";
    std::cout << remainder << '\n';
    std::cout <<"\n\twhich is approximately " 
              << static_cast<long double>(remainder) << '\n';
    
    iota::HugeInt x{"-80538738812075974"};
    iota::HugeInt y{"80435758145817515"};
    iota::HugeInt z{"12602123297335631"};
    
    iota::HugeInt k = x*x*x + y*y*y + z*z*z;
    
    std::cout << "\nDid you know that, with:\n";
    std::cout << "\tx = " << x << '\n';
    std::cout << "\ty = " << y << '\n';
    std::cout << "\tz = " << z << '\n';
    std::cout << "\nx^3 + y^3 + z^3 = " << k << '\n';
    
    return 0;
}


/**
 * read_bounded_hugeint
 * 
 * Read and return a value in the range min <= value <= max.
 * Dies after 5 retries.
 * 
 * @param min
 * @param max
 * @return 
 */
iota::HugeInt read_bounded_hugeint(const iota::HugeInt& min, 
                                   const iota::HugeInt& max) {
    iota::HugeInt value;
    bool fail;
    int retries = 0;
    
    do {
        try {
            std::cout << "Enter an integer (" << min << " - " << max << "): "; 
            std::cin >> value;
            
            if (value < min || value > max) {
                fail = true;
                ++retries;
            }
            else {
                fail = false;
            }
        }
        catch (std::invalid_argument& error) {
            std::cout << "You entered an invalid HugeInt value.";
            std::cout << " Please use, e.g., [+/-]1234567876376763.\n";
            //std::cout << "Exception: " << error.what() << '\n';
            fail = true;
            ++retries;
        }  
    } while (fail && retries < 5);
    
    if (retries == 5) {
        std::cerr << "Giving up...\n";
        exit(EXIT_FAILURE);
    }
    
    return value;
}

/**
 * factorial_recursive:
 * 
 * Recursive factorial function using HugeInt. Not too slow.
 * 
 * @param n
 * @return 
 */

iota::HugeInt factorial_recursive(const iota::HugeInt& n) {
    const iota::HugeInt one{1LL};
    
    if (n <= one) {
        return one;
    } else {
        return n * factorial_recursive(n - one);
    }
}

iota::HugeInt factorial_iterative(const iota::HugeInt& n) {
    iota::HugeInt result{1LL};
    
    if (n == 0LL) {
        return result;
    }
    
    for (iota::HugeInt i = n; i >= 1; --i) {
        result *= i;
    }
    
    return result;
}

/**
 * fibonacci_recursive:
 * 
 * Recursively calculate the n'th Fibonacci number, where n>=0.
 * 
 * WARNING: S l o w . . .
 * 
 * @param n
 * @return 
 */
iota::HugeInt fibonacci_recursive(const iota::HugeInt& n) {
    const iota::HugeInt zero;
    const iota::HugeInt one{1LL};
    
    if ((n == zero) || (n == one)) {
        return n;
    } 
    else {
        return fibonacci_recursive(n - 1LL) + fibonacci_recursive(n - 2LL);
    }  
}

iota::HugeInt fibonacci_iterative(const iota::HugeInt& n) {
    const iota::HugeInt zero;
    const iota::HugeInt one{1LL};
    
    if ((n == zero) || (n == one)) {
        return n;
    }
    
    iota::HugeInt retval;
    iota::HugeInt fib_nm1 = one;
    iota::HugeInt fib_nm2 = zero;
    
    for (iota::HugeInt i = 2; i <= n; ++i) {
        retval = fib_nm1 + fib_nm2;
        fib_nm2 = fib_nm1;
        fib_nm1 = retval;
    }
    
    return retval;
}

void preamble() {
    long double min = static_cast<long double>(iota::HugeInt::getMinimum());
    long double max = static_cast<long double>(iota::HugeInt::getMaximum());
    
    std::cout.precision(std::numeric_limits<long double>::digits10);
    std::cout <<"**************************************************************"
              <<"*************\n\n";
    std::cout << "The range of integers, x, that can be represented in " 
              << "the default HugeInt\nconfiguration is, approximately\n"
              << "      " << min << " <= x <= " << max << '\n';
    
    std::cout << "\nThe precise values of the upper and lower limits can be "
              << "found using\nHugeInt::getMinimum()/HugeInt::getMaximum().\n";
   
    std::cout << "\nThe maximum number of decimal digits of an integer "
              << "representable with\na HugeInt is: " 
              << iota::HugeInt::getMaximum().numDecimalDigits()
              << "\n\n";   
    std::cout <<"**************************************************************"
              <<"*************\n\n";
}