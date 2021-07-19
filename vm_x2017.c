#include "functions_for_objdump.h"

/*
    Set the value in stack frame
    @param BYTE* sf_bp: the pointer to current stack bottom
    @param BYTE* sf_tp: the pointer to current stack top
    @param int symbol: the value for stack symbol (0:A, 1:B ......)
    @param int val: the value which will be stored in stack 
    @param BYTE *RAM: RAM
*/
void setValueInStackFrame(BYTE* sf_bp, BYTE* sf_tp, int symbol, int val, BYTE *RAM){
    int offset = symbol + 2; 
    int address = *sf_bp + offset;
    RAM[address] = val;

    if (address > *sf_tp){
        *sf_tp = address;
    }
}

/*
    Get the address in stack symbol (current stack frame)
    @param BYTE* sf_bp: the pointer to current stack bottom
    @param int symbol: the value for stack symbol (0:A, 1:B ......)
    @return the address value in stack symbol
*/
int ptr(BYTE* sf_bp, int symbol, BYTE *RAM){
    int offset = symbol + 2; 
    int address = *sf_bp + offset;
    address = RAM[address];// get the address
    return address;
}


/*
    Search the index of one function label in structure assembly array
    @param Assembly* assembly: assembly array
    @param int func_label: the number of function label 
    @param int len: the length of all assembly
*/
int searchIndexOfFunctionLabel(Assembly* assembly, int func_label, int len){
    for (int i = 0; i < len; i++){
        if (assembly[i].type_part == 2){
            if (assembly[i].val_part == func_label){
                return i;
            }
        }
    }
    return -1; // not found
}


/* 
For my stack

    address                                           
        2   |                                   |     
        1   |                                   |     
        0   |                                   |   
            ------------------------------------
                                                    <----- pointer to the top of current stack frame, sf_tp
                                                    <----- pointer to the bottom of current stack frame , sf_bp 

            initially
                sf_bp = -1;
                sf_tp = -1;


   CALL main function 
        5   |                                   |    
        4   |                                   |    
        3   |  _______________________________  |                   
        2   | A|                             |  |   <----- pointer to the top of current stack frame, sf_tp
        1   |  | return "program counter"    |  |     
        0   |  | sf_bp of previous frame(0)  |  |   <----- pointer to the bottom of current stack frame , sf_bp 
            ------------------------------------     



    MOV STK A VAL 0
    MOV STK B VAL 1
    update the top of current frame

        7   |                                   |
        6   |                                   |
        5   |                                   |
        4   |  _______________________________  |
        3   | B|       0000 0001             |  |   <----- pointer to the top of current stack frame, sf_tp 
        2   | A|       0000 0000             |  |   
        1   |  | return "program counter"    |  |     
        0   |  | sf_bp of previous frame(0)  |  |   <----- pointer to the bottom of current stack frame , sf_bp 
            ------------------------------------    


        CAL VAL 1

        7   |  ______________________________   |
        6   | A|                             |  |   <----- pointer to the top of current stack frame, sf_tp 
        5   |  |   return "program counter"  |  |
        4   |  |_________sf_bp_pre_(0)_______|  |   <----- pointer to the bottom of current stack frame , sf_bp 
        3   | B|                             |  |   
        2   | A|       0000 0101             |  |   
        1   |  | return "program counter"    |  |     
        0   |  | sf_bp of previous frame     |  |   
            ------------------------------------    

        When return set the top into bp-1 and set the bp into RAM[bp] which is the bp of previous stack frame
*/

int main(int argc, char** argv)
{   

    FILE *fp;
    if (argc != 2){
        fprintf(stderr, "Wrong number of arguments.\n");
        return 1;
    }
    
    fp = fopen(argv[1], "rb");

    if (fp == NULL){
        fprintf(stderr, "Fail to open the x2017 binary file.\n");
        return 1;
    }
    
    Assembly assembly_reverse[MAX_NUM_INSTRUCTION + MAX_FUN];
    int count = 0;
    
    parseMachineCode(assembly_reverse, &count, argc, &*argv, 0);
    
    Assembly assembly[MAX_NUM_INSTRUCTION + MAX_FUN];

    int i;
    for (i = count - 1; i >= 0; i--){
        assembly[count - i] = assembly_reverse[i];
    }

    BYTE RAM[256];
    for (int i = 0 ; i < 256; i++){
        RAM[i] = 0;
    }

    BYTE general_reg[4] = {0, 0, 0, 0};
    BYTE reg_4_sf_bp = -1; // bottom of current stack frame
    BYTE reg_5_sf_tp = -1; // top of current stack frame


    BYTE reg_7_pc = 0; // program counter;
    
    int go_to_new_stack_frame = 1;
    int function_label = 0;
    int end = 0;
    
    while (!end){

        if (go_to_new_stack_frame){
            reg_7_pc = searchIndexOfFunctionLabel(assembly, function_label, count);
            go_to_new_stack_frame = 0;

            reg_4_sf_bp = reg_5_sf_tp + 1;
            reg_5_sf_tp = reg_4_sf_bp + 2;
            
        }else{
            while(1){
                // type is operation
                // val 00, reg 1, stk 2, ptr 3

                // we do not need to worry about assembly index overflow since it is unsigned char
                Assembly* instruction = &(assembly[reg_7_pc]);

                if (instruction->type_part == 3){
                    if (instruction->val_instruction == 0){
                        // mov reg 1 val 2
                        if (instruction->type_a == 1 && instruction->type_b == 0){
                            
                            int num_reg = instruction->val_a;
                            if (num_reg == 7){
                                reg_7_pc = instruction->val_b;
                                continue;
                            }else{
                                general_reg[num_reg] = instruction->val_b;
                            }
                        }

                        // mov reg A reg B
                        else if (instruction->type_a == 1 && instruction->type_b == 1){
                            
                            int num_regA = instruction->val_a;
                            int num_regB = instruction->val_b;
                            if (num_regA == 7){
                                reg_7_pc = general_reg[num_regB];
                                continue;
                            }else if (num_regB == 7){
                                general_reg[num_regA] = reg_7_pc;
                            }else{
                                general_reg[num_regA] = general_reg[num_regB];
                            }
                        }

                        // mov reg 1 STK A
                        else if (instruction->type_a == 1 && instruction->type_b == 2){
                            int symbol = instruction->val_b;
                            int num_reg = instruction->val_a;
                            int address = reg_4_sf_bp + symbol + 2;

                            if (num_reg == 7){
                                reg_7_pc = RAM[address];
                                continue;
                            }else{
                                general_reg[num_reg] = RAM[address];
                            }
                            
                        }

                        // mov reg 0 ptr A
                        else if (instruction->type_a == 1 && instruction->type_b == 3){
                            int symbol = instruction->val_b;
                            int num_reg = instruction->val_a;

                            int address = ptr(&reg_4_sf_bp, symbol, RAM);
                            general_reg[num_reg] = RAM[address];
                        
                            if (num_reg == 7){
                                reg_7_pc = RAM[address];
                                continue;
                            }else{
                                general_reg[num_reg] = RAM[address];
                            }
                        }

                        // mov stk A val 8
                        else if (instruction->type_a == 2 && instruction->type_b == 0){
                            setValueInStackFrame(&reg_4_sf_bp, &reg_5_sf_tp, instruction->val_a, instruction->val_b, RAM);
                        }
                        // mov stk A reg 0
                        else if (instruction->type_a == 2 && instruction->type_b == 1){
                            int num_reg = instruction->val_b;
                            int val = 0;
                            if (num_reg == 7){
                                val = reg_7_pc;
                            }else{
                                val = general_reg[num_reg];
                            }
    
                            setValueInStackFrame(&reg_4_sf_bp, &reg_5_sf_tp, instruction->val_a, val, RAM);
                        }

                        // mov stk A stk B
                        else if (instruction->type_a == 2 && instruction->type_b == 2){
                            int symbol = instruction->val_b;
                            int val_B = RAM[reg_4_sf_bp + symbol + 2];
    
                            setValueInStackFrame(&reg_4_sf_bp, &reg_5_sf_tp, instruction->val_a, val_B, RAM);
                        }

                        // mov STK A PTR B
                        else if (instruction->type_a == 2 && instruction->type_b == 3){
                            int symbol = instruction->val_b;
                            int address = ptr(&reg_4_sf_bp, symbol, RAM);
                            
                            setValueInStackFrame(&reg_4_sf_bp, &reg_5_sf_tp, instruction->val_a, RAM[address], RAM);
                        }


                        // mov ptr A val 0
                        else if (instruction->type_a == 3 && instruction->type_b == 0){
                            int symbol = instruction->val_a;
                            int val = instruction->val_b;

                            int address = ptr(&reg_4_sf_bp, symbol, RAM);
                            RAM[address] = val;
                        } 


                        // mov ptr A reg 0 
                        else if (instruction->type_a == 3 && instruction->type_b == 1){
                            int symbol = instruction->val_a;
                            int num_reg = instruction->val_b;

                            int address = ptr(&reg_4_sf_bp, symbol, RAM);
                            
                            if (num_reg == 7){
                                RAM[address] = reg_7_pc;
                            }else{
                                RAM[address] = general_reg[num_reg];
                            }
                        }

                        // mov ptr A stk B
                        else if (instruction->type_a == 3 && instruction->type_b == 2){
                            int symbol_A = instruction->val_a;
                            int symbol_B = instruction->val_b;

                            int address_A = ptr(&reg_4_sf_bp, symbol_A, RAM);
                            int address_B = reg_4_sf_bp + symbol_B + 2;

                            RAM[address_A] = RAM[address_B];
        
                        }

                        // mov ptr A ptr B
                        else if (instruction->type_a == 3 && instruction->type_b == 3){
                            int symbol_A = instruction->val_a;
                            int symbol_B = instruction->val_b;

                            int address_A = ptr(&reg_4_sf_bp, symbol_A, RAM);
                            int address_B = ptr(&reg_4_sf_bp, symbol_B, RAM);


                            RAM[address_A] = RAM[address_B];
                        }

                        else {
                            fprintf(stderr, "MOV types are invalid.\n");
                            return 1;
                        }



                        
                    }else if (instruction->val_instruction == 1){
                        if (instruction->type_a != 0){
                            fprintf(stderr, "CAL type is invalid.\n");
                            return 1;
                        }
                        // call val A （pc）
                        BYTE new_bp = reg_5_sf_tp + 1;
                        // 255 - 36 - 2
                        if (new_bp > 217){
                            fprintf(stderr, "StackOverFlow\n");
                            return 1;
                        }

                        RAM[new_bp] = reg_4_sf_bp;
                        RAM[new_bp + 1] = reg_7_pc + 1;

                        go_to_new_stack_frame = 1;
                        function_label = instruction->val_a;
                        break;


                    }else if (instruction->val_instruction == 2){
                
                        // ret
                        reg_7_pc = RAM[reg_4_sf_bp + 1];
                        

                        reg_5_sf_tp = reg_4_sf_bp - 1;
                        reg_4_sf_bp = RAM[reg_4_sf_bp];

                        if (reg_7_pc == 0){
                            
                            end = 1;
                            break;
                        }

                        continue;
                        

                    }else if (instruction->val_instruction == 3){
                        //[REF A B] - Takes a stack symbol B and stores its corresponding stack address in A
                        // REF REG 0 STK B
                        if (instruction->type_a == 1 && instruction->type_b == 2){
                            int num_reg = instruction->val_a;
                            int symbol = instruction->val_b;

                            int offset = symbol + 2; 
                            int address = reg_4_sf_bp + offset;
                            if (num_reg == 7){
                                fprintf(stderr, "invalid to put address into program counter\n");
                                return 1;
                            }
                            general_reg[num_reg] = address;

                        }
                        // REF STK A STK B
                        else if (instruction->type_a == 2 && instruction->type_b == 2){
                            int symbol_A = instruction->val_a;
                            int symbol_B = instruction->val_b;

                            int address = reg_4_sf_bp + symbol_B + 2;

                            setValueInStackFrame(&reg_4_sf_bp, &reg_5_sf_tp, symbol_A, address, RAM);
                        }
                        
                        // REF PTR A STK B
                        else if (instruction->type_a == 3 && instruction->type_b == 2){
                            int symbol_A = instruction->val_a;
                            int symbol_B = instruction->val_b;

                            int address_A = ptr(&reg_4_sf_bp, symbol_A, RAM);
                            int address_B = reg_4_sf_bp + symbol_B + 2;

                            RAM[address_A] = address_B;

                        }

                        // REF STK A PTR B 
                        else if (instruction->type_a == 2 && instruction->type_b == 3){
                            int symbol_A = instruction->val_a;
                            int symbol_B = instruction->val_b;

                            int address_A = reg_4_sf_bp + symbol_A + 2;
                            int address_B = reg_4_sf_bp + symbol_B + 2;

                            RAM[address_A] = RAM[address_B];

                        }

                        // REF REG 0 PTR B 
                        else if (instruction->type_a == 1 && instruction->type_b == 3){
                            int reg_num = instruction->val_a;
                            int symbol_B = instruction->val_b;

                            int address_B = reg_4_sf_bp + symbol_B + 2;

                            if (reg_num == 7){
                                fprintf(stderr, "invalid to put address into program counter\n");
                                return 1;
                            }else{
                                general_reg[reg_num] = RAM[address_B];
                            }
                        }

                        // REF PTR A PTR B
                        else if (instruction->type_a == 3 && instruction->type_b == 3){
                            int symbol_A = instruction->val_a;
                            int symbol_B = instruction->val_b;

                            int address_A = ptr(&reg_4_sf_bp, symbol_A, RAM);
                            int address_B = reg_4_sf_bp + symbol_B + 2;

                            RAM[address_A] = RAM[address_B];

                        }

                    }else if (instruction->val_instruction == 4){
                        // add a b
                        int num_regA = instruction->val_a;
                        int num_regB = instruction->val_b;
                        if (num_regA <= 3 && num_regB <= 3){
                            general_reg[num_regA] += general_reg[num_regB];
                            
                        }else if (num_regA == 7){
                            // If there is function label, we do not count
                            for (int i = reg_7_pc; i < reg_7_pc + general_reg[num_regB]; i++){
                                if (i > count - 1 || assembly[i].type_part == 2){
                                    fprintf(stderr, "JMP Fail.\n");
                                    return 1;
                                }
                            }
                            // Since the pc always points to the next instruction, so after add jmp offset
                            // we still need to go to next instruction
                            reg_7_pc += general_reg[num_regB];
                            
                            
                        }else if (num_regB == 7){
                            general_reg[num_regA] += reg_7_pc;
                        }
                        

                    }else if (instruction->val_instruction == 5){
                        // print sth in memory
                        int val = instruction->val_a;
                        if (instruction->type_a == 0){
                            printf("%u\n", val);
                            
                        }else if (instruction->type_a == 1){
                            printf("%u\n", general_reg[val]);
                            
                        }else if (instruction->type_a == 2){
                            int symbol = instruction->val_a;
                            int offset = symbol + 2; 
                            int address = reg_4_sf_bp + offset;
                            
                            printf("%u\n", RAM[address]);
                            
                        }else if (instruction->type_a == 3){
                            
                            int symbol = instruction->val_a;
                            int address = ptr(&reg_4_sf_bp, symbol, RAM);
                            
                            printf("%u\n", RAM[address]);
                            
                            
                        }
                    }else if (instruction->val_instruction == 6){
                        int num_reg = instruction->val_a;
                        general_reg[num_reg] = ~general_reg[num_reg];
                    }else if (instruction->val_instruction == 7){
                        int num_reg = instruction->val_a;
                        
                        if (general_reg[num_reg] == 0){
                            general_reg[num_reg] = 1;
                            
                        }else{
                            general_reg[num_reg] = 0;
                            
                        }
                    }
                }

                reg_7_pc ++;
            }
        }
    }

    return 0;

}
