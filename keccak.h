#ifndef keccak_h
#define keccak_h
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>


/*

C implementation KECCAK-f for 1600 bit blocks, using function to process binary message interpretation in code per binary digit is 
unsigned char (8-bit data type is the smallest we can work with in C)
    
Is this in bytes or binary? all high level code is translated into assmebly and then machine code or directly into machine code.
machine code is binary and that is what the hardware works with. besides, full blown parallelism that hardware always exhibits,
and why we say crypto or any algorithm is always faster in hardware, it is understood regardless of how we represent the data
at high level (hex or binary or chars), we are working with bits... whose underlying representation is binary.

*/

#define INPUT_MSG "/Users/samiracarolinaolivamadrigal/Library/Autosave Information/crypto/crypto/msg.binary"
#define Nr 24    //Number of Rounds is the same for all Message Digest Size
#define WORD_SIZE 64
#define A_LEN 25

uint16_t b = 1600; //block length based on default paramters b = r + c with c = 1024 and bitrate r = 576.
uint16_t c = 1024; //capacity
uint16_t r = 576;  //bitrate
uint16_t l = 6;  

static const uint8_t a_len = 64; //word size
//https://keccak.team/keccak_specs_summary.html#roundConstants
static const uint64_t RC[24] = {0x0000000000000001, 0x0000000000008082,
0x800000000000808A, 0x8000000080008000, 0x000000000000808B, 0x0000000080000001,
0x8000000080008081, 0x8000000000008009, 0x000000000000008A, 0x0000000000000088,
0x0000000080008009, 0x000000008000000A, 0x000000008000808B, 0x800000000000008B,
0x8000000000008089, 0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
0x000000000000800A, 0x800000008000000A, 0x8000000080008081, 0x8000000000008080,
0x0000000080000001, 0x8000000080008008};

/*------------------------------------------------------------------
            print current state -- print all the lanes
-------------------------------------------------------------------*/
void print_cs(uint64_t **A)
{
    puts("\n");
    for(uint8_t i = 0;  i < 5; i++)
    {
        for(uint8_t j = 0;  j < 5; j++)
        {
            printf("%016llx\n", A[j][i]);//specify up tp 16 leading 0s... 64-bit.
        }
    }
    printf("\n");
}

/*------------------------------------------------------------------------
                        theta -- substitution
XORs each bit of state array with the parities of two columns in the array.
A[x][y][z] == (A[x][y] & (0x0000000000000001 << z))
 -------------------------------------------------------------------------*/
void theta(uint64_t **A)
{
    uint64_t C[5] = {0};
    uint64_t D[5] = {0}; //because in C... we don't have data type for indivudal bits... like wires...
    uint64_t bit;//must hold max shift out
    int8_t x, y, z;
    uint64_t mask1, mask63;//must use variable; literal with << is not computed correctly in this case.
    
    mask1 = 0x0000000000000001; mask63 = 0x8000000000000000;
    // for all pairs (x, z) such that 0≤x<5 and 0≤z<w, let
    for(x = 0; x < 5; x = x + 1)
    {
        for(z = 0; z < WORD_SIZE; z = z + 1)
        {
            bit = (A[x][0] & (mask1 << z)) ^ (A[x][1] & (mask1 << z)) ^ (A[x][2] & (mask1 << z)) ^ (A[x][3] & (mask1 << z)) ^ (A[x][4] & (mask1 << z));
            C[x] = bit ? C[x] | (mask1 << z): C[x] & ~(mask1 << z);
        }
    }
    //For all pairs (x, z) such that 0≤x<5 and 0≤z<w let
    for(x = 1; x < 5; x = x + 1)
    {
        for(z = 1; z < WORD_SIZE; z = z + 1)
        {
            bit = (C[(x-1)%5] & (mask1 << z)) ^ (C[(x+1)%5] & (mask1 << ((z-1)%WORD_SIZE)));
            D[x] = bit ? D[x] | (mask1 << z) : D[x] & ~(mask1 << z);
        }
    }
    
    //2 special cases for the modular arithmetic indexing
    //case: x= 0, so... -1 % 5 = -1+5 = 4...
    for(u_int8_t z = 1; z < WORD_SIZE; z = z + 1)
    {
        bit = (C[4] & (mask1 << z)) ^ (C[1] & (mask1 << ((z-1)%WORD_SIZE)));
        D[0] = bit ? D[0] | (mask1 << z) : D[0] & ~(mask1 << z);
    }
    
    //case z = 0
    for(x = 1; x < 5; x = x + 1)
    {
        bit = (C[(x-1)%5] & mask1) ^ (C[(x+1)%5] & mask63);
        D[x] = bit ? D[x] | mask1 : D[x] & ~(mask1);
        //bit = (C[4] & mask1) ^ (C[1] & mask63);
        //D[x] = bit ? D[x] | mask1 : D[x] & ~(mask1);
    }
    //case z=0 and x=0 ..how nice is verilog with bits...
    bit = (C[4] & mask1) ^ (C[1] & mask63);
    D[0] = bit ? D[0] | mask1 : D[0] & ~(mask1);
    
    //For all triples (x, y, z) such that 0≤x<5, 0≤y<5, and 0≤z<w, let
    for(x = 0; x < 5; x = x + 1)
    {
        for(y = 0; y < 5; y = y + 1)
        {
            for(z = 0; z < WORD_SIZE; z = z + 1)
            {
                bit = (A[x][y] & (mask1 << z)) ^ (D[x] & (mask1 << z));
                A[x][y] = bit ? A[x][y] | (mask1 << z): A[x][y] & ~(mask1 << z);

            }
        }
    }
}

/*------------------------------------------------------------------------
                             rho -- permutation
The effect of is to rotate the bits of each lane by a length, called
the offset, which depends on the fixed x and y coordinates of the lane.
 -------------------------------------------------------------------------*/
void rho(uint64_t **A)
{
    uint64_t offset[5][5];
    uint8_t x, y;
    offset[0][0] = 0;
    offset[0][1] = 36;
    offset[0][2] = 3;
    offset[0][3] = 105;
    offset[0][4] = 210;
    offset[1][0] = 1;
    offset[1][1] = 300;
    offset[1][2] = 10;
    offset[1][3] = 45;
    offset[1][4] = 66;
    offset[2][0] = 190;
    offset[2][1] = 6;
    offset[2][2] = 171;
    offset[2][3] = 15;
    offset[2][4] = 253;
    offset[3][0] = 28;
    offset[3][1] = 55;
    offset[3][2] = 153;
    offset[3][3] = 21;
    offset[3][4] = 120;
    offset[4][0] = 91;
    offset[4][1] = 276;
    offset[4][2] = 231;
    offset[4][3] = 136;
    offset[4][4] = 78;

    for(x = 0; x < 5; x = x + 1)
        for(y = 0; y < 5; y = y + 1)//circular rotate left by offset(x,y)
             A[x][y] = ((A[x][y] << offset[x][y]) | (A[x][y] >> (WORD_SIZE - offset[x][y])));

}

/*------------------------------------------------------------------------
                            pi -- permutation
                rearrange the positions of the lanes
 -------------------------------------------------------------------------*/
void pi(uint64_t **A)
{
    uint8_t x,y,z;
    uint64_t bit, mask1;
    uint64_t AP[5][5];
    
    for(x = 0; x < 5; x = x + 1)
        memcpy(AP[x], A[x], sizeof(uint64_t)*5);
    
    mask1 = 0x0000000000000001;
    for(z = 0; z < 64; z = z+1)
    {
        for(x = 0; x< 5; x = x+1)
        {
            for(y = 0; y<5; y = y+1)
            {
                bit = (AP[(x+(3*y))%5][x] & (mask1 << z));
                A[x][y] = bit ? A[x][y] | (mask1 << z) :  A[x][y] & ~(mask1 << z);
            }
            
        }
    }
}

/*------------------------------------------------------------------------
                        Reverse a string of Bytes
 -------------------------------------------------------------------------*/
void ReverseBinaryString(unsigned char *ary, int count)
{
    unsigned char *start, *end, temp;
    int c;
    
    c = (int)count/2;
    start = ary;
    end = (*(ary + (count - 1)) == '\n') ? ary + (count-2) : ary + (count-1);

    for(int i = 0; i < c; i++)
    {
        temp = *end;
        *end = *start;
        *start = temp;
        start++;
        end--;
    }
}

/*------------------------------------------------------------------------
                Get block count:     //k blocks of size r
 -------------------------------------------------------------------------*/
void GetBlockCount(int count, int *k, int *bits)
{
    int blocks;
    
    *k = count / r;  //get quotient
    if(count % r == 0)
        *k = *k + 1; //for uniformity
    else
    {
        for(blocks = (*k)*r; blocks <= count; blocks += r)
            (*k)++;
    }
    *bits = (*k)*r - count; //padding bits
}

/*------------------------------------------------------------------------
                         Simple Padding 1000.....0000
 -------------------------------------------------------------------------*/
void pad10star(unsigned char **buf, int count, int k, int bits)
{
    unsigned char *pad;
    
    *buf = realloc(*buf, k*r);
    pad = calloc(bits, sizeof(unsigned char));
    memset(pad,'0',bits*sizeof(unsigned char));
    pad[0] = '1';//set msb to 1. for example: [1]0000.
    memcpy(*buf + count, pad, bits*sizeof(unsigned char));
    free(pad);
}

/*------------------------------------------------------------------------
                      Miltirate Padding 1000...00001
 -------------------------------------------------------------------------*/
void pad10star1(unsigned char **buf, int count, int k, int bits)
{
    unsigned char *pad;
    
    *buf = realloc(*buf, k*r);
    pad = calloc(bits, sizeof(unsigned char));
    memset(pad,'0',bits*sizeof(unsigned char));
    pad[bits-1] = '1'; pad[0] = '1';//for example: [1]00...0000[1]
    memcpy(*buf + count, pad, bits*sizeof(unsigned char));
    free(pad);
}

/*------------------------------------------------------------------------
                        Process binary input message
 -------------------------------------------------------------------------*/
void PrintMsgStats(long count, size_t bytes, size_t words)
{
    printf("SIZE MAX %lu\n", SIZE_MAX);
    printf("FILE_SIZE %lu\n", count);
    printf("BYTES %lu\n", bytes);
    printf("WORDS %lu\n", words);
}

/*------------------------------------------------------------------------
                        Process binary input message
 -------------------------------------------------------------------------*/
unsigned char *ProcessBinaryMessage(char *file_name, long *count)
 {
    FILE *fp;
    size_t bytes, words;
    unsigned char *buf;
    
    fp = fopen(file_name, "rb");//1600 bit message
    
    fseek(fp, 0, SEEK_END);//fp points to end of file
    *count = ftell(fp) - 1;//count chars (bits) in file; discard \n
    rewind(fp); //go to start of file again
    bytes = *count / 8;
    words = bytes / 8; //each words is 8 Bytes = 64bits
    
    PrintMsgStats(*count, bytes, words);

    buf = calloc(*count, sizeof(unsigned char));//must process as array of bytes.. 1 char to represent 1 bit.
    fread(buf, sizeof(unsigned char), *count, fp);//read entire file
    fclose(fp);
    
    return buf;
}

/*------------------------------------------------------------------------
            Parition & Pad: P_k-1||0^c, P1||0^c, ..., P0||0^c -1600/64
 
P_0 ---> [........][.....] (block of len (r + c) bits == ary[25] of 64bit elements
 -------------------------------------------------------------------------*/
uint64_t **PartitionMessage(unsigned char *buf, int k)
{
    //binary string of len 1600 => uint64_t ary[25]; // we want an arry of these..
    //process 64 chars at a time, strqtol base 2, to a uint64_t
    uint64_t **P;
    unsigned char block[WORD_SIZE], *ptr;
    
    ptr = buf;
    P = (uint64_t **)calloc(k, sizeof(uint64_t*)); //k  blocks of size (r + c)
    for(int i = 0; i < k; i = i + 1)
    {
        P[i] = (uint64_t *)calloc(A_LEN, sizeof(uint64_t));
        for(int j = 0; j < 9; j++) //576/64 = 9, first 576 bits of PT are Msg data
        {
            memcpy(block, ptr, WORD_SIZE*sizeof(unsigned char));
            P[i][j] = strtoull(block, (char**)NULL, 2);
            ptr = ptr + WORD_SIZE;
        }
        for(int j = 9; j < A_LEN; j++) //the rest are 0's padding 0^c bits.
                P[i][j] = 0x0;
    }
    return P;
}

/*------------------------------------------------------------------------
                        Pi into A format 5 x 5 x 64
 -------------------------------------------------------------------------*/
uint64_t **PartitionToMatrix(uint64_t *Pi) //ith b-bit block
{
    uint64_t **Ap;
    int k;
    
    Ap = calloc(5, sizeof(uint64_t*));
    for(int i = 0; i < 5; i++)
        Ap[i] = calloc(5, sizeof(uint64_t));
    
    k = 0;
    for(int i = 0; i < 5; i = i + 1)//col major
    {
        for(int j = 0; j < 5; j++) //576/64 = 9, first 576 bits of PT are Msg data
        {
            Ap[j][i] = Pi[k]; k++;
        }
    }
    return Ap;
}

/*------------------------------------------------------------------------
                        xi -- substitution
The effect of χ is to XOR each bit with a non-linear function of two other
bits in its row.
 -------------------------------------------------------------------------*/
void xi(uint64_t **A)
{
    uint8_t x,y,z;//so used to thinking in bits and registers lol
    uint64_t bit, mask1;
    uint64_t AP[5][5];
    
    for(x = 0; x < 5; x = x + 1)//required aux matrix since will access
        memcpy(AP[x], A[x], sizeof(uint64_t)*5);//same coordinates that get overwritten

    mask1 = 0x0000000000000001;
    for(x = 0; x < 5; x = x + 1)
    {
        for(y = 0; y < 5; y = y + 1)
        {
            for(z = 0; z < WORD_SIZE; z = z + 1)
            {
                bit =  (AP[x][y] & (mask1 << z)) ^ ( (~(AP[(x+1)%5][y] & (mask1 << z))) & (AP[(x+2)%5][y] & (mask1 << z)));
                A[x][y] = bit ? A[x][y] | (mask1 << z) : A[x][y] & ~(mask1 << z);
            }
        }
    }
}

/*------------------------------------------------------------------------
                        iota -- substitution
The effect of ι is to modify some of the bits of Lane (0, 0) in a manner that
depends on the round index ir. The other 24 lanes are not affected by ι.
 -------------------------------------------------------------------------*/
void iota(uint64_t *Lane00, uint8_t ir)
{
    *Lane00 = *Lane00 ^ RC[ir];
}

/*------------------------------------------------------------------------
                                Rnd
                Rnd(A, ir) = ι(χ(π(ρ(θ(A)))), ir).
 -------------------------------------------------------------------------*/
 void Rnd(uint64_t **A, uint8_t ir)
 {
        theta(A);
        rho(A);
        pi(A);
        xi(A);
        iota(&A[0][0],ir);
}

/*------------------------------------------------------------------------
                 Initialize State Matrix
 -------------------------------------------------------------------------*/
uint64_t **InitializeState(void)
{
    uint64_t **A = (uint64_t**)calloc(5, sizeof(uint64_t*));
    for(int i = 0; i < 5; i++)
        A[i] = (uint64_t*)calloc(5, sizeof(uint64_t));
    return A;
}

/*------------------------------------------------------------------------
                    Set Parameters
 -------------------------------------------------------------------------*/
void SetParameters(uint16_t bitrate)
{
    switch (bitrate){
  case 1152:
    break;
  case 1088:
    break;
  case 832:
    break;
  case 576:
    r = bitrate;
    c = 1024;
    b = r + c;
    break;
  default:
    break;
    }
}

/*------------------------------------------------------------------------
                            KECCAK: SHA3-512
 -------------------------------------------------------------------------*/
void keccak_f(char *message_file,  uint16_t r, void (*pad)(unsigned char **buf, int count, int k, int bits))//uint8_t c, uint8_t r, uint8_t Nr, uint8_t pad_opt, uint8_t l)
{
    uint64_t **P, **A, **Pi;
    unsigned char *buf;
    long count;
    int k, bits;
    
    //A = InitializeState();
    //PREPROCESSING OF MESSAGE
    SetParameters(r);
    buf = ProcessBinaryMessage(message_file,&count);
    ReverseBinaryString(buf,(int)count);//we have to reverse the entire string 1st. -- only if needed by system... endianess.
    GetBlockCount((int)count, &k, &bits);
    pad(&buf,(int)count,k,bits); //pad10 or pad101 puts(buf);
    P = PartitionMessage(buf, k); //in a loop
    //for each Pi, process as a State array
    //Pi = PartitionToMatrix(P[0]); //ith b-bit block, for now, uint test, of f() with 1 block
    //print_cs(Pi);    
    
    //ABSORBTION PHASE -- Algorithm 7: KECCAK-p[b, nr](S)
    A = InitializeState();
    for(uint8_t i = 0; i < k; k++)//for each Pi, process as a State array
    {
        Pi = PartitionToMatrix(P[i]); //ith b-bit block, for now, uint test, of f() with 1 block
        _xor(A, Pi);
        for(uint8_t ir = 0; ir < Nr; ir++)
            Rnd(A,ir);
    }
    free(A);
    free(P);
    free(Pi);
    free(buf);
}


#endif /* keccak_h */
