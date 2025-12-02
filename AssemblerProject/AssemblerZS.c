/*
	Name: Zach Schwartz

	Project Title: Assembler Project

	Date (Last Updated): 11/30/2025

	Description:
		Code for the first 7 parts of the assembler project.
		The compiler reads in assembly code from a file, and
		can now handle moving values between memory and registers,
		adding values from memory and registers, outputting,
		halting, printing a memory dump, all of the jumps,
		comparing values, and getting user inputed values
		from the console.
*/

#define _CRT_SECURE_NO_WARNINGS // allows the use of deprecated code

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char ASM_FILE_NAME[ ] = "Assembler8ZS.asm"; // name of linked assembly file

#define MAX 150			// strlen of simulators memory can be changed
#define COL 7			// number of columns for output
#define LINE_SIZE 100	// For c-strings

// operand types, registers, and other
#define AXREG 0
#define BXREG 1
#define CXREG 2
#define DXREG 3
#define BXADDR 4
#define BXPLUS 5
#define ADDRESS 6
#define CONSTANT 7

// commands
#define RET 3
#define FUN 4
#define HALT 5
#define GET 6
#define PUT 7
#define JE 8
#define JNE 9
#define JB 10
#define JBE 11
#define JA 12
#define JAE 13
#define JMP 14
#define CMP 96
#define SUB 128
#define ADD 160
#define MOVREG 192
#define MOVMEM 224

// boolean
#define TRUE 1
#define FALSE 0

// list of all operand types
enum operType { reg, mem, constant, arrayBx, arrayBxPlus, none };

// registers and flag add the stack pointer
struct Registers
{
	int AX;
	int BX;
	int CX;
	int DX;
	int flag;
	int pointer;
}regis;

// global variables
typedef short int Memory;    // sets the type of memory to short int 
Memory memory[ MAX ] = { 0 };  // global variable the memory of the virtual machine
Memory address;              // global variable the current address in the virtual machine

// function prototypes
void runMachineCode( ); // executes the machine code
void splitCommand( char line[ ], char part1[ ], char part2[ ], char part3[ ] ); // splits line of asm into it's three parts
void splitFunction( char line[ LINE_SIZE ] ); // splits function specific line of asm into it's nessecary parts
void convertToMachineCode( FILE* fin ); // converts a single line of ASM to machine code
void assembler( ); // converts the entire ASM file and stores it in memory
void printMemoryDump( ); // prints memory with commands represented as integes
int pop( );
void push( int value );

// helper functions prototypes 
int convertToNumber( char line[ ], int start ); // converts a sub-string to an int
int whichOperand( char operand[ ] ); // returns the number of the letter registar
void changeToLowerCase( char line[ ] ); // changes each character to lower case
void printMemoryDumpHex( ); // prints memory in hexedecimal
void putValue( int operand, int value ); // puts a value into the correct register based on the operand
Memory getValue( Memory operand ); // gets a value from the correct register based on the operand

int main( )
{
	assembler( );
	runMachineCode( );
	printMemoryDump( ); // displays memory with final values
	printf( "\n" );
	system( "pause" );
	return 0;
}

/********************   assembler   ***********************
changes the assembly code to machine code and places the
commands into the memory.
parameters: none
return value: none
-----------------------------------------------------------*/
void assembler( )
{
	address = 0;
	FILE* fin;   // File pointer for reading in the assembly code.			
	fopen_s( &fin, ASM_FILE_NAME, "r" );
	if ( fin == NULL )
	{
		printf( "Error, file didn't open\n\nExiting program...\n\n" );
		system( "pause" );
		exit( 1 );
	}
	for ( int i = 0; i < MAX && !feof( fin ); i++ )
	{
		convertToMachineCode( fin );
	}
}

/********************   convertToMachineCode   ***********************
Converts a single line of ASM to machine code

takens in a file pointer to the asm file, reads one line and splits it
into its parts then converts it to machine code based on the command and operands

parameters: FILE* fin - pointer to the asm file
return value: none
---------------------------------------------------------------------*/
void convertToMachineCode( FILE* fin )
{
	char line[ LINE_SIZE ];		// full command
	char part1[ LINE_SIZE ];	// the asm commmand
	char part2[ LINE_SIZE ] = "";// the two operands, could be empty
	char part3[ LINE_SIZE ] = "";
	int machineCode = 0;	// One line of converted asm code from the file
	Memory operand3 = 0;  // the second operand, could be empty

	fgets( line, LINE_SIZE, fin );		// Takes one line from the asm file
	changeToLowerCase( line );


	// Empty Lines
	if ( line[ 0 ] == '\n' || line[ 0 ] == '\0' ) // comment or blank line
	{
		address++;
		return;
	}
	else if ( isdigit( line[ 0 ] ) || line[ 0 ] == '-' )
	{
		memory[ address ] = convertToNumber( line, 0 ); // puts the constant value into the next memory address
		address++;
		return;
	}

	// Handling For Commented Line
	else if ( line[ 0 ] == ';' )
	{
		return;
	}

	// One Part Commands
	else if ( line[ 0 ] == 'h' )  //halt
	{
		memory[ address ] = HALT;
		address++;
		return;
	}
	else if ( line[ 0 ] == 'p' )
	{
		machineCode = PUT;
		memory[ address ] = machineCode;
		address++;
		return;
	}
	else if ( line[ 0 ] == 'g' )
	{
		machineCode = GET;
		memory[ address ] = machineCode;
		address++;
		return;
	}
	else if ( line[ 0 ] == 'r' )
	{
		machineCode = RET;
		memory[ address ] = machineCode;
		address++;
		return;
	}
	else if ( line[ 0 ] == 'f' )
	{
		machineCode = FUN;
		memory[ address ] = machineCode;
		address++; // move to next memory address
		splitFunction( line );
		address++; // leave one blank memory address for function return value
		return;
	}

	splitCommand( line, part1, part2, part3 ); // split larger, non-function commands

	// Two Part Commands
	if ( part1[ 0 ] == 'j' )
	{
		if ( part1[ 1 ] == 'e' )
		{
			machineCode = JE;
		}
		else if ( part1[ 1 ] == 'n' )
		{
			machineCode = JNE;
		}
		else if ( part1[ 1 ] == 'm' )
		{
			machineCode = JMP;
		}
		else if ( part1[ 1 ] == 'b' )
		{
			if ( part1[ 2 ] == 'e' )
			{
				machineCode = JBE;
			}
			else
			{
				machineCode = JB;
			}
		}
		else if ( part1[ 1 ] == 'a' )
		{
			if ( part1[ 2 ] == 'e' )
			{
				machineCode = JAE;
			}
			else
			{
				machineCode = JA;
			}
		}
		memory[ address ] = machineCode;
		address++;
		memory[ address ] = convertToNumber( part2, 0 );
		address++;
		return;
	}

	// Three Part Commands
	else if ( part1[ 0 ] == 'm' )  //move into a register
	{
		if ( part2[ 0 ] == '[' )
		{
			machineCode = MOVMEM;
			char temp[ LINE_SIZE ];
			strcpy( temp, part2 ); // copies part2 without the '['
			strcpy( part2, part3 ); // copies part2 without the '['
			strcpy( part3, temp ); // copies part2 without the '['
		}
		else
		{
			machineCode = MOVREG;
		}
	}
	else if ( part1[ 0 ] == 'a' )  //move into a register
	{
		machineCode = ADD;
	}
	else if ( part1[ 0 ] == 'c' )
	{
		machineCode = CMP;
	}

	// Ask Professor About This One
	operand3 = whichOperand( part3 );
	machineCode = machineCode | ( whichOperand( part2 ) << 3 ); // bitshifts 3 to the left
	machineCode = machineCode | operand3;
	memory[ address ] = machineCode;
	address++;

	if ( operand3 == ADDRESS || operand3 == CONSTANT )
	{
		memory[ address ] = convertToNumber( part3, 0 ); // puts the constant value into the next memory address
		address++;
	}
	else if ( operand3 == BXPLUS )
	{
		memory[ address ] = convertToNumber( part3, 4 );
		address++;
	}

	printMemoryDump( );
}

/********************   runMachineCode   ***********************
Executes the machine code that is in memory, the virtual machine

Actually executes the machine code based on the commands and operands stored in memory
and interpreted by the convertToMachineCode function. Uses bit masking to get the parts of each command.

parameters: none
return value: none
-----------------------------------------------------------*/
void runMachineCode( )
{
	Memory mask1 = 224;   //111 00 000
	Memory mask2 = 24;    //000 11 000 
	Memory mask3 = 7;	  //000 00 111
	Memory part1, part2, part3; //command, operand1, 
	int value1, value2;   //the actual values in the registers or constants
	regis.pointer = MAX - 1; // hardcodes stack to end of memory

	address = 0;
	Memory fullCommand = memory[ address ];
	address++;
	while ( fullCommand != HALT )
	{
		part1 = fullCommand & mask1;
		part2 = ( fullCommand & mask2 ) >> 3;
		part3 = fullCommand & mask3;

		// One Part Commands
		if ( fullCommand == PUT )
		{
			printf( "\t\t\t\t  PUT - AX is: %d\n\n", regis.AX );
		}

		if ( fullCommand == GET )
		{
			printf( "\nEnter a number: " );
			scanf( "%d", &value1 ); // pass by reference (scanf wants the reference, not the value)
			putValue( AXREG, value1 ); // pass by value
		}

		// Two Part Commands (Jumps)
		else if ( fullCommand >= JE && fullCommand <= JMP )
		{
			Memory targetAddr = memory[ address ];
			address++;

			if ( fullCommand == JE && regis.flag == 0 )
			{
				address = targetAddr;
			}
			else if ( fullCommand == JNE && regis.flag != 0 )
			{
				address = targetAddr;
			}
			else if ( fullCommand == JB && regis.flag == -1 )
			{
				address = targetAddr;
			}
			else if ( fullCommand == JBE && regis.flag < 1 )
			{
				address = targetAddr;
			}
			else if ( fullCommand == JA && regis.flag == 1 )
			{
				address = targetAddr;
			}
			else if ( fullCommand == JAE && regis.flag > -1 )
			{
				address = targetAddr;
			}
			else if ( fullCommand == JMP )
			{
				address = targetAddr;
			}
		}

		else if ( fullCommand == FUN )
		{
			int funcStartAddr = memory[ address ];
			int paramAddr = address + 1;
			int numParams = memory[ paramAddr ];
			int returnAddr = address + numParams + 3; // +3 to skip function, location, and parameters
			push( returnAddr );
			push( regis.flag );
			push( regis.DX );
			push( regis.CX );
			push( regis.BX );
			push( regis.AX );
			memory[ funcStartAddr - 1 ] = paramAddr;
			address = funcStartAddr;
		}

		else if ( fullCommand == RET )
		{
			int returnValue = getValue( AXREG );
			regis.AX = pop( );
			regis.BX = pop( );
			regis.CX = pop( );
			regis.DX = pop( );
			regis.flag = pop( );
			int returnAddr = pop( );
			memory[ returnAddr - 1 ] = returnValue;
			address = returnAddr;

		}

		// Three Part Commands
		else if ( part1 == MOVREG )
		{
			value1 = getValue( part3 );
			putValue( part2, value1 );
		}
		else if ( part1 == MOVMEM )
		{
			value1 = getValue( part2 );
			putValue( ADDRESS, value1 );
		}
		else if ( part1 == ADD )
		{
			// get the values from part2 and part3
			value1 = getValue( part2 );
			value2 = getValue( part3 );
			// add the values together
			value1 = value1 + value2;
			// put the value into the register specified by part2
			putValue( part2, value1 );
		}
		else if ( part1 == CMP )
		{
			value1 = getValue( part2 );
			value2 = getValue( part3 );
			if ( value1 == value2 )
			{
				regis.flag = 0;
			}
			else if ( value1 > value2 )
			{
				regis.flag = 1;
			}
			else
			{
				regis.flag = -1;
			}
		}

		fullCommand = memory[ address ];  //the next command
		address++;
		//debugging, comment out when you don't need it
		printMemoryDump( );
	}
}

/********************   splitCommand   ***********************
splits a line of asm into it's parts

Takes in a line of assembly code as a character array and splits it into three seperate arrays
part1 - the command
part2 - the first operand
part3 - the second operand

parameters: char line[] - the full line of assembly code
			char part1[] - the command
			char part2[] - the first operand
			char part3[] - the second operand
return values: nothing
-----------------------------------------------------------*/
void splitCommand( char line[ ], char part1[ ], char part2[ ], char part3[ ] )
{
	int index = 0;           // the character location in the string line
	int index2 = 0;          // character location in new string ie the parts

	//moves the first set of characters from line into instruction
	while ( line[ index ] != ' ' && line[ index ] != '\0' && line[ index ] != '\n' )
	{
		part1[ index2 ] = line[ index ];
		index++;
		index2++;
	}
	part1[ index2 ] = '\0';				// add the string stopper
	index2 = 0;

	if ( line[ index ] == '\0' )  //no space, command has no other parts
	{
		strcpy( part2, "\0" );
		strcpy( part3, "\0" );
	}
	else
	{
		if ( line[ index ] != '/0' && index + 1 < LINE_SIZE )
		{
			if ( line[ index ] == ' ' && line[ index + 1 ] != ' ' && line[ index + 1 ] != "\n" )
			{
				index++;  //skips the space
				while ( ( line[ index ] != ' ' && line[ index ] != '\0' && line[ index ] != '\n' ) && index < LINE_SIZE && index2 < LINE_SIZE - 1 )
				{
					part2[ index2 ] = line[ index ];
					index++;
					index2++;
				}
			}
		}
		part2[ index2 ] = '\0'; // add the string stopper
		index2 = 0;

		if ( line[ index ] != '/0' && index + 1 < LINE_SIZE )
		{
			if ( line[ index ] == ' ' && line[ index + 1 ] != ' ' && line[ index + 1 ] != "\n" )
			{
				index++;  //skips the space
				while ( ( line[ index ] != ' ' && line[ index ] != '\0' && line[ index ] != '\n' ) && index < LINE_SIZE && index2 < LINE_SIZE - 1 )
				{
					part3[ index2 ] = line[ index ];
					index++;
					index2++;
				}
			}
		}
		part3[ index2 ] = '\0'; // add the string stopper
		printf( "Command = %s %s %s\n\n", part1, part2, part3 );
	}
}

/********************   splitFunction   ***********************
splits a function line of asm into multiple parts

Takes in a function line of assembly code as a character array and splits it into multiple seperate arrays

parameters: char line[] - the full line of assembly code

return values: nothing
-----------------------------------------------------------*/
void splitFunction( char line[ LINE_SIZE ] )
{
	/*
	* 1 - get fun, memory space, and number of parameters
	* 2 - get number of parameters
	* 3 - get parameters
	* 4 -
	*/
	Memory machineCode = 0; // initialize machineCode variable holder
	int loopCounter = 0; // initialize loop counter
	int index = 0;

	machineCode = convertToNumber( line, 4 );
	memory[ address ] = machineCode;
	address++;
	index = 4; // moves index to the memory location

	while ( line[ index ] != ' ' )
	{
		index++;
	}

	int numParams = convertToNumber( line, index ); // auto skips space to get the number of parameters for the function

	machineCode = numParams;
	memory[ address ] = machineCode; // stores num of params in memory
	address++;
	index++;

	while ( loopCounter != numParams ) // loop through all parameters
	{
		while ( line[ index ] != ' ' && line[ index ] != '\n' && line[ index ] != '\0' ) // get to right before next whitespace
		{
			index++;
		}
		index++; // skips whitespace
		machineCode = convertToNumber( line, index );
		memory[ address ] = machineCode;
		address++;
		loopCounter++;
	}
	return;
}

/*********************************************************************************
/****************************   HELPER FUNCTIONS   *******************************
/*********************************************************************************

/****************************   printMemoryDump   ********************************
prints memory by number
MAX is the amount of elements in the memory array (Vicki used 100)
COL is the number of columns that are to be displayed (Vicki used 7)
parameters: none
return value: none
---------------------------------------------------------------------------------*/
void printMemoryDump( )
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	for ( int row = 0; row < numRows; row++ )
	{
		location = row;
		for ( int column = 0; location < MAX && column < COL; column++ )
		{
			if ( !( numRows - 1 == row && carryOver - 1 < column ) )
			{
				printf( "%5d.%5d", location, memory[ location ] );
				location += ( numRows - ( carryOver - 1 < column ) );
			}
		}
		printf( "\n" );
	}
	printf( "\nAX:%d\t", regis.AX );
	printf( "BX:%d\t", regis.BX );
	printf( "CX:%d\t", regis.CX );
	printf( "DX:%d\n\n", regis.DX );
	printf( "Address: %d\n", address );
	printf( "\nFlag: %d\n\n", regis.flag );
}


/*********************   printMemoryDumpHex   ********************
Prints memory in
*parameters: none
*return value: none
----------------------------------------------------------------*/
void printMemoryDumpHex( )
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	for ( int row = 0; row < numRows; row++ )
	{
		location = row;
		for ( int column = 0; location < MAX && column < COL; column++ )
		{
			if ( !( numRows - 1 == row && carryOver - 1 < column ) )
			{
				printf( "%5d.%3x", location, memory[ location ] );
				location += ( numRows - ( carryOver - 1 < column ) );
			}
		}
		printf( "\n" );
	}
	printf( "\nAX:%d\t", regis.AX );
	printf( "BX:%d\t", regis.BX );
	printf( "CX:%d\t", regis.CX );
	printf( "DX:%d\n", regis.DX );
	printf( "Address: %d\n", address );
	printf( "Flag: %d\n\n", regis.flag );
}

/*****************************************************************************/
/*********** helper function for converting to machine code ******************/
/*****************************************************************************/

/*********************   whichOperand   ***************************
/* changes the letter of the register to a number parameters:
/* letter - the first letter of the operand, register, number, [
/* return value - the number of the register
/*--------------------------------------------------------------*/
int whichOperand( char operand[ ] )
{
	char letter = operand[ 0 ];
	if ( letter == 'a' )
	{
		return AXREG;
	}
	else if ( letter == 'b' )
	{
		return BXREG;
	}
	else if ( letter == 'c' )
	{
		return CXREG;
	}
	else if ( letter == 'd' )
	{
		return DXREG;
	}
	else if ( isdigit( letter ) )
	{
		return CONSTANT;
	}
	else if ( letter == '[' )
	{
		if ( operand[ 1 ] == 'b' && operand[ 2 ] == 'x' )
		{
			if ( operand[ 3 ] == '+' )
			{
				return BXPLUS;
			}
			return BXADDR;
		}
		return ADDRESS;
	}
	return -1;  //something went wrong if -1 is returned
}

/*********************ConvertToNumber ********************/
/*  takes in a line and converts digits to a integer
/*  line - is the string of assembly code to convert
/*  start - is the location where the line is being converted,
/*--------------------------------------------------------------*/
int convertToNumber( char line[ ], int start )
{
	int value; // is the integer value of the digits in the code
	char number[ 16 ];  //just the digits
	int negative = 0;  //negative or positive number

	int i = 0;
	while ( line[ start ] == '[' || line[ start ] == ' ' )
	{
		start++;
	}
	if ( line[ start ] == '-' )
	{
		start++;
		negative = 1;
	}
	while ( i < 16 && isdigit( line[ start ] ) )
	{
		number[ i ] = line[ start ];
		i++;
		start++;
	}
	number[ i ] = '\0';
	value = atoi( number );
	if ( negative == 1 )
	{
		value = -value;
	}
	return value;
}  //end convertToNumber

/*********************   changeToLowerCase   ********************
Changes each character to lower case
*parameters:
* line - the string that was entered the line is completely changed to lower case
* return value: none
----------------------------------------------------------------*/
void changeToLowerCase( char line[ ] )
{
	int index = 0;
	while ( index < strlen( line ) )
	{
		line[ index ] = tolower( line[ index ] );
		index++;
	}
}

/********************   getValue   ***********************
Function to take in an operand, and return one of the memory registers.
parameters: Memory operand - the operand to get the value from
return value: Returns register value, contant value, or memory value based on operand
-----------------------------------------------------------*/
Memory getValue( Memory operand )
{
	int value;
	switch ( operand )
	{
		case AXREG:
			return regis.AX;
			break;
		case BXREG:
			return regis.BX;
			break;
		case CXREG:
			return regis.CX;
			break;
		case DXREG:
			return regis.DX;
			break;
		case CONSTANT:
			value = memory[ address ];
			address++;
			return value;
			break;
		case ADDRESS:
			value = memory[ address ];
			address++;
			return memory[ value ];
			break;
		case BXADDR:
			return memory[ regis.BX ];
			break;
		case BXPLUS:
			value = memory[ address ];
			address++;
			return memory[ regis.BX + value ];
			break;
	}
	return -1; // error case
}

/********************   putValue   ***********************
Function to take in an operand and a value, and store the value in the correct register based on the operand.
parameters: The numberical value of the operand, and the int value to store
return value: None
-----------------------------------------------------------*/
void putValue( int operand, int value )
{
	if ( operand == AXREG )
	{
		regis.AX = value;
	}
	else if ( operand == BXREG )
	{
		regis.BX = value;
	}
	else if ( operand == CXREG )
	{
		regis.CX = value;
	}
	else if ( operand == DXREG )
	{
		regis.DX = value;
	}
	else if ( operand == ADDRESS )
	{
		int addr;
		addr = memory[ address ];
		address++;
		memory[ addr ] = value;
	}
	else if ( operand == BXADDR )
	{
		memory[ regis.BX ] = value;
	}
	else if ( operand == BXPLUS )
	{
		int distance = memory[ address ];
		address++;
		memory[ regis.BX + distance ] = value;
	}
	else
	{
		printf( "Error, invalid operand" ); // error case
	}
}

/********************   pop   ***********************
Function to remove and return the most recently added value to the register variable

parameters: None
return value: The removed item / value
-----------------------------------------------------------*/
int pop( )
{
	regis.pointer++; // total depth
	return memory[ regis.pointer ];
}

/********************   push   ***********************
Function to add a value to the register variable

parameters: The value to add
return value: None
-----------------------------------------------------------*/
void push( int value )
{
	memory[ regis.pointer ] = value;
	regis.pointer--; // go backwards because stack is stored at end of memory location
}

// List Of Problems (Head Bangers) - Anything that takes more than a few minutes to figure out
// 1. For assembler 4, when getting and moving code from memory the code compiling and the lines changing got me stuck
// 2. Moving from memory to a different memory addres is not working at the moment
// 3. Oh my god functions kicked my butt
// 4. I forgot to add a conditional to catch BXPLUS so the bounds were not working for Assembler 8 code