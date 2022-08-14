# MIPS Processor in C

Another version of instruction is available at [Craft](https://www.craft.do/s/FpLeUcZV9xNpvJ) (same to this `README.md`). 

## Overview

We implement a full gate-level circuit representing the datapath for a reduced, but still Turing complete, MIPS instruction set architecture (ISA). We first implement logic gates like and, or, not, xor, and nand and then use them to build essential components, including memory, control, ALU, decoder, adder, multiplexor, etc. At length, we connect them by datapath. 

There some rules we follow in our C implementation:
1. No if and if-else logical control statements or equivalent.
2. No different loop structures, i.e., basic for loops other than `for int i = 0; i < N; ++i`.
3. No calls to any external or library functions.

Basically, we want to ensure that we use gates for all logical control and operations. No tricky workarounds are allowed.

To use the code, store MIPS instruction in a file, i.e., `example1.txt`, and run the following code:
```
$ gcc -c project.c
$ gcc project.o
$ ./a.out < example1.txt
```

## Supported Instructions

Our ISA will include the following instructions:

| Instruction | Type   | Description         | Input format            | Operation                      | op [31-26] | func [5-0] |
|-------------|--------|---------------------|-------------------------|--------------------------------|------------|------------|
| add         | R-type | Integer addition    | add reg1 reg2 reg3      | reg1 = reg2 + reg3             | 000000     | 100000     |
| sub         | R-type | Integer subtraction | sub reg1 reg2 reg3      | reg1 = reg2 - reg3             | 000000     | 100010     |
| and         | R-type | Logical AND         | and reg1 reg2 reg3      | reg1 = reg2 & reg3             | 000000     | 100100     |
| or          | R-type | Logical OR          | or reg1 reg2 reg3       | reg1 = reg2 \| reg3            | 000000     | 100101     |
| slt         | R-type | Set less than       | slt reg1 reg2 reg3      | reg1 = (reg2 < reg3 ? 1: 0)    | 000000     | 101010     |
| jr          | R-type | Jump register       | jr reg1                 | PC = reg1                      | 000000     | 001000     |
| j           | J-type | Jump                | j address               | PC = address                   | 000010     |            |
| jal         | J-type | Jump and link       | jal address             | RA = PC, PC = address          | 000011     |            |
| beq         | I-type | Banch on equal      | beq reg1 reg2 offset    | if (reg1 == reg2) PC += offset | 000100     |            |
| addi        | I-type | Add immediate       | addi reg1 reg2 constant | reg1 = reg2 + constant         | 001000     |            |
| lw          | I-type | Load Word           | lw reg1 reg2 offset     | reg1 = M[reg2+offset]          | 100011     |            |
| sw          | I-type | Store Word          | sw reg1 reg2 offset     | M[reg2+offset] = reg1          | 101011     |            |

The “input format” given above refers to the format of the assembly instructions we’ll parse, convert to machine code, and then process through our circuit. We implement help functions to get the op codes (6-bit), register (5-bit, except J-type), and function codes (6-bit, only for R-type), and then integrate them to parse the input instructions into their 32-bit binary machine code representation. For this project, we only consider these 9 registers below:

| Register | Binary | Use                     |
|----------|--------|-------------------------|
| zero     | 00000  | Constant value 0        |
| v0       | 00010  | Return value register 0 |
| a0       | 00100  | Argument register 0     |
| t0       | 01000  | Temporary register 0    |
| t1       | 01001  | Temporary register 1    |
| s0       | 10000  | Saved register 0        |
| s1       | 10001  | Saved register 1        |
| sp       | 11101  | Stack pointer           |
| ra       | 11111  | Return address          |  

## Datapath Design

In *Computer Organization and Design, 5th edition* by David A. Patterson and John L. Hennessy, it provides us a datapath design in Chapter 4.4, A Simple Implementation Scheme, that supports the instructions we need except jumpy and link (jal) and jump register (jr). We provide some modifications base on the design from that book to support all the instructions above. Our whole design of datapath is shown below.

![Modified_Datapath](https://user-images.githubusercontent.com/74130971/182014394-f1fcf8b2-0b18-4643-81fb-797841ff6478.png)

To implement jal, we change two Mux-2 near Registers Memory and Data Memory into Mux-4, expand the control signal MemtoReg and RegDst to 2-bit, and add some necessary datapath. To implement jr, we add a new 1-bit control signal called JMPReg that represents if the current instruction is jr or not. There are two ways to integrate the new signal JMPReg with the original datapath design, shown in figure below: add a new Mux-2 and treat JMPReg and Jump as two separate 1-bit control signal, or change the Mux-2 into Mux-4 and combine Jump, JMPReg into a 2-bit signal. We choose to use the first one in our design. 

![jr_design](https://user-images.githubusercontent.com/74130971/182014050-765d4840-8afe-456e-8b7b-dcb9345e6ef9.jpg)

## Control Design

Since we modify some of the control signal, a new truth table for Control and ALU Control is shown below. 

<img width="1340" alt="Screen Shot 2022-07-31 at 04 01 16" src="https://user-images.githubusercontent.com/74130971/182016131-8a0516cb-3e49-43e1-8c2a-3b17b8ffb591.png">

We can implement the Control by sum of products (SOP), and the ALU Control by following the design below. 

<img width="824" alt="Screen Shot 2022-07-31 at 04 10 11" src="https://user-images.githubusercontent.com/74130971/182016527-4c698d91-d19d-42ab-a2f5-c8c734974d7e.png">

## Example Input and Output

Example 1:
```
$ cat example1.txt
addi t0 zero 12
addi t1 zero 13
add s0 t0 t1
sub s1 t0 t1
and a0 t0 t1
or v0 t0 t1

$ gcc -c project.c
$ gcc project.o
$ ./a.out < example1.txt 
PC: 0
Instruction: 00100000000010000000000000001100
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Register: 0 0 0 0 0 0 0 0 12 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32 0 0
PC: 1
Instruction: 00100000000010010000000000001101
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Register: 0 0 0 0 0 0 0 0 12 13 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32 0 0
PC: 2
Instruction: 00000001000010011000000000100000
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Register: 0 0 0 0 0 0 0 0 12 13 0 0 0 0 0 0 25 0 0 0 0 0 0 0 0 0 0 0 0 32 0 0
PC: 3
Instruction: 00000001000010011000100000100010
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Register: 0 0 0 0 0 0 0 0 12 13 0 0 0 0 0 0 25 -1 0 0 0 0 0 0 0 0 0 0 0 32 0 0
PC: 4
Instruction: 00000001000010010010000000100100
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Register: 0 0 0 0 12 0 0 0 12 13 0 0 0 0 0 0 25 -1 0 0 0 0 0 0 0 0 0 0 0 32 0 0
PC: 5
Instruction: 00000001000010010001000000100101
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Register: 0 0 13 0 12 0 0 0 12 13 0 0 0 0 0 0 25 -1 0 0 0 0 0 0 0 0 0 0 0 32 0 0
```

Example 2:
```
$ cat example2.txt
addi t0 zero 25
add t1 t0 t0
addi sp sp -2
sw t0 sp 0
sw t1 sp 1
lw s0 sp 0
lw s1 sp 1
addi sp sp 2

$ gcc -c project.c
$ gcc project.o
$ ./a.out < example2.txt
PC: 0
Instruction: 00100000000010000000000000011001
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Register: 0 0 0 0 0 0 0 0 25 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32 0 0
PC: 1
Instruction: 00000001000010000100100000100000
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Register: 0 0 0 0 0 0 0 0 25 50 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 32 0 0
PC: 2
Instruction: 00100011101111011111111111111110
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Register: 0 0 0 0 0 0 0 0 25 50 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 30 0 0
PC: 3
Instruction: 10101111101010000000000000000000
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 25 0
Register: 0 0 0 0 0 0 0 0 25 50 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 30 0 0
PC: 4
Instruction: 10101111101010010000000000000001
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 25 50
Register: 0 0 0 0 0 0 0 0 25 50 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 30 0 0
PC: 5
Instruction: 10001111101100000000000000000000
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 25 50
Register: 0 0 0 0 0 0 0 0 25 50 0 0 0 0 0 0 25 0 0 0 0 0 0 0 0 0 0 0 0 30 0 0
PC: 6
Instruction: 10001111101100010000000000000001
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 25 50
Register: 0 0 0 0 0 0 0 0 25 50 0 0 0 0 0 0 25 50 0 0 0 0 0 0 0 0 0 0 0 30 0 0
PC: 7
Instruction: 00100011101111010000000000000010
Data: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 25 50
Register: 0 0 0 0 0 0 0 0 25 50 0 0 0 0 0 0 25 50 0 0 0 0 0 0 0 0 0 0 0 32 0 0
```

## Reference

1. Rensselaer Polytechnic Institute, Fall 2021, CSCI 2500 Computer Organization by professor Konstantin Kuzmin, *MIPS Processor in C*. 
2. *Computer Organization and Design, 5th edition* by David A. Patterson and John L. Hennessy, Chapter 4.4 A Simple Implementation Scheme.
3. Rochester Institute of Technology, EECC 550 Computer Organization by Dr. Muhammad Shaaban, *Adding Support for jal to Single Cycle Datapath* [[link](http://meseec.ce.rit.edu/eecc550-winter2005/550-chapter5-exercises.pdf)]
