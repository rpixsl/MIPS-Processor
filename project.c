/* 
Class Project: The logical conclusion (v1.1)
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
BIT ONE[32] = {TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
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
BIT not_gate (BIT A);
BIT or_gate  (BIT A, BIT B);
BIT or_gate3 (BIT A, BIT B, BIT C);
BIT and_gate (BIT A, BIT B);
BIT and_gate3(BIT A, BIT B, BIT C);
BIT and_gate6(BIT A, BIT B, BIT C, BIT D, BIT E, BIT F);
BIT xor_gate (BIT A, BIT B);
BIT nor_gate (BIT A, BIT B);
BIT nand_gate(BIT A, BIT B);

void decoder2(BIT I0, BIT I1, BIT* O0, BIT* O1, BIT* O2, BIT* O3);
BIT  multiplexor2(BIT S, BIT I0, BIT I1);
void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output);
BIT  multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3);

void copy_bits(const BIT* A, BIT* B);
void print_binary(const BIT* A);
void convert_to_binary(int a, BIT* A, int length);
void convert_to_binary_char(int a, char* A, int length);
int  binary_to_integer(const BIT* A);

int  get_instructions(BIT Instructions[][32]);

void Instruction_Memory(BIT* ReadAddress, BIT* Instruction);
void Control(const BIT* OpCode, const BIT* funct,
             BIT* RegDst, BIT* ALUSrc, BIT* MemToReg, BIT* RegWrite,
             BIT* MemRead, BIT* MemWrite, BIT* Branch, BIT* Jump, BIT* JMPReg,
             BIT* ALUOp );
void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
                   BIT* ReadData1, BIT* ReadData2);
void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData);
void ALU_Control(const BIT* ALUOp, const BIT* funct, BIT* ALUControl);
void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result);
void Data_Memory(BIT MemWrite, BIT MemRead,
                 BIT* Address, BIT* WriteData, BIT* ReadData);
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

void convert_to_binary(int a, BIT* A, int length) {
    /* Use your implementation from Lab 6 */
    convert_to_binary_char(a, A, 32);
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

int  binary_to_integer(const BIT* A) {
    unsigned a = 0;
    unsigned mult = 1;

    for (int i = 0; i < 32; ++i) {
        a += A[i] * mult;
        mult *= 2;
    }

    return (int) a;
}


/******************************************************************************/
/* Parsing functions */
/******************************************************************************/

// TODO: Implement any helper functions to assist with parsing

int  get_instructions(BIT Instructions[][32]) {
    char line[256] = {0};
    int instruction_count = 0;
    while (fgets(line, 256, stdin) != NULL) {
        // TODO: perform conversion of instructions to binary
        // Input: coming from stdin via: ./a.out < input.txt
        // Output: Convert instructions to binary in the instruction memory
        // Return: Total number of instructions
        // Note: you are free to use if-else and external libraries here
        // Note: you don't need to implement circuits for saving to inst. mem.
        // My approach:
        // - Use sscanf on line to get strings for instruction and registers
        // - Use instructions to determine op code, funct, and shamt fields
        // - Convert immediate field and jump address field to binary
        // - Use registers to get rt, rd, rs fields
        // Note: I parse everything as strings, then convert to BIT array at end

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

BIT Zero          =  FALSE;
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
}

void Control(const BIT* OpCode, const BIT* funct,
             BIT* RegDst, BIT* ALUSrc, BIT* MemToReg, BIT* RegWrite,
             BIT* MemRead, BIT* MemWrite, BIT* Branch, BIT* Jump, BIT* JMPReg,
             BIT* ALUOp ) {
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
    ALUSrc      = or_gate3(lw, sw, addi);
    MemToReg[1] = jal;
    MemToReg[0] = lw;
    *RegWrite    = or_gate(or_gate(
                                and_gate(R_type, not_gate(jr)) ,
                                jal
                                    )  ,
                          or_gate(lw, addi)
                          );
    MemRead     = lw;
    MemWrite    = sw;
    Branch      = beq;
    Jump        = or_gate(j, jal);
    JMPReg      = and_gate(R_type, jr);
    ALUOp[1]    = R_type;
    ALUOp[0]    = beq;
}

void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
                   BIT* ReadData1, BIT* ReadData2) {
    // TODO: Implement register read functionality
    // Input: two 5-bit register addresses
    // Output: the values of the specified registers in ReadData1 and ReadData2
    // Note: Implementation will be very similar to instruction memory circuit
}

void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData) {
    // TODO: Implement register write functionality
    // Input: one 5-bit register address, data to write, and control bit
    // Output: None, but will modify register file
    // Note: Implementation will again be similar to those above
}

void ALU_Control(const BIT* ALUOp, const BIT* funct, BIT* ALUControl) {
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

void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result) {
    // TODO: Implement 32-bit ALU
    // Input: 4-bit ALUControl, two 32-bit inputs
    // Output: 32-bit result, and zero flag big
    // Note: Can re-use prior implementations (but need new circuitry for zero)
}

void Data_Memory(BIT MemWrite, BIT MemRead,
                 BIT* Address, BIT* WriteData, BIT* ReadData) {
    // TODO: Implement data memory
    // Input: 32-bit address, control flags for read/write, and data to write
    // Output: data read if processing a lw instruction
    // Note: Implementation similar as above
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
