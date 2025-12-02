# Computer Architecture & Assemble Language Assembler Project
This GitHub repository contains the code for my CSC2025 Assembler Project. The goal of this project is to write an assembler / compiler in C to convert and subsequently run code written in assembly language. All parts are tested using a seperate ASM file, with one main ASM final as a final test.

## Part 1 / 2
Create code that will take a line of ASM code and break it into the three parts. The command, and one or two operands. Then, write the C code that converts the ASM into binary and stores it into memory. It will move a constant into a register and moves numbers from register to register.

Commands Added:
* MOV register constant
* MOV register register
* HALT

## Part 3
Write the code that will add two numbers and output the result. The numbers can be either in registers or memory.

Commands Added:
* ADD register register
* ADD register constant
* PUT

## Part 4
Create code that can store the sum of two values into a memory location.

Commands Added:
* MOV address register

## Part 5
Write code that can access two values stored in memory locations, add them together, and place the result in a new memory location.

Commands Added:
* MOV register address
* MOV address register
* Blank Line Handling
* Single Number Line Handling

## Part 6
Write code that outputs the larger of two numbers.

Commands Added:
* CMP register register
* CMP register constant
* CMP register address
* JE / JNE / JB / JBE / JA / JAE / JMP

## Part 7
Write code that will add up to 10 user-inputed numbers using a loop.

Commands Added:
* GET

## Part 8
Write the code so that the assembler can handle functions, parameters, and return values.

Commands Added:
* FUN
* RET
* command register [BX]
* MOV [BX] register
* command register [BX+###]
* MOV [BX+###] register

## Part 9
Modify the ASM file for part 8 to pass the return value by reference to a memory location, instead of directly returning it to the original memory location.

*No new commands implemented.*
