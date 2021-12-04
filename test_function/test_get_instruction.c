#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef char BIT;
#define TRUE 1
#define FALSE 0

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

void convert_to_binary(int a, BIT* A, int length) {
    /* Use your implementation from Lab 6 */
    convert_to_binary_char(a, A, 32);
}

int binary_to_integer(BIT* A)
{
  unsigned a = 0;
  unsigned mult = 1;
  
  for (int i = 0; i < 32; ++i) {
    a += A[i]*mult;
    mult *= 2;
  }
  
  return (int)a;
}


/******************************************************************************/
/* Parsing functions */
/******************************************************************************/

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

int get_instructions(BIT Instructions[][32]) {
    char line[128] = {0};
    int  instruction_count = 0;

    // mips instruction
    BIT op  [128] = {FALSE};
    BIT reg1[128] = {FALSE};
    BIT reg2[128] = {FALSE};
    BIT reg3[128] = {FALSE};
    // binary instruction
    BIT opcode[6] = {FALSE};
    BIT rs[5]     = {FALSE};
    BIT rt[5]     = {FALSE};
    BIT rd[5]     = {FALSE};
    BIT immediate[16] = {FALSE};
    BIT address[26]   = {FALSE};
    BIT func[6]       = {FALSE};

    while (fgets(line, 256, stdin) != NULL) {
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

            convert_opcode(op, opcode);
            convert_reg(reg2, rs);
            convert_reg(reg3, rt);
            convert_reg(reg1, rd);
            convert_func(op, func);

            for (int i = 0; i < 6; ++i) {
                Instructions[instruction_count][i] = func[5-i];
            }
            for (int i = 0; i < 5; ++i) {
                Instructions[instruction_count][i+6] = '0';
            }
            for (int i = 0; i < 5; ++i) {
                Instructions[instruction_count][i+11] = rd[4-i];
            }
            for (int i = 0; i < 5; ++i) {
                Instructions[instruction_count][i+16] = rt[4-i];
            }
            for (int i = 0; i < 5; ++i) {
                Instructions[instruction_count][i+21] = rs[4-i];
            }
            for (int i = 0; i < 6; ++i) {
                Instructions[instruction_count][i+26] = opcode[5-i];
            }
        }
        else if ((strcmp(op, "lw" ) == 0) | (strcmp(op, "sw"  ) == 0) |
                 (strcmp(op, "beq") == 0) | (strcmp(op, "addi") == 0) ) {

            convert_opcode(op, opcode);
            convert_reg(reg2, rs);
            convert_reg(reg1, rt);
            convert_to_binary_char(atoi(reg3), immediate, 16);

            for (int i = 0; i < 16; ++i) {
                Instructions[instruction_count][i] = immediate[15-i];
            }
            for (int i = 0; i < 5; ++i) {
                Instructions[instruction_count][i+16] = rt[4-i];
            }
            for (int i = 0; i < 5; ++i) {
                Instructions[instruction_count][i+21] = rs[4-i];
            }
            for (int i = 0; i < 6; ++i) {
                Instructions[instruction_count][i+26] = opcode[5-i];
            }
        }
        else if ((strcmp(op, "j") == 0) | (strcmp(op, "jal") == 0)) {

            convert_opcode(op, opcode);
            convert_to_binary_char(atoi(reg1), address, 26);

            for (int i = 0; i < 26; ++i) {
                Instructions[instruction_count][i] = address[25-i];
            }
            for (int i = 0; i < 6; ++i) {
                Instructions[instruction_count][i+26] = opcode[5-i];
            }
        }
        else if ((strcmp(op, "jr") == 0)) {

            convert_opcode(op, opcode);
            convert_reg(reg1, rs);
            convert_func(op, func);

            for (int i = 0; i < 6; ++i) {
                Instructions[instruction_count][i] = func[5-i];
            }
            for (int i = 0; i < 5; ++i) {
                Instructions[instruction_count][i+6] = '0';
            }
            for (int i = 0; i < 5; ++i) {
                Instructions[instruction_count][i+11] = '0';
            }
            for (int i = 0; i < 5; ++i) {
                Instructions[instruction_count][i+16] = '0';
            }
            for (int i = 0; i < 5; ++i) {
                Instructions[instruction_count][i+21] = rs[4-i];
            }
            for (int i = 0; i < 6; ++i) {
                Instructions[instruction_count][i+26] = opcode[5-i];
            }
        }
        else {
            printf("Error: Unknown operation.\n");
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

int main() {
    setbuf(stdout, NULL);

    // parse instructions into binary format
    int counter = get_instructions(MEM_Instruction);

    for (int i = 0; i < counter; i++) {
        printf("%s\n\n", MEM_Instruction[i]);
    }

    return 0;
}
