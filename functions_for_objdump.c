#include "functions_for_objdump.h"


int stackSymbol(int num){
    if (num >= 0 && num <= 25){
        num += 65;
    }else if (num >= 26 && num <= 51){
        num = num - 26 + 65 + 32; 
    }

    return num;
}

void printAssembly(int print_type, Assembly * assembly){
    int type = 0;
    int val = 0;

    if (print_type == 1){
        type = assembly -> type_a;
        val = assembly -> val_a;
    }else if (print_type == 2){
        type = assembly -> type_b;
        val = assembly -> val_b;
    }else {
        return ;
    }


    if (type == 0){
        printf("VAL %d", val);
    }else if (type == 1){
        printf("REG %d", val);
    }else if(type == 2){
        printf("STK %c", stackSymbol(val));
    }else if (type == 3){
        printf("PTR %c", stackSymbol(val));
    }
    
}


// the start and end offset are all included
BYTE getBitsFromByte(BYTE original, int start_offset, int end_offset){
    BYTE mask = 0;
    BYTE degree = 1;

    for (int i = 0; i < start_offset; i++){
        degree *= 2;
    }

    for (int i = start_offset; i <= end_offset; i++){
        mask += degree;
        degree *= 2;
    }

    return (mask & original) >> start_offset;
}


BYTE getBitsFromTwoBytes(BYTE* all_bytes, BYTE index1, BYTE index2, int len, int start){
    if (index1 >= 0 && index2 >= 0){
        BYTE b1 = all_bytes[index1];
        BYTE b2 = all_bytes[index2];
        if ((start + len - 1) <= 7){
            return getBitsFromByte(b2, start, start + len - 1);
        }else{
            BYTE part2 = getBitsFromByte(b2, start, 7);
        
            BYTE part1 = getBitsFromByte(b1, 0, (start + len - 1) - 8);
      
            return (part1 << (7 - start + 1)) | part2;
        }
    }else{
        BYTE b2 = all_bytes[index2];
        return getBitsFromByte(b2, start, start + len - 1);
    }
    
}

int bitsOfValue(int type){
    if (type == 0){ 
        return 8;
    }else if (type == 1){ 
        return 3;
    }else if (type == 2){ 
        return 5;
    }else{
        return 5;
    }
}

void checkOffset(int* start_offset_ptr, int* bytes_index_ptr){
    if (*start_offset_ptr > 7){
                *start_offset_ptr %= 8;
                *bytes_index_ptr -= 1; // it is impossible for one thing to cross 3 bytes, at most 2 bytes
        }
    
    return;
}

void setAssembly(int type_val, int num_of_types, BYTE* all_bytes_ptr, int* bytes_index, int* start_offset, Assembly* assembly){

    if (num_of_types == 1 || num_of_types == 2){
        int type_A = (int) getBitsFromTwoBytes(all_bytes_ptr, *bytes_index - 1, *bytes_index, 2, *start_offset);
        *start_offset += 2;
        checkOffset(start_offset, bytes_index);   

        int bits_of_value = bitsOfValue(type_A);
        int value_type_A = (int) getBitsFromTwoBytes(all_bytes_ptr, *bytes_index - 1, *bytes_index, bits_of_value, *start_offset);
        *start_offset += bits_of_value;
        checkOffset(start_offset, bytes_index);

        assembly -> type_a = type_A;
        assembly -> val_a = value_type_A;

    }
    if (num_of_types == 2){
        int type_B = (int) getBitsFromTwoBytes(all_bytes_ptr, *bytes_index - 1, *bytes_index, 2, *start_offset);
        *start_offset += 2;
        checkOffset(start_offset, bytes_index);
        
        int bits_of_value = bitsOfValue(type_B);
        int value_type_B = (int) getBitsFromTwoBytes(all_bytes_ptr, *bytes_index - 1, *bytes_index, bits_of_value, *start_offset);
        *start_offset += bits_of_value;
        checkOffset(start_offset, bytes_index);

        assembly -> type_b = type_B;
        assembly -> val_b = value_type_B;
    }
    
     assembly -> val_instruction = type_val;
    
    return;
}

void setMapForCharacters(Map* maps, int func_label, int val){
    if (func_label >= 8){
        printf("func_label %d\n", func_label);
        fprintf(stderr, "map overflow");
        return;
    }
    Map* map_this_function = &maps[func_label];
    for (int i = 0; i < (map_this_function->count); i++){
        if (map_this_function->count >= 32){
            fprintf(stderr, "map overflow");
            return;
        }
        if ((map_this_function->set)[i] == val){
            return;
        }
    }

    (map_this_function->set)[map_this_function->count] = val;
    (map_this_function->count)++;

}


void resetStackSymbolVal(Map* maps, Assembly* assembly, int len){
    int func_label = 0;
    for (int i = len - 1; i >= 0; i--){
        Assembly* instruction = &assembly[i];
        if (instruction->type_part == 2){
            func_label = instruction->val_part;
        }else if (instruction->type_part == 3){
            if (instruction->type_a == 2 || instruction->type_a == 3){
                for (int j = 0; j < maps[func_label].count ; j++){
                    if ((maps[func_label].set)[j] == instruction->val_a){
                        instruction->val_a = j;
                        break;
                    }
                }
            }
            if (instruction->type_b == 2 || instruction->type_b == 3){
                for (int j = 0; j < maps[func_label].count ; j++){
                    if ((maps[func_label].set)[j] == instruction->val_b){
                        instruction->val_b = j;
                        break;
                    }
                }
            }
        }
    }

}

int parseMachineCode(Assembly * assembly, int* count, int num_of_argu, char **argument, int printed){
    FILE *fp;
    if (num_of_argu != 2){
        fprintf(stderr, "Wrong number of arguments.\n");
    }
    
    fp = fopen(argument[1], "rb");
    if (fp == NULL){
        fprintf(stderr, "Fail to open the x2017 binary file.\n");
        return 1;
    }

    // Max number of bytes is 
    //  1 byte (function label + bit for number of instructions) * MAX_FUN
    //  + max number of instructions * max length of every instruction.
    BYTE all_bytes[MAX_FUN + MAX_NUM_INSTRUCTION * MAX_INSTRUTION_LEN]; 

    int ret;

    // Read the file by byte
    // store them in the array all_bytes
    int num_of_bytes = 0;
    while( (ret = fread(all_bytes + num_of_bytes,sizeof(BYTE), 1, fp)) != 0 ){   
                num_of_bytes ++;
    }

    fclose(fp);
    int bytes_index = num_of_bytes - 1;
    
    int num_of_instructions = 0;
    int bits_padding = 0;
    int operation = -1;

    int is_function_label = 0;
    int is_operation = 0;
    int is_num_of_insturctions = 1;

    // start_offset is the distance from the last bit of current byte
    int start_offset = 0;

    // If we want to know every instruction, we need to read from back
    while (bytes_index != -1){
        if (bytes_index > num_of_bytes - 1){
            fprintf(stderr, "Error on bytes of index\n");
        }
        if (is_num_of_insturctions){
            
            num_of_instructions = (int) getBitsFromTwoBytes(all_bytes, bytes_index - 1, bytes_index, 5, start_offset);
            
            start_offset += 5;
            checkOffset(&start_offset, &bytes_index);

            // change the flag
            is_num_of_insturctions = 0;
            is_operation = 1;
        }

        else if (is_operation){
            // If number of instruction is equal to 0, we will traverse the end of another funcion again.
            if (num_of_instructions == 0){
                is_operation = 0;
                is_function_label = 1;
                continue; 
            }

            operation = (int) getBitsFromTwoBytes(all_bytes, bytes_index - 1, bytes_index, 3, start_offset);
        
            start_offset += 3;
            checkOffset(&start_offset, &bytes_index);

            assembly[*count].type_part = 3;
            

            if (operation == 2){
                setAssembly(operation, 0, all_bytes, &bytes_index, &start_offset, &assembly[*count]);
            }else if (operation == 1 || operation == 5 || operation == 6 || operation == 7){
                setAssembly(operation, 1, all_bytes, &bytes_index, &start_offset, &assembly[*count]);
            }else if (operation == 0 || operation == 3 || operation == 4){
                setAssembly(operation, 2, all_bytes, &bytes_index, &start_offset, &assembly[*count]);
            }

            num_of_instructions --;
            (*count) ++;

        }
        
        else if (is_function_label){
            int function_label = (int) getBitsFromTwoBytes(all_bytes, bytes_index - 1, bytes_index, 3, start_offset);
            start_offset += 3;
            checkOffset(&start_offset, &bytes_index);

            assembly[*count].type_part = 2;
            assembly[*count].val_part = function_label; 
            (*count) ++;

            if (bytes_index <= 0){
                if (bytes_index == 0){
                    bits_padding = 8 - start_offset;
                }else{
                    bits_padding = 0;
                }
                assembly[*count].type_part = 1;
                assembly[*count].val_part = bits_padding;
                
                break;
            }

            is_function_label = 0;
            is_num_of_insturctions = 1;
        }


    }

    Map mapsForCharacters[8];
    for (int i = 0; i < 8; i++){
        mapsForCharacters[i].count = 0;
    }
    int func_label = 0;
    for (int i = (*count - 1); i >= 0; i--){
        Assembly* instruction = &assembly[i];
        if (instruction->type_part == 2){
            func_label = instruction->val_part;
            
        }else if (instruction->type_part == 3){
            if (instruction->val_instruction == 0 || instruction->val_instruction == 3 || instruction->val_instruction == 4 ||instruction->val_instruction == 5){
                if (instruction->type_a == 2){
                    setMapForCharacters(mapsForCharacters, func_label, instruction->val_a);
                }
            }
           
           if (instruction->val_instruction == 0 || instruction->val_instruction == 3 || instruction->val_instruction == 4){
                if (instruction->type_b == 2){
                    setMapForCharacters(mapsForCharacters, func_label, instruction->val_b);
                }
           }
           
        }
    }

    resetStackSymbolVal(mapsForCharacters, assembly, *count);
    
    if (printed){
        
        for (int i = *count; i >= 0; i--){

            if (assembly[i].type_part == 3){
                if (assembly[i].val_instruction == 0){
                    printf("    MOV ");
                    printAssembly(1, &assembly[i]);
                    printf(" ");

                    printAssembly(2, &assembly[i]);
                    printf("\n");
                    
                }else if (assembly[i].val_instruction == 1){
                    printf("    CAL ");
                    printAssembly(1, &assembly[i]);
                    printf("\n");

                }else if (assembly[i].val_instruction == 2){
                    printf("    RET\n");

                }else if (assembly[i].val_instruction == 3){
                    printf("    REF ");
                    printAssembly(1, &assembly[i]);
                    printf(" ");

                    printAssembly(2, &assembly[i]);
                    printf("\n");

                }else if (assembly[i].val_instruction == 4){
                    printf("    ADD ");
                    printAssembly(1, &assembly[i]);
                    printf(" ");

                    printAssembly(2, &assembly[i]);
                    printf("\n");

                }else if (assembly[i].val_instruction == 5){
                    printf("    PRINT ");
                    printAssembly(1, &assembly[i]);
                    printf("\n");

                }else if (assembly[i].val_instruction == 6){
                    printf("    NOT REG %d\n", assembly[i].val_a);

                }else if (assembly[i].val_instruction == 7){
                    
                    printf("    EQU REG %d\n", assembly[i].val_a);
                }
            }else if (assembly[i].type_part == 2){
                printf("FUNC LABEL %d\n", assembly[i].val_part);
                
            }
        }
    }

    return 0;

}
