//
// Created by Shijie on 2021/12/3.
//

/******************************************************************************/
/* Usual suspects to include  */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define BIT type as a char (i.e., one byte)
typedef char BIT;
#define TRUE 1
#define FALSE 0
#define UNDEF -1

/******************************************************************************/
/* Program state - memory spaces, PC, and control */
/******************************************************************************/
BIT PC[32]                  = {FALSE};
BIT MEM_Instruction[32][32] = {FALSE};
BIT MEM_Data[32][32]        = {FALSE};
BIT MEM_Register[32][32]    = {FALSE};

BIT RegDst[2]     = {FALSE};
BIT ALUSrc        =  FALSE ;
BIT MemToReg[2]   = {FALSE};
BIT RegWrite      =  FALSE ;
BIT MemRead       =  FALSE ;
BIT MemWrite      =  FALSE ;
BIT Branch        =  FALSE ;
BIT Jump          =  FALSE ; // for j, jal instruction
BIT JMPReg        =  FALSE ; // for jr instruction
BIT ALUOp[2]      = {FALSE};

BIT Zero          =  FALSE;
BIT ALUControl[4] = {FALSE};

BIT  not_gate(BIT A) {
    if (A)
        return FALSE;
    else
        return TRUE;
}

BIT  or_gate(BIT A, BIT B) {
    if (A || B)
        return TRUE;
    else
        return FALSE;
}

BIT  or_gate3(BIT A, BIT B, BIT C) {
    return or_gate(A, or_gate(B, C));
}

BIT  and_gate(BIT A, BIT B) {
    if (A && B)
        return TRUE;
    else
        return FALSE;
}

BIT  and_gate3(BIT A, BIT B, BIT C) {
    return and_gate(A, and_gate(B, C));
}

BIT  and_gate6(BIT A, BIT B, BIT C, BIT D, BIT E, BIT F) {
    return and_gate(and_gate3(A, B, C) , and_gate3(D, E, F) );
}

BIT  xor_gate(BIT A, BIT B) {
    if (A ^ B)
        return TRUE;
    else
        return FALSE;
}

BIT  nor_gate(BIT A, BIT B) {
    return not_gate(or_gate(A, B));
}

BIT  nand_gate(BIT A, BIT B) {
    return not_gate(and_gate(A, B));
}

void decoder2(BIT I0, BIT I1, BIT* O0, BIT* O1, BIT* O2, BIT* O3) {
    // Note: The input -> output mapping is slightly modified from before
    *O0 = and_gate(not_gate(I1), not_gate(I0));
    *O1 = and_gate(not_gate(I1), I0);
    *O2 = and_gate(I1, not_gate(I0));
    *O3 = and_gate(I1, I0);
}

void decoder3(BIT* I, BIT EN, BIT* O) {
    // TODO: implement 3-to-8 decoder using gates
    O[0] = and_gate3(not_gate(I[2]), not_gate(I[1]), not_gate(I[0]));
    O[1] = and_gate3(not_gate(I[2]), not_gate(I[1]), I[0]);
    O[2] = and_gate3(not_gate(I[2]), I[1], not_gate(I[0]));
    O[3] = and_gate3(not_gate(I[2]), I[1], I[0]);
    O[4] = and_gate3(I[2], not_gate(I[1]), not_gate(I[0]));
    O[5] = and_gate3(I[2], not_gate(I[1]), I[0]);
    O[6] = and_gate3(I[2], I[1], not_gate(I[0]));
    O[7] = and_gate3(I[2], I[1], I[0]);

    O[0] = and_gate(EN, O[0]);
    O[1] = and_gate(EN, O[1]);
    O[2] = and_gate(EN, O[2]);
    O[3] = and_gate(EN, O[3]);
    O[4] = and_gate(EN, O[4]);
    O[5] = and_gate(EN, O[5]);
    O[6] = and_gate(EN, O[6]);
    O[7] = and_gate(EN, O[7]);
}

void decoder5(BIT* I, BIT* O) {
    // TODO: implement 5-to-32 decoder using 2-to-4 and 3-to-8 decoders
    BIT EN[4] = {FALSE};
    decoder2(I[3], I[4], &EN[0], &EN[1], &EN[2], &EN[3]);
    decoder3(I, EN[3], &O[24]);
    decoder3(I, EN[2], &O[16]);
    decoder3(I, EN[1], &O[8]);
    decoder3(I, EN[0], &O[0]);
}

BIT  multiplexor2(BIT S, BIT I0, BIT I1) {
    BIT x0 = and_gate(not_gate(S), I0);
    BIT x1 = and_gate(S, I1);
    return or_gate(x0, x1);
}

void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output) {
    for (int i = 0; i < 32; ++i) {
        BIT x0 = and_gate(not_gate(S), I0[i]);
        BIT x1 = and_gate(S, I1[i]);
        Output[i] = or_gate(x0, x1);
    }
}

BIT  multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3) {
    BIT x0, x1, x2, x3 = FALSE;
    decoder2(S0, S1, &x0, &x1, &x2, &x3);

    BIT y0 = and_gate(x0, I0);
    BIT y1 = and_gate(x1, I1);
    BIT y2 = and_gate(x2, I2);
    BIT y3 = and_gate(x3, I3);

    BIT z0 = or_gate(y0, y1);
    BIT z1 = or_gate(y2, y3);

    return or_gate(z0, z1);
}

/******************************************************************************/
/* Helper functions */
/******************************************************************************/
void copy_bits(const BIT* A, BIT* B) {
    for (int i = 0; i < 32; ++i) {
        B[i] = A[i];
    }
}

void print_binary(const BIT* A) {
    for (int i = 31; i >= 0; --i) {
        printf("%d", A[i]);
    }
}

void convert_to_binary_char(int a, char* A, int length) {
    /* Use your implementation from Lab 6 */
    if (a >= 0) {
        for (int i = 0; i < length; ++i) {
            A[i] = (a % 2 == 1 ? '1' : '0');
            a /= 2;
        }
    }
    else {
        a += 1;
        for (int i = 0; i < length; ++i) {
            A[i] = (a % 2 == 0 ? '1' : '0');
            a /= 2;
        }
    }
}

void convert_to_binary(int a, BIT* A, int length) {
    /* Use your implementation from Lab 6 */
    convert_to_binary_char(a, A, 32);
}

int  binary_to_integer(const BIT* A) {
    unsigned a = 0;
    unsigned mult = 1;

    for (int i = 0; i < 32; ++i) {
        a += A[i] * mult;
        mult *= 2;
    }

    return (int) a;
}

int  binary_to_integer5(const BIT* A) {
    unsigned a = 0;
    unsigned mult = 1;

    for (int i = 0; i < 5; ++i) {
        a += A[i] * mult;
        mult *= 2;
    }

    return (int) a;
}

void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
                   BIT* ReadData1, BIT* ReadData2) {
    // TODO: Implement register read functionality
    // Input: two 5-bit register addresses
    // Output: the values of the specified registers in ReadData1 and ReadData2
    // Note: Implementation will be very similar to instruction memory circuit
    int register1_index = binary_to_integer5(ReadRegister1);
    int register2_index = binary_to_integer5(ReadRegister2);
    copy_bits(MEM_Register[register1_index], ReadData1);
    copy_bits(MEM_Register[register2_index], ReadData2);
}

void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData) {
    // TODO: Implement register write functionality
    // Input: one 5-bit register address, data to write, and control bit
    // Output: None, but will modify register file
    // Note: Implementation will again be similar to those above
    int write_register_index = binary_to_integer(WriteRegister);
    BIT write_data[32];
    multiplexor2_32(RegWrite, MEM_Register[write_register_index], WriteData, write_data);
    copy_bits(write_data, MEM_Register[write_register_index]);
}


int main() {
    BIT ReadRegister1[5] = {TRUE, FALSE, FALSE, FALSE, FALSE};
    BIT ReadRegister2[5] = {FALSE, TRUE, FALSE, FALSE, FALSE};
    BIT WriteRegister[5] = {TRUE, FALSE, FALSE, FALSE, FALSE};

    BIT ReadData1[32] = {FALSE};
    BIT ReadData2[32] = {FALSE};
    BIT WriteData[32] = {TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                     FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                     FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                     FALSE, FALSE, FALSE, FALSE, FALSE};


    Write_Register(TRUE, WriteRegister, WriteData);
    for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 32; ++j) {
            printf("%d", MEM_Register[i][j]);
        }
        printf("\n");
    }
    printf("\n\n\n\n\n\n\n\n\n\n");
    Read_Register(ReadRegister1, ReadRegister2, ReadData1, ReadData2);

    for (int i = 0; i < 32; ++i) {
        printf("%d", ReadData1[i]);
    }

    printf("\n");

    for (int i = 0; i < 32; ++i) {
        printf("%d", ReadData2[i]);
    }

    return 0;
}




