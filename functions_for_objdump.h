#ifndef FUNCTIONS_FOR_OBJDUMP_H
#define FUNCTIONS_FOR_OBJDUMP_H

#define BYTE unsigned char

#define MAX_FUN (8)
#define MAX_NUM_INSTRUCTION (32 * 8)
#define MAX_INSTRUTION_LEN (3)

#include <stdio.h>

// Record every part of assembly language
typedef struct Assembly{

    int type_part;          // 1:padding, 2:func_label, 3:instructions
    int val_part;           // Decimal representation of func_label

    int val_instruction;    // Decimal representation of instructions/operations
    int type_a;             // type of A      eg. MOV TYPE_A VAL_A TYPE_B VAL_B
    int val_a;              // value of A     eg. MOV TYPE_A VAL_A TYPE_B VAL_B
    int type_b;             // type of B      eg. MOV TYPE_A VAL_A TYPE_B VAL_B
    int val_b;              // type of B      eg. MOV TYPE_A VAL_A TYPE_B VAL_B
    
}Assembly;

// Map the value to letters in order of apperance
typedef struct MapForCharacters
{
    BYTE set[32];           // If 00110 is the first apperance then set[0] = 00110.
    int count;              // record how many stack symols appears in this stack frame
}Map;

/*
    Transfer the number into ASCII of letters.
    @param int num: the decimal representation
    @return the ASCII of letter
*/
int stackSymbol(int num);

/*
    Print part of the assembly language after parsing the machine code
    @param int print_type: 0 value, 1 register, 2 stack symbol, 3 pointer
    @param Assembly* assembly: all assembly code
*/
void printAssembly(int print_type, Assembly* assembly);

/*
    Get continuously bits from one byte
    @param BYTE original: the source byte
    @param int start_offset: the start offset of bits we want get(count from the end of bytes)
    @param int end_offset: the end offset of bits we want get(count from the end of bytes)
    @return the bits we get
*/
BYTE getBitsFromByte(BYTE original, int start_offset, int end_offset);

/*
    Get continuously bits from two continuously bytes [byte 1 : byte 2]
    @param BYTE* all_bytes: the source bytes
    @param BYTE index1: the index of first byte
    @param BYTE index2: the index of second byte
    @param int len: the len of bits we want get
    @param int start: the start offset from the second byte (count from the end)
    @return the bits we get 
*/
BYTE getBitsFromTwoBytes(BYTE* all_bytes, BYTE index1, BYTE index2, int len, int start);

/*
    get how many bits of one type will hold. eg. VAL(00) will hold 8 bits
    @param int type: the type 
*/
int bitsOfValue(int type);

/*
    Check the start offset, if it exceeds 7, since we count from the end of one byte, the
    last index of one byte is 7, so we need go to next byte
    @param int* start_offset_ptr: the start offset pointer
    @param int* bytes_index_ptr: the current byte pointer
*/
void checkOffset(int* start_offset_ptr, int* bytes_index_ptr);

/*
    Since assembly array stores all the assembly information, we set every element in this array according to machine code
    @param int type_val: the value of operations
    @param int num_of_types: the number of types for this operation
    @param BYTE* all_bytes_ptr: the pointer for all bytes
    @param int* bytes_index: the pointer for current byte
    @param int* start_offset: the pointer for start offset
    @param Assembly* assembly: the pointer for assembly array
*/
void setAssembly(int type_val, int num_of_types, BYTE* all_bytes_ptr, int* bytes_index, int* start_offset, Assembly* assembly);

/*
    Map the value to letters in order of apperance in differnt functions
    @param Map* maps: maps for differnt functions
    @param int func_label: the number of function func_label
    @param int val: the value in machine code
*/
void setMapForCharacters(Map* maps, int func_label, int val);

/*
    Reset the value for stack symbol in order. eg A is 0, B is 1......
    @param Map* maps: maps for differnt functions
    @param Assembly* assembly: all assembly 
    @param int len: the length for assembly. How many operations(including function label)
*/
void resetStackSymbolVal(Map* maps, Assembly* assembly, int len);

/*
    parse machine code into assembly
    @param Assembly* assembly: pointer to start address of assembly array
    @param int* count: the number of bytes
    @param int num_of_argu: num_of_arguments
    @param char **argument: *argv[] in int main()
    @param int printed: if it is 1, print the assembly, else, not print.
*/
int parseMachineCode(Assembly * assembly, int* count, int num_of_argu, char **argument, int printed);





















#endif
