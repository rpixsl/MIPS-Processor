/*
Class Project: The logical conclusion (v2.0)
CSCI-2500 Fall 2021
*/

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

// useful constants
BIT ONE [32] = {TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE};
BIT ZERO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                FALSE, FALSE, FALSE, FALSE, FALSE};
BIT REG_THIRTY_ONE[5] = {TRUE, TRUE, TRUE, TRUE, TRUE};
BIT THIRTY_TWO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE,
                      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
                      FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};


/******************************************************************************/
/* Function prototypes */
/******************************************************************************/

// gate
BIT not_gate (BIT A);
BIT or_gate  (BIT A, BIT B);
BIT or_gate3 (BIT A, BIT B, BIT C);
BIT or_gate6 (BIT A, BIT B, BIT C, BIT D, BIT E, BIT F);
BIT and_gate (BIT A, BIT B);
BIT and_gate3(BIT A, BIT B, BIT C);
BIT and_gate4(BIT A, BIT B, BIT C, BIT D);
BIT and_gate6(BIT A, BIT B, BIT C, BIT D, BIT E, BIT F);
BIT xor_gate (BIT A, BIT B);
BIT nor_gate (BIT A, BIT B);
BIT nand_gate(BIT A, BIT B);

// decoder
void decoder2(BIT I0, BIT I1, BIT* O0, BIT* O1, BIT* O2, BIT* O3);
void decoder3(BIT* I, BIT EN, BIT* O);
void decoder5(BIT* I, BIT* O);

// multiplexor
BIT  multiplexor2(BIT S, BIT I0, BIT I1);
void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output);
BIT  multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3);
void multiplexor4_5(BIT S0, BIT S1,
                    BIT* I0, BIT* I1, BIT* I2, BIT* I3, BIT* Output);
void multiplexor4_32(BIT S0, BIT S1,
                     BIT* I0, BIT* I1, BIT* I2, BIT* I3, BIT* Output);

// ALU
void adder1(BIT A, BIT B, BIT CarryIn, BIT* CarryOut, BIT* Sum);
void adder32(BIT* A, BIT* B, BIT Binvert, BIT* Result);
void and32(BIT* A, BIT* B, BIT* Result);
void or32(BIT* A, BIT* B, BIT* Result);
void slt32(BIT* A, BIT* B, BIT* Result);
void nor32(BIT* A, BIT* B, BIT* Result);

// bits operation
void copy_bits(const BIT* A, BIT* B);
void print_binary(const BIT* A);
void convert_to_binary(int a, BIT* A, int length);
void convert_to_binary_char(int a, char* A, int length);
int  binary_to_integer(const BIT* A);
int  binary_to_integer5(const BIT* A);

// parsing functions
void convert_opcode(char reg[], char reg_binary[]);
void convert_reg(char reg[], char reg_binary[]);
void convert_func(char reg[], char reg_binary[]);
int  get_instructions(BIT Instructions[][32]);

// Program state
void print_instruction();
void print_state();

// component
void Instruction_Memory(BIT* ReadAddress, BIT* Instruction);
void Control(const BIT* OpCode, const BIT* funct );
void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
                   BIT* ReadData1, BIT* ReadData2);
void Write_Register(BIT* WriteRegister, BIT* WriteData);
void ALU_Control(const BIT* funct);
void ALU(BIT* Input1, BIT* Input2, BIT* Result);
void Data_Memory(const BIT* Address, const BIT* WriteData, BIT* ReadData);
void Extend_Sign16(const BIT* Input, BIT* Output);
void updateState();


/******************************************************************************/
/* Functions provided for your convenience */
/******************************************************************************/

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
    // 3-to-8 decoder using gates
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
    // 5-to-32 decoder using 2-to-4 and 3-to-8 decoders
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

void multiplexor4_5(BIT S0, BIT S1,
                     BIT* I0, BIT* I1, BIT* I2, BIT* I3, BIT* Output) {
    for (int i = 0; i < 5; ++i) {
        BIT x0, x1, x2, x3 = FALSE;
        decoder2(S0, S1, &x0, &x1, &x2, &x3);

        BIT y0 = and_gate(x0, I0[i]);
        BIT y1 = and_gate(x1, I1[i]);
        BIT y2 = and_gate(x2, I2[i]);
        BIT y3 = and_gate(x3, I3[i]);

        BIT z0 = or_gate(y0, y1);
        BIT z1 = or_gate(y2, y3);

        Output[i] = or_gate(z0, z1);
    }
}

void multiplexor4_32(BIT S0, BIT S1,
                     BIT* I0, BIT* I1, BIT* I2, BIT* I3, BIT* Output) {
    for (int i = 0; i < 32; ++i) {
        BIT x0, x1, x2, x3 = FALSE;
        decoder2(S0, S1, &x0, &x1, &x2, &x3);

        BIT y0 = and_gate(x0, I0[i]);
        BIT y1 = and_gate(x1, I1[i]);
        BIT y2 = and_gate(x2, I2[i]);
        BIT y3 = and_gate(x3, I3[i]);

        BIT z0 = or_gate(y0, y1);
        BIT z1 = or_gate(y2, y3);

        Output[i] = or_gate(z0, z1);
    }
}

void adder1(BIT A, BIT B, BIT CarryIn, BIT* CarryOut, BIT* Sum) {
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

void convert_to_binary(int a, BIT* A, int length) {
    /* Use your implementation from Lab 6 */
    convert_to_binary_char(a, A, 32);
}

void convert_to_binary_char(int a, char* A, int length) {
    /* Use your implementation from Lab 6 */
    if (a >= 0) {
        for (int i = length-1; i >= 0; i--) {
            A[i] = (a % 2 == 1 ? '1' : '0');
            a /= 2;
        }
    }
    else {
        a += 1;
        for (int i = length-1; i >= 0; i--) {
            A[i] = (a % 2 == 0 ? '1' : '0');
            a /= 2;
        }
    }
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


/******************************************************************************/
/* Parsing functions */
/******************************************************************************/

// TODO: Implement any helper functions to assist with parsing

void convert_opcode(char reg[], char reg_binary[]) {
    if ((strcmp(reg, "add") == 0) | (strcmp(reg, "sub") == 0) |
        (strcmp(reg, "and") == 0) | (strcmp(reg, "or" ) == 0) |
        (strcmp(reg, "slt") == 0) | (strcmp(reg, "jr" ) == 0)) {
        strcpy(reg_binary, "000000");
    } else if (strcmp(reg, "j") == 0) {
        strcpy(reg_binary, "000010");
    } else if (strcmp(reg, "jal") == 0) {
        strcpy(reg_binary, "000011");
    } else if (strcmp(reg, "beq") == 0) {
        strcpy(reg_binary, "000100");
    } else if (strcmp(reg, "addi") == 0) {
        strcpy(reg_binary, "001000");
    } else if (strcmp(reg, "lw") == 0) {
        strcpy(reg_binary, "100011");
    } else if (strcmp(reg, "sw") == 0) {
        strcpy(reg_binary, "101011");
    } else {
        printf("Error: Unknown Register.\n");
    }
}

void convert_reg(char reg[], char reg_binary[]) {
    if (strcmp(reg, "zero") == 0) {
        strcpy(reg_binary, "00000");
    } else if (strcmp(reg, "v0") == 0) {
        strcpy(reg_binary, "00010");
    } else if (strcmp(reg, "a0") == 0) {
        strcpy(reg_binary, "00100");
    } else if (strcmp(reg, "t0") == 0) {
        strcpy(reg_binary, "01000");
    } else if (strcmp(reg, "t1") == 0) {
        strcpy(reg_binary, "01001");
    } else if (strcmp(reg, "s0") == 0) {
        strcpy(reg_binary, "10000");
    } else if (strcmp(reg, "s1") == 0) {
        strcpy(reg_binary, "10001");
    } else if (strcmp(reg, "sp") == 0) {
        strcpy(reg_binary, "11101");
    } else if (strcmp(reg, "ra") == 0) {
        strcpy(reg_binary, "11111");
    } else {
        printf("Error: Unknown Register.\n");
    }
}

void convert_func(char reg[], char reg_binary[]) {
    if (strcmp(reg, "add") == 0) {
        strcpy(reg_binary, "100000");
    } else if (strcmp(reg, "sub") == 0) {
        strcpy(reg_binary, "100010");
    } else if (strcmp(reg, "and") == 0) {
        strcpy(reg_binary, "100100");
    } else if (strcmp(reg, "or") == 0) {
        strcpy(reg_binary, "100101");
    } else if (strcmp(reg, "slt") == 0) {
        strcpy(reg_binary, "101010");
    } else if (strcmp(reg, "jr") == 0) {
        strcpy(reg_binary, "001000");
    } else {
        printf("Error: Unknown Register.\n");
    }
}

int  instruction_count = 0; // counter: number of instruction
int get_instructions(BIT Instructions[][32]) {
    // mips instruction
    BIT op  [128] = {FALSE};
    BIT reg1[128] = {FALSE};
    BIT reg2[128] = {FALSE};
    BIT reg3[128] = {FALSE};
    // binary instruction
    BIT opcode[6] = {FALSE};     // opcode   : [31-26]
    BIT rs[6]   = {FALSE};       // rs       : [25-21]
    BIT rt[6]   = {FALSE};       // rt       : [20-16]
    BIT rd[6]   = {FALSE};       // rd       : [15-11]
    BIT func[7] = {FALSE};       // func     : [5-0]
    BIT immediate[16] = {FALSE}; // immediate: [15-0]
    BIT address  [26] = {FALSE}; // address  : [25-0]

    char line[128] = {0};
    while (fgets(line, 128, stdin) != NULL) {
        // TODO: perform conversion of instructions to binary
        // Input: coming from stdin via: ./a.out < input.txt
        // Output: Convert instructions to binary in the instruction memory
        // Return: Total number of instructions
        // Note: you are free to use if-else and external libraries here
        // Note: you don't need to implement circuits for saving to op. mem.
        // My approach:
        // - Use sscanf on line to get strings for instruction and registers
        // - Use instructions to determine op code, funct, and shamt fields
        // - Convert immediate field and jump address field to binary
        // - Use registers to get rt, rd, rs fields
        // Note: I parse everything as strings, then convert to BIT array at end

        sscanf(line, "%s %s %s %s", op, reg1, reg2, reg3);

        if ((strcmp(op, "add") == 0) | (strcmp(op, "sub") == 0) |
            (strcmp(op, "or" ) == 0) | (strcmp(op, "and") == 0) |
            (strcmp(op, "slt") == 0)) {
            // convert mips code into binary instruction
            convert_opcode(op, opcode);
            convert_reg(reg2, rs);
            convert_reg(reg3, rt);
            convert_reg(reg1, rd);
            convert_func(op, func);

            // store 32-bit binary instruction in the 'Instructions'
            for (int i = 0; i < 6; ++i) { // func: [5-0]
                Instructions[instruction_count][i] = func[5-i];
            }
            for (int i = 0; i < 5; ++i) { // shamt: [10-6]
                Instructions[instruction_count][i+6] = '0';
            }
            for (int i = 0; i < 5; ++i) { // rd: [15-11]
                Instructions[instruction_count][i+11] = rd[4-i];
            }
            for (int i = 0; i < 5; ++i) { // rt: [20-16]
                Instructions[instruction_count][i+16] = rt[4-i];
            }
            for (int i = 0; i < 5; ++i) { // rs: [25-21]
                Instructions[instruction_count][i+21] = rs[4-i];
            }
            for (int i = 0; i < 6; ++i) { // opcode: [31-26]
                Instructions[instruction_count][i+26] = opcode[5-i];
            }
        }
        else if (strcmp(op, "beq") == 0) {
            // convert mips code into binary instruction
            convert_opcode(op, opcode);
            convert_reg(reg2, rs);
            convert_reg(reg1, rt);
            convert_to_binary_char(atoi(reg3), immediate, 16);

            // store 32-bit binary instruction in the 'Instructions'
            for (int i = 0; i < 16; ++i) { // immediate: [15-0]
                Instructions[instruction_count][i] = immediate[15-i];
            }
            for (int i = 0; i < 5; ++i) { // rt: [20-16]
                Instructions[instruction_count][i+16] = rs[4-i];
            }
            for (int i = 0; i < 5; ++i) { // rs: [25-21]
                Instructions[instruction_count][i+21] = rt[4-i];
            }
            for (int i = 0; i < 6; ++i) { // opcode: [31-26]
                Instructions[instruction_count][i+26] = opcode[5-i];
            }
        }
        else if ((strcmp(op, "lw" ) == 0) | (strcmp(op, "sw"  ) == 0) |
                 (strcmp(op, "addi") == 0) ) {
            // convert mips code into binary instruction
            convert_opcode(op, opcode);
            convert_reg(reg2, rs);
            convert_reg(reg1, rt);
            convert_to_binary_char(atoi(reg3), immediate, 16);

            // store 32-bit binary instruction in the 'Instructions'
            for (int i = 0; i < 16; ++i) { // immediate: [15-0]
                Instructions[instruction_count][i] = immediate[15-i];
            }
            for (int i = 0; i < 5; ++i) { // rt: [20-16]
                Instructions[instruction_count][i+16] = rt[4-i];
            }
            for (int i = 0; i < 5; ++i) { // rs: [25-21]
                Instructions[instruction_count][i+21] = rs[4-i];
            }
            for (int i = 0; i < 6; ++i) { // opcode: [31-26]
                Instructions[instruction_count][i+26] = opcode[5-i];
            }
        }
        else if ((strcmp(op, "j") == 0) | (strcmp(op, "jal") == 0)) {
            // convert mips code into binary instruction
            convert_opcode(op, opcode);
            convert_to_binary_char(atoi(reg1), address, 26);

            // store 32-bit binary instruction in the 'Instructions'
            for (int i = 0; i < 26; ++i) { // address: [25-0]
                Instructions[instruction_count][i] = address[25-i];
            }
            for (int i = 0; i < 6; ++i) { // opcode: [31-26]
                Instructions[instruction_count][i+26] = opcode[5-i];
            }
        }
        else if ((strcmp(op, "jr") == 0)) {
            // convert mips code into binary instruction
            convert_opcode(op, opcode);
            convert_reg(reg1, rs);
            convert_func(op, func);

            // store 32-bit binary instruction in the 'Instructions'
            for (int i = 0; i < 6; ++i) { // func: [5-0]
                Instructions[instruction_count][i] = func[5-i];
            }
            for (int i = 0; i < 5; ++i) { // shamt: [10-6]
                Instructions[instruction_count][i+6] = '0';
            }
            for (int i = 0; i < 5; ++i) { // rd: [15-11]
                Instructions[instruction_count][i+11] = '1';
            }
            for (int i = 0; i < 5; ++i) { // rt: [20-16]
                Instructions[instruction_count][i+16] = '0';
            }
            for (int i = 0; i < 5; ++i) { // rs: [25-21]
                Instructions[instruction_count][i+21] = rs[4-i];
            }
            for (int i = 0; i < 6; ++i) { // opcode: [31-26]
                Instructions[instruction_count][i+26] = opcode[5-i];
            }
        }
        else {
            printf("Error: Unknown operation.\n");
        }

        for (int i = 0; i < 32; ++i){
            Instructions[instruction_count][i] =
                    (Instructions[instruction_count][i] == '1' ? TRUE : FALSE);
        }

        instruction_count++;
    }

    return instruction_count;
}


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

BIT Zero          =  FALSE ;
BIT ALUControl[4] = {FALSE};

void print_instruction() {
    unsigned pc = binary_to_integer(PC);
    printf("PC: %d\n", pc);
    printf("Instruction: ");
    print_binary(MEM_Instruction[pc]);
    printf("\n");
}

void print_state() {
    printf("Data: ");
    for (int i = 0; i < 32; ++i) {
        printf("%d ", binary_to_integer(MEM_Data[i]));
    }
    printf("\n");

    printf("Register: ");
    for (int i = 0; i < 32; ++i) {
        printf("%d ", binary_to_integer(MEM_Register[i]));
    }
    printf("\n");
}


/******************************************************************************/
/* Functions that you will implement */
/******************************************************************************/

void Instruction_Memory(BIT* ReadAddress, BIT* Instruction) {
    // TODO: Implement instruction memory
    // Input: 32-bit instruction address
    // Output: 32-bit binary instruction
    // Note: Useful to use a 5-to-32 decoder here
    for (int i = 0; i < 32; i++) {
        int index = binary_to_integer(ReadAddress);
        Instruction[i] = MEM_Instruction[index][i];
    }
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
                           not_gate(OpCode[0]) );
    BIT jr     = and_gate6(not_gate(funct[5]),
                           not_gate(funct[4]),
                           funct[3],
                           not_gate(funct[2]),
                           not_gate(funct[1]),
                           not_gate(funct[0]) );
    BIT j      = and_gate6(not_gate(OpCode[5]),
                           not_gate(OpCode[4]),
                           not_gate(OpCode[3]),
                           not_gate(OpCode[2]),
                           OpCode[1],
                           not_gate(OpCode[0]) );
    BIT jal    = and_gate6(not_gate(OpCode[5]),
                           not_gate(OpCode[4]),
                           not_gate(OpCode[3]),
                           not_gate(OpCode[2]),
                           OpCode[1],
                           OpCode[0] );
    BIT beq    = and_gate6(not_gate(OpCode[5]),
                           not_gate(OpCode[4]),
                           not_gate(OpCode[3]),
                           OpCode[2],
                           not_gate(OpCode[1]),
                           not_gate(OpCode[0]) );
    BIT lw     = and_gate6(OpCode[5],
                           not_gate(OpCode[4]),
                           not_gate(OpCode[3]),
                           not_gate(OpCode[2]),
                           OpCode[1],
                           OpCode[0] );
    BIT sw     = and_gate6(OpCode[5],
                           not_gate(OpCode[4]),
                           OpCode[3],
                           not_gate(OpCode[2]),
                           OpCode[1],
                           OpCode[0] );
    BIT addi   = and_gate6(not_gate(OpCode[5]),
                           not_gate(OpCode[4]),
                           OpCode[3],
                           not_gate(OpCode[2]),
                           not_gate(OpCode[1]),
                           not_gate(OpCode[0]) );

    RegDst[1]   = jal;
    RegDst[0]   = R_type;
    ALUSrc      = or_gate3(lw, sw, addi);
    MemToReg[1] = jal;
    MemToReg[0] = lw;
    RegWrite    = or_gate(or_gate(and_gate(R_type, not_gate(jr)) ,jal ),
                          or_gate(lw, addi) );
    MemRead  = lw;
    MemWrite = sw;
    Branch   = beq;
    Jump     = or_gate(j, jal);
    JMPReg   = and_gate(R_type, jr);
    ALUOp[1] = R_type;
    ALUOp[0] = beq;
}

void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
                   BIT* ReadData1, BIT* ReadData2) {
    // TODO: Implement register read functionality
    // Input: two 5-bit register addresses
    // Output: the values of the specified registers in ReadData1 and ReadData2
    // Note: Implementation will be very similar to instruction memory circuit
    BIT read_index1[32] = {FALSE};
    BIT read_index2[32] = {FALSE};
    decoder5(ReadRegister1, read_index1);
    decoder5(ReadRegister2, read_index2);

    for (int i = 0; i < 32; i++) {
        multiplexor2_32(or_gate(read_index1[i], ZERO[i]), ReadData1, MEM_Register[i], ReadData1);
        multiplexor2_32(or_gate(read_index2[i], ZERO[i]), ReadData2, MEM_Register[i], ReadData2);
    }
}

void Write_Register(BIT* WriteRegister, BIT* WriteData) {
    // TODO: Implement register write functionality
    // Input: one 5-bit register address, data to write, and control bit
    // Output: None, but will modify register file
    // Note: Implementation will again be similar to those above
    BIT write_index[32] = {FALSE};
    decoder5(WriteRegister, write_index);

    for (int i = 0; i < 32; i++) {
        multiplexor2_32(and_gate(RegWrite, write_index[i]), MEM_Register[i], WriteData, MEM_Register[i]);
    }
}

void ALU_Control(const BIT* funct) {
    // TODO: Implement ALU Control circuit
    // Input: 2-bit ALUOp from main control circuit, 6-bit funct field from the
    //        binary instruction
    // Output:4-bit ALUControl for input into the ALU
    // Note: Can use SOP or similar approaches to determine bits
    ALUControl[0] = and_gate(or_gate(funct[0], funct[3]), ALUOp[1]);
    ALUControl[1] = or_gate (not_gate(ALUOp[1]), not_gate(funct[2]));
    ALUControl[2] = or_gate (and_gate(ALUOp[1], funct[1]), ALUOp[0]);
    ALUControl[3] = and_gate(ALUOp[0], not_gate(ALUOp[0]));
}

void ALU(BIT* Input1, BIT* Input2, BIT* Result) {
    // TODO: Implement 32-bit ALU
    // Input: 4-bit ALUControl, two 32-bit inputs
    // Output: 32-bit result, and zero flag big
    // Note: Can re-use prior implementations (but need new circuitry for zero)

    // A. determine which result we should output by 'ALU Control'
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

    // B. determine six kinds result
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

    // C. output the result by the determination above
    for (int i = 0; i < 32; ++i) {
        Result[i] = or_gate6(and_gate(is_and, and_result[i]),
                             and_gate(is_or, or_result[i]),
                             and_gate(is_add, add_result[i]),
                             and_gate(is_sub, sub_result[i]),
                             and_gate(is_slt, slt_result[i]),
                             and_gate(is_nor, nor_result[i]));
    }

    // D. output the 'Zero' 1-bit control signal
    for (int i = 0; i < 32; ++i) {
        Zero = or_gate(Zero, sub_result[i]);
    }
    Zero = not_gate(Zero);
}

void Data_Memory(const BIT* Address, const BIT* WriteData, BIT* ReadData) {
    // TODO: Implement data memory
    // Input: 32-bit address, control flags for read/write, and data to write
    // Output: data read if processing a lw instruction
    // Note: Implementation similar as above

    int index = binary_to_integer(Address);
    for (int i = 0; i < 32; i++) {
        // write data
        MEM_Data[index][i] = multiplexor2(MemWrite,
                                          MEM_Data[index][i],
                                          WriteData[i]);
        // read data
        ReadData[i] = and_gate(MEM_Data[index][i], MemRead);
    }
}

void Extend_Sign16(const BIT* Input, BIT* Output) {
    // TODO: Implement 16-bit to 32-bit sign extender
    // Copy Input to Output, then extend 16th Input bit to 17-32 bits in Output
    for (int i = 0; i < 16; ++i) {
        Output[i] = Input[i];
    }
    for (int i = 16; i < 32; ++i) {
        Output[i] = Input[15];
    }
}

void updateState() {
    // TODO: Implement the full datapath here
    // Essentially, you'll be figuring out the order in which to process each of
    // the sub-circuits comprising the entire processor circuit. It makes it
    // easier to consider the pipelined version of the process, and handle things
    // in order of the pipeline. The stages and operations are:
    // Fetch - load instruction from instruction memory
    // Decode - set control bits and read from the register file
    // Execute - process ALU
    // Memory - read/write data memory
    // Write Back - write to the register file
    // Update PC - determine the final PC value for the next instruction

    BIT Next_PC[32] = {FALSE};
    BIT Instruction[32] = {FALSE};

    BIT opcode[6] = {FALSE};
    BIT rs[5] = {FALSE};
    BIT rt[5] = {FALSE};
    BIT rd[5] = {FALSE};
    BIT func[6] = {FALSE};
    BIT immediate[32] = {FALSE};
    BIT address[26] = {FALSE};
    BIT jal[5] = {FALSE};

    // A. Fetch - load instruction from instruction memory

    // get next pc address: PC+1
    copy_bits(PC, Next_PC);
    adder32(Next_PC, ONE, FALSE, Next_PC);

    // load instruction
    Instruction_Memory(PC, Instruction);
    for (int i = 0; i < 6 ; i++) opcode[i] = Instruction[i+26];
    for (int i = 0; i < 5 ; i++) rs[i] = Instruction[i+21];
    for (int i = 0; i < 5 ; i++) rt[i] = Instruction[i+16];
    for (int i = 0; i < 5 ; i++) rd[i] = Instruction[i+11];
    for (int i = 0; i < 6 ; i++) func[i]      = Instruction[i];
    for (int i = 0; i < 16; i++) immediate[i] = Instruction[i];
    Extend_Sign16(immediate, immediate);
    for (int i = 0; i < 26; i++) address[i]   = Instruction[i];
    for (int i = 0; i < 5 ; i++) jal[i] = TRUE;

    // B. Decode

    // set control bits
    Control(opcode, func);
    ALU_Control(func);

    // read from the register file
    BIT Read_reg_1[32] = {FALSE}; // input
    BIT Read_reg_2[32] = {FALSE}; // input
    BIT ReadData1 [32] = {FALSE}; // output
    BIT ReadData2 [32] = {FALSE}; // output
    copy_bits(rs, Read_reg_1);
    copy_bits(rt, Read_reg_2);
    Read_Register(rs, rt,
                  ReadData1, ReadData2);

    // C. Execute - process ALU

    BIT ALU_input_1[32] = {FALSE}; // input
    BIT ALU_input_2[32] = {FALSE}; // input
    BIT ALU_result [32] = {FALSE}; // output
    copy_bits(ReadData1, ALU_input_1);
    multiplexor2_32(ALUSrc, ReadData2, immediate, ALU_input_2);
    ALU(ALU_input_1, ALU_input_2, ALU_result);

    // D. Memory - read/write data memory

    BIT Data_address[32]    = {FALSE}; // input
    BIT Data_write_data[32] = {FALSE}; // input
    BIT Data_read_data[32]  = {FALSE}; // output
    copy_bits(ALU_result, Data_address);
    copy_bits(ReadData2, Data_write_data);
    Data_Memory(Data_address, Data_write_data,
                Data_read_data);

    // E. Write Back - write to the register file

    BIT WriteRegister[32] = {FALSE}; // input
    BIT WriteData[32]     = {FALSE}; // input
    multiplexor4_5(RegDst[0], RegDst[1],
                     rt, rd, jal, REG_THIRTY_ONE,
                     WriteRegister);
    multiplexor4_32(MemToReg[0], MemToReg[1],
                    ALU_result, Data_read_data, Next_PC, ZERO,
                    WriteData);
    Write_Register(WriteRegister, WriteData);

    // F. Update PC - determine the final PC value for the next instruction
    //    Consist one adder and three mux(s)

    // adder
    BIT Adder_input_1[32] = {FALSE}; // input  of adder
    BIT Adder_input_2[32] = {FALSE}; // input  of adder
    BIT BranchTarget [32] = {FALSE}; // output of adder , input of mux_1
    copy_bits(Next_PC, Adder_input_1);
    copy_bits(immediate, Adder_input_2);
    adder32(Adder_input_1, Adder_input_2,
            FALSE, BranchTarget);

    // mux_1  --  for branch
    BIT mux_1_control = FALSE;     // control of mux_1
    BIT mux_1_input_1[32] = {FALSE}; // input  of mux_1
    BIT mux_1_input_2[32] = {FALSE}; // input  of mux_1, output of adder
    BIT mux_1_output [32] = {FALSE}; // output of mux_1, input of mux_2
    copy_bits(Next_PC, mux_1_input_1);
    copy_bits(BranchTarget, mux_1_input_2);
    mux_1_control = and_gate(Branch, Zero);
    multiplexor2_32(mux_1_control, mux_1_input_1, mux_1_input_2,
                    mux_1_output);

    // mux_2  --  for j, jal
    BIT mux_2_control = FALSE;     // control of mux_2
    BIT mux_2_input_1[32] = {FALSE}; // input  of mux_2, output of mux_1
    BIT mux_2_input_2[32] = {FALSE}; // input  of mux_2
    BIT mux_2_output [32] = {FALSE}; // output of mux_2, input  of mux_3
    mux_2_control = Jump;
    copy_bits(mux_1_output, mux_2_input_1);
    for (int i = 0; i < 26; ++i) mux_2_input_2[i] = address[i];
    multiplexor2_32(mux_2_control, mux_2_input_1, mux_2_input_2,
                    mux_2_output);

    // mux_3  --  for jr
    BIT mux_3_control = FALSE;     // control of mux_3
    BIT mux_3_input_1[32] = {FALSE}; // input of mux_3 , output of mux_2
    BIT mux_3_input_2[32] = {FALSE}; // input of mux_3
    BIT mux_3_output [32] = {FALSE}; // output of mux_3, final PC address
    mux_3_control = JMPReg;
    copy_bits(mux_2_output, mux_3_input_1);
    copy_bits(ReadData1, mux_3_input_2);
    multiplexor2_32(mux_3_control, mux_3_input_1, mux_3_input_2,
                    mux_3_output);

    // update the PC
    copy_bits(mux_3_output, PC);

    // G. Default all the control signal

    RegDst[1]     =  FALSE;
    RegDst[0]     =  FALSE;
    ALUSrc        =  FALSE;
    MemToReg[1]   =  FALSE;
    MemToReg[0]   =  FALSE;
    RegWrite      =  FALSE;
    MemRead       =  FALSE;
    MemWrite      =  FALSE;
    Branch        =  FALSE;
    Jump          =  FALSE;
    JMPReg        =  FALSE;
    ALUOp[1]      =  FALSE;
    ALUOp[0]      =  FALSE;

    Zero          =  FALSE;
    ALUControl[3] =  FALSE;
    ALUControl[2] =  FALSE;
    ALUControl[1] =  FALSE;
    ALUControl[0] =  FALSE;
}


/******************************************************************************/
/* Main */
/******************************************************************************/

int main() {
    setbuf(stdout, NULL);

    // parse instructions into binary format
    int counter = get_instructions(MEM_Instruction);

    // load program and run
    copy_bits(ZERO, PC);
    copy_bits(THIRTY_TWO, MEM_Register[29]);

    while (binary_to_integer(PC) < counter) {
        print_instruction();
        updateState();
        print_state();
    }

    return 0;
}