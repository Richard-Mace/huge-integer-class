/*
 * HugeInt.cpp
 *
 * Implementation of the HugeInt class. See comments in HugeInt.h for
 * details of representation, etc.
 *
 * Richard Mace, February, 2020
 *
 * RADIX 2^32 VERSION
 * 
 */

#include "HugeInt.h"
#include <cstdlib>   // for abs(), labs(), etc.
#include <iostream>
#include <iomanip>
#include <cstring>
#include <stdexcept>
#include <cmath>


/*
 * Non-member utility function (in anonymous namespace -- file scope only).
 * 
 */

namespace { /* anonymous namespace */
    
/*
 * Simple function to check for non-digit characters in a C string.
 * 
 * Returns true if string contains all digit characters; otherwise
 * false.
 * 
 */
 
inline bool is_all_digits(const char *const str) {
    if (std::strlen(str) == 0) {
        return false;
    }
    
    for (size_t i = 0; i < std::strlen(str); ++i) {
        if (std::isdigit(static_cast<unsigned char>(str[i])) == 0) {
            return false;
        } 
    }
        
    return true;
}
    
} /* anonymous namespace */



/*
 * Member functions in namespace iota
 *
 */

namespace iota {

/**
 * Constructor (conversion constructor)
 *
 * Construct a HugeInt from a long long int.
 *
 */ 

HugeInt::HugeInt(long long int x) {
    if (x == 0) {
        return;
    }
    
    long long int xp{std::llabs(x)};
    int           i{0};

    // Successively determine units, 2^32's, (2^32)^2's, (2^32)^3's, etc.
    // storing them in digits_[0], digits_[1], digits_[2], ...,
    // respectively. That is units = digits_[0], 2^32's = digits_[1], etc.
    while (xp > 0) {
        digits_[i++] = xp % base_;
        xp /= base_;
    }

    if (x < 0) {
        radixComplement();
    }
}

/**
 * Constructor (conversion constructor)
 *
 * Construct a HugeInt from a null-terminated C string representing the
 * base 10 representation of the number. The string is assumed to have 
 * the form "[+/-]31415926", including an optional '+' or '-' sign. 
 *
 * WARNING: No spaces are allowed in the decimal string containing numerals.
 * 
 * 
 * @param str
 */

HugeInt::HugeInt(const char *const str) {
    const std::size_t len{std::strlen(str)};

    if (len == 0) {
        throw std::invalid_argument{"empty decimal string in constructor."};
    }

    // Check for explicit positive and negative signs and adjust accordingly.
    // If negative, we flag the case and perform a radix complement at the end.
    bool        flagNegative{false};
    std::size_t numDecimalDigits{len};
    int         offset{0};
    
    if (str[0] == '+') {
        --numDecimalDigits;
        ++offset;
    } 
    
    if (str[0] == '-') {
        flagNegative = true;
        --numDecimalDigits;
        ++offset;
    }
    
    // validate the string of numerals
    if (!is_all_digits(str + offset)) {
        throw std::invalid_argument{"string contains non-digit in constructor."};
    }

    // Loop (backwards) through each decimal digit, digit[i], in the string, 
    // adding its numerical contribution, digit[i]*10^i, to theNumber. Here i 
    // runs upwards from zero, starting at the right-most digit of the string 
    // of decimal digits.
    HugeInt theNumber;       // zero by default
    HugeInt powerOfTen{1LL}; // initially 10^0 = 1
    
    for (size_t i = 0; i < numDecimalDigits; ++i) {
        std::uint32_t digitValue = 
            static_cast<std::uint32_t>(str[len - 1 - i]) - '0';
        theNumber += powerOfTen.shortMultiply(digitValue);
        powerOfTen = powerOfTen.shortMultiply(10);
    }

    if (flagNegative) {
        theNumber.radixComplement();
    }

    for (std::size_t i = 0; i < numDigits_; ++i) {
        digits_[i] = theNumber.digits_[i];
    }
}

/**
 * Copy constructor (could be defaulted)
 * 
 * @param rhs
 */

HugeInt::HugeInt(const HugeInt& other) {
    for (std::size_t i = 0; i < numDigits_; ++i)
        digits_[i] = other.digits_[i];
}

/**
 * Assignment operator
 * 
 * @param rhs
 * @return 
 */

const HugeInt& HugeInt::operator=(const HugeInt& rhs) {
    for (std::size_t i = 0; i < numDigits_; ++i) {
            digits_[i] = rhs.digits_[i]; 
    }
    
    return *this;
}

/**
 * Unary minus operator
 * 
 * @return 
 */

HugeInt HugeInt::operator-() const {
    HugeInt copy{*this};

    return copy.radixComplement();
}

/**
 * operator long double() 
 *
 * Use with static_cast<long double>(hugeint) to convert hugeint to its
 * approximate (long double) floating point value.
 * 
 * WARNING: Can overflow easily and will silently emit NaNs if the magnitude
 * of the HugeInt exceeds the limits that can be represented by a long double.
 * 
 */

HugeInt::operator long double() const {
    long double sign{1.0L};
    HugeInt     copy{*this};

    if (copy.isNegative()) {
        copy.radixComplement();
        sign = -1.0L;
    }
    
    long double retval{0.0L};
    long double pwrOfBase{1.0L}; // Base = 2^32; (2^32)^0 initially
    
    for (std::size_t i = 0; i < numDigits_; ++i) {
        retval += copy.digits_[i] * pwrOfBase;
        pwrOfBase *= base_;
    }

    return retval * sign;
}

/**
 * Operator +=
 *
 * NOTE: With the conversion constructors provided, also
 *       provides operator+=(long long int).
 * 
 * @param increment
 * @return 
 */

HugeInt& HugeInt::operator+=(const HugeInt& increment) {
    *this = *this + increment;
    return *this;
}

/**
 * Operator -=
 * 
 * NOTE: With the conversion constructors provided, also
 *       provides operator-=(long long int).
 * 
 * 
 * @param decrement
 * @return 
 */

HugeInt& HugeInt::operator-=(const HugeInt& decrement) {
    *this = *this - decrement;
    return *this;
}

/**
 * Operator *=
 * 
 * NOTE: With the conversion constructors provided, also
 *       provides operator*=(long long int).
 * 
 * @param multiplier
 * @return 
 */

HugeInt& HugeInt::operator*=(const HugeInt& multiplier) {
    *this = *this * multiplier;
    return *this;
}

/**
 * Operator /=
 * 
 * NOTE: With the conversion constructors provided, also
 *       provides operator/=(long long int).
 * 
 * @return 
 */

HugeInt& HugeInt::operator/=(const HugeInt& divisor) {
    *this = *this / divisor;
    return *this;
}

/**
 * Operator %=
 * 
 * See synopsis for operator % for the convention used for signed operands.
 * 
 * NOTE: With the conversion constructors provided, also
 *       provides operator/=(long long int).
 * 
 * @param divisor
 * @return 
 */
HugeInt& HugeInt::operator%=(const HugeInt& divisor) {
    *this = *this % divisor;
    return *this;
}

/**
 * Operator ++ (prefix)
 * 
 * @return 
 */

HugeInt& HugeInt::operator++() {
    *this = *this + 1LL;
    return *this;
}

/**
 * Operator ++ (postfix)
 * 
 * @param 
 * @return 
 */

HugeInt HugeInt::operator++(int) {
   HugeInt retval{*this};
   ++(*this);
   
   return retval;
}

/**
 * Operator -- (prefix)
 * 
 * @return 
 */

HugeInt& HugeInt::operator--() {
   *this = *this - 1LL;
   return *this;
}
 
/**
 * Operator -- (postfix)
 * 
 * @param 
 * @return 
 */

HugeInt HugeInt::operator--(int) {
   HugeInt retval{*this};
   --(*this);
   
   return retval;
}


////////////////////////////////////////////////////////////////////////////
// Convert to string                                                      //
////////////////////////////////////////////////////////////////////////////

/**
 * toRawString()
 * 
 * Format a HugeInt as string in raw internal format, i.e., as a sequence 
 * of base-2^32 digits (each in decimal form, 0 <= digit <= 2^32 - 1).
 *  
 * @return 
 */

std::string HugeInt::toRawString() const {
    int istart{numDigits_ - 1};

    for ( ; istart >= 0 && digits_[istart] == 0; --istart);
    
    std::ostringstream oss;
    
    if (istart == -1) // the number is zero
    {
        oss << digits_[0];
    } 
    else {
        for (int i = istart; i >= 0; --i) {
            oss << std::setw(10) << std::setfill('0') << digits_[i] << " ";
        }
    }

    return oss.str();
}

/**
 * toDecimalString()
 * 
 * @return 
 */

std::string HugeInt::toDecimalString() const {
    std::ostringstream oss;

    oss << *this;
    
    return oss.str();
}

/////////////////////////////////////////////////////////////////////////////
// Useful informational member functions                                   //
/////////////////////////////////////////////////////////////////////////////

/**
 * getMinimum()
 * 
 * Return the minimum representable value for a HugeInt. Static member
 * function.
 * 
 * @return 
 */

HugeInt HugeInt::getMinimum() {
    HugeInt retval;
    
    retval.digits_[numDigits_ - 1] = base_ / 2;
    
    return retval;
}

/**
 * getMaximum()
 * 
 * Return the maximum representable value for a HugeInt. Static member 
 * function.
 * 
 * @return 
 */

HugeInt HugeInt::getMaximum() {
    HugeInt retval;
    
    retval.digits_[numDigits_ - 1] = base_ / 2;
    
    --retval;
    
    return retval;
}

/**
 * numDecimalDigits()
 * 
 * Return the number of decimal digits this HugeInt has. 
 * 
 * We use a simple algorithm using base-10 logarithms. Consider, e.g., 457, 
 * which we can write as 4.57 * 10^2. Taking base-10 logs: 
 *         
 *          log10(4.57 * 10^2) = log10(4.57) + 2.
 * 
 * Since 0 = log10(1) < log10(4.57) < log10(10) = 1, we need to round up 
 * (always) to get the extra digit, corresponding to the fractional part in the 
 * equation above. Hence the use of ceil below. Values of x in the range 
 * -10 < x < 10 are dealt with as a special case.
 * 
 * @return 
 */

int HugeInt::numDecimalDigits() const {
    
    if (-10 < *this && *this < 10) {
        return 1;
    }
    else {
        long double approx = static_cast<long double>(*this);
        return static_cast<int>(std::ceil(std::log10(std::fabs(approx))));
    }
}

////////////////////////////////////////////////////////////////////////////
// friend functions                                                       //
////////////////////////////////////////////////////////////////////////////

/**
 * friend binary operator +
 *
 * Add two HugeInts a and b and return c = a + b.
 *
 * Note: since we provide a conversion constructor for long long int's, this 
 *       function, in effect, also provides the following functionality by 
 *       implicit conversion of long long int's to HugeInt
 *
 *       c = a + <some long long int>    e.g.  c = a + 2412356LL
 *       c = <some long long int> + a    e.g.  c = 2412356LL + a
 * 
 * @param a
 * @param b
 * @return 
 */

HugeInt operator+(const HugeInt& a, const HugeInt& b) {
    HugeInt sum;
    
    std::uint64_t partial{0};
    for (std::size_t i = 0; i < HugeInt::numDigits_; ++i) {
        partial += static_cast<std::uint64_t>(a.digits_[i]) 
                 + static_cast<std::uint64_t>(b.digits_[i]);

        sum.digits_[i] = static_cast<uint32_t>(partial);
        partial >>= 32;
    }

    return sum;
}

/**
 * friend binary operator-
 *
 * Subtract HugeInt a from HugeInt a and return the value c = a - b.
 *
 * Note: since we provide a conversion constructor for long long int's, this 
 *       function, in effect, also provides the following functionality by 
 *       implicit conversion of long long int's to HugeInt:
 *
 *       c = a - <some long long int>    e.g.  c = a - 2412356LL
 *       c = <some long long int> - a    e.g.  c = 2412356LL - a
 * 
 * @param a
 * @param b
 * @return 
 */

HugeInt operator-(const HugeInt& a, const HugeInt& b) {
    return a + (-b);
}

/**
 * friend binary operator *
 *
 * Multiply two HugeInt numbers. Uses standard long multipication algorithm
 * adapted to base 2^32. See comments on implicit conversion before 
 * HugeInt operator+(const HugeInt&, const HugeInt&) above, which are 
 * applicable here also.
 * 
 * @param a
 * @param b
 * @return 
 */

HugeInt operator*(const HugeInt& a, const HugeInt& b) {
    HugeInt product;
    HugeInt partial;

    for (std::size_t i = 0; i < HugeInt::numDigits_; ++i) {
        partial = a.shortMultiply(b.digits_[i]);
        product += partial.shiftLeftDigits(i);
    }

    return product;
}

/**
 * friend binary operator /
 * 
 * Return the quotient of two HugeInt numbers. Uses utility function 
 * unsigned_divide, which employs Donald Knuth's long division algorithm. See 
 * comments on implicit conversion before 
 * HugeInt operator+(const HugeInt&, const HugeInt&) above, which are 
 * applicable here also.
 * 
 * @param a
 * @param b
 * @return 
 */
HugeInt operator/(const HugeInt& a, const HugeInt& b) {    
    if (a < 0) {
        if (b < 0) {
            return unsigned_divide(-a, -b, nullptr);
        }
        else {
            return -unsigned_divide(-a, b, nullptr);
        }
    }
    else {
        if (b < 0) {
            return -unsigned_divide(a, -b, nullptr);
        }
        else {
            return unsigned_divide(a, b, nullptr);
        }
    }
}

/**
 * friend binary operator %
 * 
 * Return the remainder from the division of two HugeInt numbers. Uses utility 
 * function unsigned_divide. Adheres to the C/C++ convention that the sign of 
 * the remainder is the same as the sign of the dividend. See comments on 
 * implicit conversion before HugeInt operator+(const HugeInt&, const HugeInt&) 
 * above, which are applicable here also.
 * 
 * @param a
 * @param b
 * @return 
 */

HugeInt operator%(const HugeInt& a, const HugeInt& b) {
    HugeInt remainder;
       
    if (a < 0) {
        if (b < 0) {
            unsigned_divide(-a, -b, &remainder);
            return -remainder;
        }
        else {
            unsigned_divide(-a, b, &remainder);
            return -remainder;
        }
    }
    else {
        if (b < 0) {
            unsigned_divide(a, -b, &remainder);
            return remainder;
        }
        else {
            unsigned_divide(a, b, &remainder);
            return remainder;
        }
    }
}

/**
 * unsigned_divide: (private utility function)
 * 
 * Unsigned division of a by b giving quotient q = [a/b] and remainder r, such 
 * that 
 *                   a = q * b + r,    where 0 <= r < b.
 * 
 * Dividend a is assumed non-negative (a >= 0) and divisor b is positive 
 * definite (b > 0). If the number of base-2^32 digits in b is 1, then short 
 * division is used. Otherwise Donald Knuth's Algorithm D is used. 
 * 
 * The implementation of Knuth's algorithm here is very similar to that which 
 * appears in the book Hacker's Delight by Henry S. Warren, but borrows some 
 * ideas from janmr's blog entry (to which credit is duly given):
 *  
 *   https://janmr.com/blog/2014/04/basic-multiple-precision-long-division/
 * 
 * If remainder is not a nullptr, then the remainder r is returned in space 
 * allocated by the caller.
 * 
 * WARNING: no checks on the validity of a and b are made for performance 
 * reasons.
 * 
 * @param a
 * @param b
 * @return 
 */

HugeInt unsigned_divide(const HugeInt& a, const HugeInt& b, 
                        HugeInt* const remainder)
{   
    HugeInt dividend{a};
    HugeInt divisor{b};
    
    // Determine the number of base-2^32 digits in dividend and divisor.
    int n{HugeInt::numDigits_};
    for ( ; n > 0 && divisor.digits_[n - 1] == 0; --n);
    
    int m{HugeInt::numDigits_};
    for ( ; m > 0 && dividend.digits_[m - 1] == 0; --m);
    
    // Technically, m can equal 0 here, if 'a' (the dividend) = 0. This is no 
    // problem as it will be caught and handled by CASE 1 below.
   
    // CASE 1: m < n => quotient = 0; remainder = dividend.
    HugeInt quotient;
    
    if (m < n) {
        if (remainder != nullptr) {
            if (*remainder != 0) {
                *remainder == 0LL;
            }
    
            for (int i = 0; i < m; ++i) {
                remainder->digits_[i] = dividend.digits_[i];
            }
        }
        
        return quotient;
    }
    
    // CASE 2: Divisor has only one base-2^32 digit (n = 1). Do a short 
    //         division and return.
    if (n < 2) {
        std::uint64_t partial{0};
        
        for (int i = m - 1 ; i >= 0; --i) {
            partial = HugeInt::base_ * partial 
                    + static_cast<std::uint64_t>(dividend.digits_[i]);
            quotient.digits_[i] = 
                    static_cast<std::uint32_t>(partial / divisor.digits_[0]);
            partial %= divisor.digits_[0];
        } 
        
        if (remainder != nullptr) {
            if (*remainder != 0) {
                *remainder == 0LL;
            }
            
            remainder->digits_[0] = partial;
        }
        
        return quotient;
    }
    
    // CASE 3: m >= n and the number of digits, n, in the divisor is >= 2. 
    // Proceed with long division using Donald Knuth's Algorithm D.
    //
    // Determine power-of-two normalisation factor, d = 2^shifts, necessary for
    // d * divisor.digits[n-1] >= base_ / 2.
    int shifts{0};
    std::uint32_t vn{divisor.digits_[n - 1]};
    
    while (vn < (HugeInt::base_ >> 1)) {
        vn <<= 1;
        ++shifts;
    }
    
    // Scale the divisor and dividend by factor d, using shifts for efficiency. 
    // This scaling does not affect the quotient, but it ensures that
    // q_k <= qhat <= q_k + 2 (see later).
    for (int i = n - 1; i > 0; --i) {
        divisor.digits_[i] = (divisor.digits_[i] << shifts) | 
          (static_cast<std::uint64_t>(divisor.digits_[i - 1]) >> (32 - shifts));
    }
    divisor.digits_[0] = divisor.digits_[0] << shifts;
    
    // Prepend a (m+1)'th zero-value digit to the dividend, then shift.
    dividend.digits_[m] = 
        static_cast<std::uint64_t>(dividend.digits_[m - 1]) >> (32 - shifts);
    for (int i = m - 1; i > 0; --i) {
        dividend.digits_[i] = (dividend.digits_[i] << shifts) |
         (static_cast<std::uint64_t>(dividend.digits_[i - 1]) >> (32 - shifts));
    }
    dividend.digits_[0] = dividend.digits_[0] << shifts;
    
    // Do the long division using the primary school algorithm, estimating
    // partial quotients with a two most significant digit approximation for 
    // the dividend and a single most significant digit approximation for the 
    // divisor.
    for (int k = m - n; k >= 0; --k) {
        std::uint64_t rhat = dividend.digits_[k + n] * HugeInt::base_ 
            + static_cast<std::uint64_t>(dividend.digits_[k + n - 1]);
        
        std::uint64_t qhat = rhat / divisor.digits_[n - 1];
    
        rhat %= divisor.digits_[n - 1];
        
        // Digit q_k estimated by qhat must satisfy 0 <= q_k <= base_ - 1. 
        // If too large, decrement and adjust remainder rhat accordingly.
        if (qhat == HugeInt::base_) {
            qhat -= 1;
            rhat += divisor.digits_[n - 1];
        }
       
        // Compare with a "second order" approximation to the partial quotient. 
        // If this comparison indicates that qhat overestimates, decrement,
        // adjust remainder rhat and repeat.
        while (rhat < HugeInt::base_ && (qhat * divisor.digits_[n - 2]
                > HugeInt::base_ * rhat + dividend.digits_[k + n - 2])) {
            qhat -= 1;
            rhat += divisor.digits_[n - 1];
        }
        
        // We have an estimate qhat for the true digit q_k that satisfies
        // q_k <= qhat <= q_k + 1. Calculate the corresponding remainder 
        // (a_{k+n} ... a_{k}) - qhat * (b_{n-1}...b_{0}) for this partial
        // quotient, storing the result in digits a_{k+n}... a_{k} of the 
        // dividend. Care is taken with the carries. The overwritten digits 
        // accrue, and eventually become, the complete remainder.
    
        std::int64_t carry{0};     // signed; carry > 0, borrow < 0
        std::int64_t widedigit;    // signed
        for (int i = 0; i < n; ++i) {
            std::uint64_t product = static_cast<std::uint32_t>(qhat) 
                            * static_cast<std::uint64_t>(divisor.digits_[i]);
            
            widedigit = (dividend.digits_[k + i] + carry) 
                        - (product & 0xffffffffLL);
            
            dividend.digits_[k + i] = widedigit; // assigns 2^32-complement
                                                 // if widedigit < 0
            
            carry = (widedigit >> 32) - (product >> 32);
        }
        
        widedigit = dividend.digits_[k + n] + carry;
        dividend.digits_[k + n] = widedigit;           // 2^32-complement if
                                                       // widedigit < 0
        
        // Accept and store the tentative quotient digit.
        quotient.digits_[k] = qhat;
        
        // However, since q_k <= qhat <= q_k + 1, either we have the correct
        // digit, or we need to decrement. To resolve this, check if there was 
        // a borrow on determining the final k + n digit of the remainder. If 
        // no, we have q_k = qhat and we are done. Otherwise, qhat = q_k + 1, 
        // and we need to decrement and add the divisor to digits k + n ... k 
        // of the dividend (now the remainder).
        if (widedigit < 0) {
            quotient.digits_[k] -= 1;
            widedigit = 0;
            for (int i = 0; i < n; ++i) {
                widedigit += static_cast<std::uint64_t>(dividend.digits_[k + i])
                           + divisor.digits_[i];
                dividend.digits_[k + i] = widedigit;
                widedigit >>= 32;
            }
            
            dividend.digits_[k + n] += carry;
        }
    } /* end main loop over k */
    
    // We are done. Return the remainder?
    if (remainder != nullptr) {
        if (*remainder != 0) {
            *remainder == 0LL;
        }
    
        // Denormalise dividend, which now contains the full remainder 
        // (stored in n - 1 digits). 
        for (int i = 0; i < n - 1; ++i) {
            remainder->digits_[i] = (dividend.digits_[i] >> shifts) |
                    (static_cast<std::uint64_t>(dividend.digits_[i + 1]) 
                        << (32 - shifts));
        }
    
        remainder->digits_[n - 1] = dividend.digits_[n - 1] >> shifts;
    }
    
    return quotient; 
}

////////////////////////////////////////////////////////////////////////////
// Relational operators (friends)                                         //
////////////////////////////////////////////////////////////////////////////

bool operator==(const HugeInt& lhs, const HugeInt& rhs) {
   HugeInt diff{rhs - lhs};
   
   return diff.isZero();
}

bool operator!=(const HugeInt& lhs, const HugeInt& rhs) {
   return !(rhs == lhs);
}

bool operator<(const HugeInt& lhs, const HugeInt& rhs) {
   HugeInt diff{lhs - rhs};
   
   return diff.isNegative();
}

bool operator>(const HugeInt& lhs, const HugeInt& rhs) {
    return rhs < lhs;
}

bool operator<=(const HugeInt& lhs, const HugeInt& rhs) {
    return !(lhs > rhs);
}

bool operator>=(const HugeInt& lhs, const HugeInt& rhs) {
    return !(lhs < rhs);
}


////////////////////////////////////////////////////////////////////////////
// Private utility functions                                              //
////////////////////////////////////////////////////////////////////////////

/**
 * isZero()
 * 
 * Return true if the HugeInt is zero, otherwise false.
 * 
 * @return 
 */

bool HugeInt::isZero() const {
    int i{numDigits_};
    for ( ; i > 0 && digits_[i - 1] == 0; --i);
    
    return i == 0;
}

/**
 * isNegative()
 * 
 * Return true if a number x is negative (x < 0). If x >=0, then
 * return false.
 * 
 * NOTE: In the radix-2^32 complement convention, negative numbers, x, are 
 *       represented by the range of values: (2^32)^N/2 <= x <=(2^32)^N - 1.
 *       Since (2^32)^N/2 = (2^32/2)*(2^32)^(N-1) = 2147483648*(2^32)^(N-1), 
 *       we need only check whether the (N - 1)'th base 2^32 digit is at 
 *       least 2147483648. 
 * 
 * @return 
 */

bool HugeInt::isNegative() const {
    return digits_[numDigits_ - 1] >= base_ / 2;
}

/**
 * shortMultiply:
 * 
 * Return the result of a base 2^32 short multiplication by multiplier, where
 * 0 <= multiplier <= 2^32 - 1.
 *
 * WARNING: assumes both HugeInt and multiplier are POSITIVE.
 * 
 * @param multiplier
 * @return 
 */

HugeInt HugeInt::shortMultiply(std::uint32_t multiplier) const {
    HugeInt product;

    std::uint64_t partial{0};
    for (std::size_t i = 0; i < numDigits_; ++i) {
        partial += static_cast<std::uint64_t>(digits_[i]) * multiplier;
        product.digits_[i] = static_cast<uint32_t>(partial);
        partial >>= 32;
    }

    return product;
}

/**
 * shortDivide: 
 * 
 * Return the result of a base 2^32 short division by divisor, where 
 * 0 < divisor <= 2^32 - 1, using the usual primary school algorithm 
 * adapted to radix 2^32. If not a nullptr, the remainder is returned 
 * in space allocated by the caller. 
 *
 * WARNING: assumes both HugeInt and the divisor are POSITIVE.
 * 
 * @param divisor
 * @return 
 */

HugeInt HugeInt::shortDivide(std::uint32_t divisor, 
                             std::uint32_t* const remainder) const {
    HugeInt quotient;
    
    std::uint64_t partial{0};
    for (int i = numDigits_ - 1; i >= 0; --i) {
        partial = base_ * partial + static_cast<std::uint64_t>(digits_[i]);
        quotient.digits_[i] = static_cast<std::uint32_t>(partial / divisor);
        partial %= divisor;
    }

    if (remainder != nullptr) {
        *remainder = static_cast<uint32_t>(partial);
    }
    
    return quotient;
}


/**
 * shiftLeftDigits
 *
 * Shift this HugeInt's radix-2^32 digits left by num places, filling
 * with zeroes from the right.
 * 
 * @param num
 * @return 
 */

HugeInt& HugeInt::shiftLeftDigits(int num) {
    if (num == 0) {
        return *this;
    }

    for (int i = numDigits_ - num - 1; i >= 0; --i) {
        digits_[i + num] = digits_[i];
    }

    for (int i = 0; i < num; ++i) {
        digits_[i] = 0;
    }

    return *this;
}

/**
 * radixComplement()
 *
 * Perform a radix complement on the object in place (changes object).
 * 
 * @return 
 */

HugeInt& HugeInt::radixComplement() {
    if (!isZero()) {
        std::uint64_t sum{1};
        for (std::size_t i = 0; i < numDigits_; ++i) {
            sum += static_cast<std::uint64_t>(base_ - 1) 
                 - static_cast<std::uint64_t>(digits_[i]);
            digits_[i] = static_cast<uint32_t>(sum);
            sum >>= 32;
        }
    }

    return *this;
}

/**
 * operator<<
 * 
 * Overloaded stream insertion for HugeInt. Format HugeInt as a string of 
 * decimal digits. The HugeInt is processed in sets of 3 decimal digits at 
 * a time, i.e., in sets of thousands. To estimate the number of "triples" 
 * digits, we solve for x:
 *
 *  (2^32)^N = 1000^x = 10^(3x) ==> x = N log_10(2^32)/3 = N * 3.211 (approx)
 *
 * where N is the number of base 2^32 digits. A safety margin of 2 is added
 * for good measure.
 * 
 * @param output
 * @param x
 * @return 
 */

std::ostream& operator<<(std::ostream& output, const HugeInt& x) {
    // Special case HugeInt == 0 is easy
    if (x == 0) {
        output << "0";
        return output;
    }

    // Set copy to the absolute value of *this
    // for use in shortDivide.
    HugeInt copy;
    
    if (x < 0) {
        output << "-";
        copy = -x;
    } 
    else {
        copy = x;
    }

    // Determine the triples.
    int i{0};
    const int numTriples{static_cast<int>(HugeInt::numDigits_ * 3.211) + 2};
    std::uint32_t triples[numTriples]{0};
    
    while (copy != 0) {
        std::uint32_t remainder;
        copy = copy.shortDivide(1000, &remainder);
        triples[i++] = remainder;
    }

    // first set of thousands has no preceding zeros
    --i;
    output << triples[i];
    
    // all the other sets of thousands
    output << std::setfill('0');
    
    for (int j = i - 1; j >= 0; --j) {
        output << ',' << std::setw(3) << triples[j];
    }
    
    return output;
}

/**
 * operator >>
 * 
 * Overloaded stream extraction for HugeInt.
 * 
 * @param input
 * @param x
 * @return 
 */

std::istream& operator>>(std::istream& input, HugeInt& x) {
    std::string str;
    
    input >> str;
    x = HugeInt(str.c_str());
    
    return input;
}

} /* namespace iota */