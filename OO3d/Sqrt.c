/* fsqrt.c
 *
 * A fast square root program adapted from the code of
 * Paul Lalonde and Robert Dawson in Graphics Gems I.
 * The format of IEEE double precision floating point numbers is:
 *
 * SEEEEEEEEEEEMMMM MMMMMMMMMMMMMMMM MMMMMMMMMMMMMMMM MMMMMMMMMMMMMMMM
 *
 * S = Sign bit for whole number
 * E = Exponent bit (exponent in excess 1023 form)
 * M = Mantissa bit
 */

#include <stdio.h>
#include <math.h>


// OOV_MOST_SIG_OFFSET gives the (int *) offset from the address of the double
// to the part of the number containing the sign and exponent.
// You will need to find the relevant offset for your architecture.
#define OOV_MOST_SIG_OFFSET 1

// OOV_SQRT_TAB_SIZE - the size of the lookup table - must be a power of four.
#define OOV_SQRT_TAB_SIZE 16384

// OOV_MANT_SHIFTS is the number of shifts to move mantissa into position.
// If you quadruple the table size subtract two from this constant,
// if you quarter the table size then add two.
// Valid values are: (16384, 7) (4096, 9) (1024, 11) (256, 13)
#define OOV_MANT_SHIFTS   7

#define OOV_EXP_BIAS   1023       /* Exponents are always positive     */
#define OOV_EXP_SHIFTS 20         /* Shifs exponent to least sig. bits */
#define OOV_EXP_LSB    0x00100000 /* 1 << OOV_EXP_SHIFTS                   */
#define OOV_MANT_MASK  0x000FFFFF /* Mask to extract mantissa          */

static int oomath_sqrt_tab[OOV_SQRT_TAB_SIZE];

void OOMath_SqrtInit()
{
    int           i;
    double        f;
    unsigned int  *fi = (unsigned int *) &f + OOV_MOST_SIG_OFFSET;
    
    for (i = 0; i < OOV_SQRT_TAB_SIZE/2; i++)
		{
        f = 0; /* Clears least sig part */
        *fi = (i << OOV_MANT_SHIFTS) | (OOV_EXP_BIAS << OOV_EXP_SHIFTS);
        f = sqrt(f);
        oomath_sqrt_tab[i] = *fi & OOV_MANT_MASK;

        f = 0; /* Clears least sig part */
        *fi = (i << OOV_MANT_SHIFTS) | ((OOV_EXP_BIAS + 1) << OOV_EXP_SHIFTS);
        f = sqrt(f);
        oomath_sqrt_tab[i + OOV_SQRT_TAB_SIZE/2] = *fi & OOV_MANT_MASK;
		}
}

double OOMath_Sqrt( double f )
{
    unsigned int e;
    unsigned int *fi = (unsigned int *) &f + OOV_MOST_SIG_OFFSET;

    if (f == 0.0) return(0.0);
    e = (*fi >> OOV_EXP_SHIFTS) - OOV_EXP_BIAS;
    *fi &= OOV_MANT_MASK;
    if (e & 1)
            *fi |= OOV_EXP_LSB;
    e >>= 1;
    *fi = (oomath_sqrt_tab[*fi >> OOV_MANT_SHIFTS]) |
          ((e + OOV_EXP_BIAS) << OOV_EXP_SHIFTS);
    return f;
}
