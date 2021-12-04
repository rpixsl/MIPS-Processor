//
// Created by Tianrui Qi on 12/3/21.
// Test the component function of project.c: 'Control'
//

#include <stdio.h>

typedef char BIT;
#define TRUE 1
#define FALSE 0

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

void Control(const BIT* OpCode, const BIT* funct ) {
    // TODO: Set control bits for everything
    // Input: opcode field from the instruction
    // OUtput: all control lines get set
    // Note: Can use SOP or similar approaches to determine bits
    BIT R_type = and_gate6(not_gate(OpCode[5]),
                           not_gate(OpCode[4]),
                           not_gate(OpCode[3]),
                           not_gate(OpCode[2]),
                           not_gate(OpCode[1]),
                           not_gate(OpCode[0])
    );
    BIT jr     = and_gate6(not_gate(funct[5]),
                           not_gate(funct[4]),
                           funct[3],
                           not_gate(funct[2]),
                           not_gate(funct[1]),
                           not_gate(funct[0])
    );
    BIT j      = and_gate6(not_gate(OpCode[5]),
                           not_gate(OpCode[4]),
                           not_gate(OpCode[3]),
                           not_gate(OpCode[2]),
                           OpCode[1],
                           not_gate(OpCode[0])
    );
    BIT jal    = and_gate6(not_gate(OpCode[5]),
                           not_gate(OpCode[4]),
                           not_gate(OpCode[3]),
                           not_gate(OpCode[2]),
                           OpCode[1],
                           OpCode[0]
    );
    BIT beq    = and_gate6(not_gate(OpCode[5]),
                           not_gate(OpCode[4]),
                           not_gate(OpCode[3]),
                           OpCode[2],
                           not_gate(OpCode[1]),
                           not_gate(OpCode[0])
    );
    BIT lw     = and_gate6(OpCode[5],
                           not_gate(OpCode[4]),
                           not_gate(OpCode[3]),
                           not_gate(OpCode[2]),
                           OpCode[1],
                           OpCode[0]
    );
    BIT sw     = and_gate6(OpCode[5],
                           not_gate(OpCode[4]),
                           OpCode[3],
                           not_gate(OpCode[2]),
                           OpCode[1],
                           OpCode[0]
    );
    BIT addi   = and_gate6(not_gate(OpCode[5]),
                           not_gate(OpCode[4]),
                           OpCode[3],
                           not_gate(OpCode[2]),
                           not_gate(OpCode[1]),
                           not_gate(OpCode[0])
    );

    RegDst[1]   = jal;
    RegDst[0]   = R_type;
    ALUSrc     = or_gate3(lw, sw, addi);
    MemToReg[1] = jal;
    MemToReg[0] = lw;
    RegWrite   = or_gate(or_gate(
                                  and_gate(R_type, not_gate(jr)) ,
                                  jal )  ,
                          or_gate(lw, addi)
    );
    MemRead  = lw;
    MemWrite = sw;
    Branch   = beq;
    Jump     = or_gate(j, jal);
    JMPReg   = and_gate(R_type, jr);
    ALUOp[1]  = R_type;
    ALUOp[0]  = beq;
}

int main(){
    BIT OpCode[6] = {TRUE, TRUE, FALSE, FALSE, FALSE, FALSE};
    BIT funct [6] = {FALSE, FALSE, FALSE , TRUE, FALSE, FALSE};

    Control(OpCode, funct);
    printf("%d%d\n", RegDst[1],RegDst[0] );
    printf("%d\n", ALUSrc );
    printf("%d%d\n", MemToReg[1],  MemToReg[0] );
    printf("%d\n", RegWrite );
    printf("%d\n", MemRead );
    printf("%d\n", MemWrite );
    printf("%d\n", Branch );
    printf("%d\n", Jump );
    printf("%d\n", JMPReg );
    printf("%d%d\n", ALUOp[1], ALUOp[0] );

    return 0;
}