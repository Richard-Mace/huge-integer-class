/*
 * HugeInt.h
 * 
 * Definition of the huge integer class
 * Richard Mace, February, 2020
 * 
 * RADIX 2^32 VERSION
 *
 * Huge integers are represented as N-digit arrays of uint32_t types, where
 * each uint32_t value represents a base-2^32 digit. By default N = 300, which 
 * corresponds to a maximum of 2890 decimal digits. Each uint32_t contains 
 * a single base-2^32 digit in the range 0 <= digit <= 2^32 - 1. If `index' 
 * represents the index of the array of uint32_t digits[N], 
 * i.e., 0 <= index <= N - 1, and 'value' represents the power of 2^32 
 * corresponding to the radix 2^32 digit at 'index', then we have the following 
 * correspondence:
 *
 * index  |...... |     4    |     3    |     2    |     1    |     0    |
 * -----------------------------------------------------------------------
 * value  |...... | (2^32)^4 | (2^32)^3 | (2^32)^2 | (2^32)^1 | (2^32)^0 |
 *
 * The physical layout of the uint32_t array in memory is:
 *
 * uint32_t digits[N] = {digits[0], digits[1], digits[2], digits[3], ... }
 *
 * which means that the units (2^32)^0 appear first in memory, while the power 
 * (2^32)^(N-1) appears last. This LITTLE ENDIAN storage represents the 
 * number in memory in the REVERSE order of the way we write decimal numbers, 
 * but is convenient.
 *
 * Negative integers are represented by their radix complement. With the 
 * base 2^32 implementation here, we represent negative integers by their base 
 * 2^32 complement. With this convention the range of 
 * non-negative integers is:
 *                      0 <= x <= (2^32)^N/2 - 1
 * The range of base 2^32 integers CORRESPONDING to negative values in the
 * base 2^32 complement scheme is:
 *                      (2^32)^N/2 <= x <= (2^32)^N - 1 
 * So -1 corresponds to (2^32)^N - 1, -2 corresponds to (2^32)^N - 2, and so on.
 * 
 * The complete range of integers represented by a HugeInt using radix 
 * complement is:
 * 
 *                     -(2^32)^N/2 <= x <= (2^32)^N/2 - 1
 */


#ifndef HUGEINT_H
#define HUGEINT_H

#include <string>
#include <iosfwd>

namespace iota {

class HugeInt {
public:
    HugeInt() = default;
    HugeInt(long long int);  // conversion constructor from long long int
    explicit HugeInt(const char* const); // conversion constructor from C string
    HugeInt(const HugeInt&);    // copy/conversion constructor

    // assignment operator
    const HugeInt& operator=(const HugeInt&);

    // unary minus operator
    HugeInt operator-() const;
    
    // conversion to long double
    explicit operator long double() const;
    
    // basic arithmetic
    friend HugeInt operator+(const HugeInt&, const HugeInt&);
    friend HugeInt operator-(const HugeInt&, const HugeInt&);
    friend HugeInt operator*(const HugeInt&, const HugeInt&);
    friend HugeInt operator/(const HugeInt&, const HugeInt&);
    friend HugeInt operator%(const HugeInt&, const HugeInt&);


    // increment and decrement operators
    HugeInt& operator+=(const HugeInt&);
    HugeInt& operator-=(const HugeInt&);
    HugeInt& operator*=(const HugeInt&);
    HugeInt& operator/=(const HugeInt&);
    HugeInt& operator%=(const HugeInt&);
    HugeInt& operator++();     // prefix
    HugeInt  operator++(int);  // postfix
    HugeInt& operator--();     // prefix
    HugeInt  operator--(int);  // postfix

    // relational operators
    friend bool operator==(const HugeInt&, const HugeInt&);
    friend bool operator!=(const HugeInt&, const HugeInt&);
    friend bool operator<(const HugeInt&, const HugeInt&);
    friend bool operator>(const HugeInt&, const HugeInt&);
    friend bool operator<=(const HugeInt&, const HugeInt&);
    friend bool operator>=(const HugeInt&, const HugeInt&);
    
    // input/output 
    std::string toRawString() const;
    std::string toDecimalString() const;
    friend std::ostream& operator<<(std::ostream&, const HugeInt&);
    friend std::istream& operator>>(std::istream&, HugeInt&);
    
    // informational
    int numDecimalDigits() const;
    static HugeInt getMinimum();
    static HugeInt getMaximum();

private:
    static const std::size_t   numDigits_{300};   // max. no. base 2^32 digits
    static const std::uint64_t base_{1ULL << 32}; // 2^32, for convenience
    std::uint32_t              digits_[numDigits_]{0}; // base 2^32 digits

    // private utility functions
    bool          isZero() const;
    bool          isNegative() const;
    HugeInt&      radixComplement();  
    HugeInt       shortMultiply(std::uint32_t) const;
    HugeInt       shortDivide(std::uint32_t, std::uint32_t* const) const;
    friend HugeInt unsigned_divide(const HugeInt&, const HugeInt&, 
                                   HugeInt* const);
    HugeInt&      shiftLeftDigits(int);
};


} /* namespace iota */

#endif /* HUGEINT_H */

