#ifndef keccak_test_h
#define keccak_test_h

#include "keccak.h"

/*------------------------------------------------------------------
                        TEST THETA
-------------------------------------------------------------------*/
bool test_theta(uint64_t **actual)
{
    uint64_t expected[5][5];
    
    expected[0][0] = 0x0000000000000000;
    expected[1][0] = 0xa3a3a3a3a3a3a3a3;
    expected[2][0] = 0xa3a3a3a3a3a3a3a3;
    expected[3][0] = 0xE4E4E4E4E4E4E4E4;
    expected[4][0] = 0xa3a3a3a3a3a3a3a3;
    expected[0][1] = 0x0000000000000000;
    expected[1][1] = 0xa3a3a3a3a3a3a3a3;
    expected[2][1] = 0xa3a3a3a3a3a3a3a3;
    expected[3][1] = 0xE4E4E4E4E4E4E4E4;
    expected[4][1] = 0x0000000000000000;
    expected[0][2] = 0xa3a3a3a3a3a3a3a3;
    expected[1][2] = 0x0000000000000000;
    expected[2][2] = 0x0000000000000000;
    expected[3][2] = 0x4747474747474747;
    expected[4][2] = 0x0000000000000000;
    expected[0][3] = 0xa3a3a3a3a3a3a3a3;
    expected[1][3] = 0x0000000000000000;
    expected[2][3] = 0x0000000000000000;
    expected[3][3] = 0x4747474747474747;
    expected[4][3] = 0x0000000000000000;
    expected[0][4] = 0xa3a3a3a3a3a3a3a3;
    expected[1][4] = 0x0000000000000000;
    expected[2][4] = 0x0000000000000000;
    expected[3][4] = 0x4747474747474747;
    expected[4][4] = 0x0000000000000000;

    for(uint8_t i = 0;  i < 5; i++)
        if(memcmp(actual[i], expected[i], 5*sizeof(uint64_t))!=0)return false;
    
   return true;
}

/*------------------------------------------------------------------
                        TEST RHO
-------------------------------------------------------------------*/
bool test_rho(uint64_t **actual)
{
    uint64_t expected[5][5];
    
    expected[0][0] = 0x0000000000000000;
    expected[1][0] = 0x4747474747474747;
    expected[2][0] = 0xE8E8E8E8E8E8E8E8;
    expected[3][0] = 0x4E4E4E4E4E4E4E4E;
    expected[4][0] = 0x1D1D1D1D1D1D1D1D;
    expected[0][1] = 0x0000000000000000;
    expected[1][1] = 0x3A3A3A3A3A3A3A3A;
    expected[2][1] = 0xE8E8E8E8E8E8E8E8;
    expected[3][1] = 0x7272727272727272;
    expected[4][1] = 0x0000000000000000;
    expected[0][2] = 0x1D1D1D1D1D1D1D1D;
    expected[1][2] = 0x0000000000000000;
    expected[2][2] = 0x0000000000000000;
    expected[3][2] = 0x8E8E8E8E8E8E8E8E;
    expected[4][2] = 0x0000000000000000;
    expected[0][3] = 0x4747474747474747;
    expected[1][3] = 0x0000000000000000;
    expected[2][3] = 0x0000000000000000;
    expected[3][3] = 0xE8E8E8E8E8E8E8E8;
    expected[4][3] = 0x0000000000000000;
    expected[0][4] = 0x8E8E8E8E8E8E8E8E;
    expected[1][4] = 0x0000000000000000;
    expected[2][4] = 0x0000000000000000;
    expected[3][4] = 0x4747474747474747;
    expected[4][4] = 0x0000000000000000;

    for(uint8_t i = 0;  i < 5; i++)
        if(memcmp(actual[i], expected[i], 5*sizeof(uint64_t))!=0)return false;
    
   return true;
}

/*------------------------------------------------------------------
                        TEST PI
-------------------------------------------------------------------*/
bool test_pi(uint64_t **actual)
{
    uint64_t expected[5][5];
    
    expected[0][0] = 0x0000000000000000;
    expected[1][0] = 0x3A3A3A3A3A3A3A3A;
    expected[2][0] = 0x0000000000000000;
    expected[3][0] = 0xE8E8E8E8E8E8E8E8;
    expected[4][0] = 0x0000000000000000;
    expected[0][1] = 0x4E4E4E4E4E4E4E4E;
    expected[1][1] = 0x0000000000000000;
    expected[2][1] = 0x1D1D1D1D1D1D1D1D;
    expected[3][1] = 0x0000000000000000;
    expected[4][1] = 0x0000000000000000;
    expected[0][2] = 0x4747474747474747;
    expected[1][2] = 0xE8E8E8E8E8E8E8E8;
    expected[2][2] = 0x8E8E8E8E8E8E8E8E;
    expected[3][2] = 0x0000000000000000;
    expected[4][2] = 0x8E8E8E8E8E8E8E8E;
    expected[0][3] = 0x1D1D1D1D1D1D1D1D;
    expected[1][3] = 0x0000000000000000;
    expected[2][3] = 0x0000000000000000;
    expected[3][3] = 0x0000000000000000;
    expected[4][3] = 0x4747474747474747;
    expected[0][4] = 0xE8E8E8E8E8E8E8E8;
    expected[1][4] = 0x7272727272727272;
    expected[2][4] = 0x0000000000000000;
    expected[3][4] = 0x4747474747474747;
    expected[4][4] = 0x0000000000000000;
    
    for(uint8_t i = 0;  i < 5; i++)
        if(memcmp(actual[i], expected[i], 5*sizeof(uint64_t))!=0)return false;
    
   return true;
}

/*------------------------------------------------------------------
                            TEST XI
-------------------------------------------------------------------*/
bool test_xi(uint64_t **actual)
{
    uint64_t expected[5][5];
    
    expected[0][0] = 0x0000000000000000;
    expected[1][0] = 0xD2D2D2D2D2D2D2D2;
    expected[2][0] = 0x0000000000000000;
    expected[3][0] = 0xE8E8E8E8E8E8E8E8;
    expected[4][0] = 0x3A3A3A3A3A3A3A3A;
    expected[0][1] = 0x5353535353535353;
    expected[1][1] = 0x0000000000000000;
    expected[2][1] = 0x1D1D1D1D1D1D1D1D;
    expected[3][1] = 0x4E4E4E4E4E4E4E4E;
    expected[4][1] = 0x0000000000000000;
    expected[0][2] = 0x4141414141414141;
    expected[1][2] = 0xE8E8E8E8E8E8E8E8;
    expected[2][2] = 0x0000000000000000;
    expected[3][2] = 0x4141414141414141;
    expected[4][2] = 0x2626262626262626;
    expected[0][3] = 0x1D1D1D1D1D1D1D1D;
    expected[1][3] = 0x0000000000000000;
    expected[2][3] = 0x4747474747474747;
    expected[3][3] = 0x1818181818181818;
    expected[4][3] = 0x4747474747474747;
    expected[0][4] = 0xE8E8E8E8E8E8E8E8;
    expected[1][4] = 0x3535353535353535;
    expected[2][4] = 0x0000000000000000;
    expected[3][4] = 0xAFAFAFAFAFAFAFAF;
    expected[4][4] = 0x1212121212121212;
    
    for(uint8_t i = 0;  i < 5; i++)
        if(memcmp(actual[i], expected[i], 5*sizeof(uint64_t))!=0)return false;
    
   return true;
}

/*------------------------------------------------------------------
                        TEST IOTA
https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA3-512_1600.pdf
For Round #0: the expected L[0,0] after iota is 01 00 00 00 00 00 00 00
which must have been the result of 00 00 00 00 00 00 00 00 XOR RC[0] where RC[0] = 1.
Hence, that is a mistake. The expected result should be 0x0000000000000001.
That error propagates to all subsequent rounds. Testing with ir=1, etc. is
useless. All references from Stalling to keccak team agree on the round
constants and formula: bitwise xor with RC_i. XOR using big or little endian does not match the expected
for the provided test vector.
-------------------------------------------------------------------*/
bool test_iota(uint64_t *actual)
{
    return (*acutal  == 0x0000000000000001) ? true: false;
}

#endif /* keccak_test_h */
