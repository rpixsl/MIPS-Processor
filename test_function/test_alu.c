#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define BIT type as a char (i.e., one byte)
typedef char BIT;
#define TRUE 1
#define FALSE 0
#define UNDEF -1

BIT ZERO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE};


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

BIT  or_gate6(BIT A, BIT B, BIT C, BIT D, BIT E, BIT F) {
    return or_gate(or_gate3(A, B, C), or_gate3(D, E, F));
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

BIT  and_gate4(BIT A, BIT B, BIT C, BIT D) {
    return and_gate3(A, B, and_gate(C, D));
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

void adder1(BIT A, BIT B, BIT CarryIn, BIT* CarryOut, BIT* Sum) {
    // TODO: implement a 1-bit adder
    // Note: you can probably copy+paste this from your (or my) Lab 5 solution

    BIT x0 = xor_gate(A, B);
    *Sum = xor_gate(CarryIn, x0);

    BIT y0 = and_gate(x0, CarryIn);
    BIT y1 = and_gate(A, B);
    *CarryOut = or_gate(y0, y1);
}

void adder32(BIT* A, BIT* B, BIT Binvert, BIT* Result) {
    BIT CarryOut = Binvert;
    for (int i = 0; i < 32; ++i) {
        adder1(A[i], xor_gate(B[i], Binvert), CarryOut,
               &CarryOut, &Result[i]);
    }
}

void and32(BIT* A, BIT* B, BIT* Result) {
    for (int i = 0; i < 32; ++i) {
        Result[i] = and_gate(A[i], B[i]);
    }
}

void or32(BIT* A, BIT* B, BIT* Result) {
    for (int i = 0; i < 32; ++i) {
        Result[i] = or_gate(A[i], B[i]);
    }
}

void slt32(BIT* A, BIT* B, BIT* Result) {
    for (int i = 0; i < 32; ++i) {
        Result[i] = FALSE;
    }
    BIT sub_result[32];
    adder32(A, B, TRUE, sub_result);
    Result[0] = sub_result[31];
}

void nor32(BIT* A, BIT* B, BIT* Result) {
    for (int i = 0; i < 32; ++i) {
        Result[i] = nor_gate(A[i], B[i]);
    }
}

void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result) {
    // TODO: Implement 32-bit ALU
    // Input: 4-bit ALUControl, two 32-bit inputs
    // Output: 32-bit result, and zero flag big
    // Note: Can re-use prior implementations (but need new circuitry for zero)
    BIT is_and = and_gate4(not_gate(ALUControl[3]),
                           not_gate(ALUControl[2]),
                           not_gate(ALUControl[1]),
                           not_gate(ALUControl[0]));
    BIT is_or  = and_gate4(not_gate(ALUControl[3]),
                           not_gate(ALUControl[2]),
                           not_gate(ALUControl[1]),
                           ALUControl[0]);
    BIT is_add = and_gate4(not_gate(ALUControl[3]),
                           not_gate(ALUControl[2]),
                           ALUControl[1],
                           not_gate(ALUControl[0]));
    BIT is_sub = and_gate4(not_gate(ALUControl[3]),
                           ALUControl[2],
                           ALUControl[1],
                           not_gate(ALUControl[0]));
    BIT is_slt = and_gate4(not_gate(ALUControl[3]),
                           ALUControl[2],
                           ALUControl[1],
                           ALUControl[0]);
    BIT is_nor = and_gate4(ALUControl[3],
                           ALUControl[2],
                           not_gate(ALUControl[1]),
                           not_gate(ALUControl[0]));

    BIT and_result[32];
    and32(Input1, Input2, and_result);
    BIT or_result[32];
    or32(Input1, Input2, or_result);
    BIT add_result[32];
    adder32(Input1, Input2, FALSE, add_result);
    BIT sub_result[32];
    adder32(Input1, Input2, TRUE, sub_result);
    BIT slt_result[32];
    slt32(Input1, Input2, slt_result);
    BIT nor_result[32];
    nor32(Input1, Input2, nor_result);

    for (int i = 0; i < 32; ++i) {
        Result[i] = or_gate6(and_gate(is_and, and_result[i]),
                             and_gate(is_or, or_result[i]),
                             and_gate(is_add, add_result[i]),
                             and_gate(is_sub, sub_result[i]),
                             and_gate(is_slt, slt_result[i]),
                             and_gate(is_nor, nor_result[i]));
    }

    for (int i = 0; i < 32; ++i) {
        *Zero = or_gate(*Zero, sub_result[i]);
    }
    *Zero = not_gate(*Zero);
}


int main() {
    BIT A[32] = {TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                 FALSE, FALSE, FALSE, FALSE, FALSE}; // 1
    BIT B[32] = {TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                 FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                 FALSE, FALSE, FALSE, FALSE, FALSE}; // 3

    BIT Result[32];
    BIT Zero = FALSE;


    BIT c[4] = {0, 0, 1, 1};
    ALU(c, A, B, &Zero, Result);
    for (int i = 31; i >= 0; i--) {
        printf("%d", Result[i]);
    }
    printf("\n");

    printf("%d\n", Zero);

    return 0;
}