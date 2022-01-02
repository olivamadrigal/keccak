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

static uint8_t a_len = 64; //word size
uint16_t b = 1600; //block length based on default paramters b = r + c with c = 1024 and bitrate r = 576.
uint16_t c = 1024; //capacity
uint16_t r = 576;  //bitrate

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
    
    A = InitializeState();
    //PREPROCESSING OF MESSAGE
    SetParameters(r);
    buf = ProcessBinaryMessage(message_file,&count);
    ReverseBinaryString(buf,(int)count);//we have to reverse the entire string 1st. -- only if needed by system... endianess.
    GetBlockCount((int)count, &k, &bits);
    pad(&buf,(int)count,k,bits); //pad10 or pad101 puts(buf);
    P = PartitionMessage(buf, k); //in a loop
    //for each Pi, process as a State array
    Pi = PartitionToMatrix(P[0]); //ith b-bit block, for now, uint test, of f() with 1 block
    print_cs(Pi);    
    free(A);
    free(P);
    free(Pi);
    free(buf);
}


#endif /* keccak_h */