# huge-integer-class
## Huge integer class using base 2<sup>32</sup>

Richard Mace, February, 2020
 
RADIX 2<sup>32</sup> VERSION

Huge integers are represented as fixed N-digit arrays of `uint32_t` types, where
each `uint32_t` value represents a base-2<sup>32</sup> digit. By default `N = 300`, which 
corresponds to a maximum of 2890 decimal digits. Each `uint32_t` contains 
a single base-2<sup>32</sup> digit in the range 0 <= digit <= 2<sup>32</sup> - 1. If 'index' 
represents the index of the array of `uint32_t` digits[N], 
i.e., 0 <= index <= N - 1, and 'value' represents the power of 2<sup>32</sup> 
corresponding to the radix 2<sup>32</sup> digit at 'index', then we have the following 
correspondence:

 index  |...... |     4    |     3    |     2    |     1    |     0    |
 -------|-------|----------|----------|----------|----------|----------|
 value  |...... | (2<sup>32</sup>)<sup>4</sup> | (2<sup>32</sup>)<sup>3</sup> | (2<sup>32</sup>)<sup>2</sup> | (2<sup>32</sup>)<sup>1</sup> | (2<sup>32</sup>)<sup>0</sup> |

The physical layout of the `uint32_t` array in memory is:

`uint32_t digits[N] = {digits[0], digits[1], digits[2], digits[3], ... }`

which means that the units (2<sup>32</sup>)<sup>0</sup> appear first in memory, while the power 
(2<sup>32</sup>)<sup>N-1</sup> appears last. This *little endian* storage represents the 
number in memory in the *reverse* order of the way we write decimal numbers, 
but is convenient.

Negative integers are represented by their *radix complement*. With the 
base 2<sup>32</sup> implementation here, we represent negative integers by their base 
2<sup>32</sup> complement. With this convention the range of 
non-negative integers is:
                      0 <= x <= (2<sup>32</sup>)<sup>N</sup>/2 - 1
The range of base 2<sup>32</sup> integers *corresponding* to negative values in the
base 2<sup>32</sup> complement scheme is:
                       (2<sup>32</sup>)<sup>N</sup>/2 <= x <= (2<sup>32</sup>)<sup>N</sup> - 1. 
So -1 corresponds to (2<sup>32</sup>)<sup>N</sup> - 1, -2 corresponds to (2<sup>32</sup>)<sup>N</sup> - 2, and so on.
  
The complete range of integers represented by a HugeInt using radix 
complement is:
               -(2<sup>32</sup>)<sup>N</sup>/2 <= x <= (2<sup>32</sup>)<sup>N</sup>/2 - 1.
