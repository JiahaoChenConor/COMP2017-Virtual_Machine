#include "functions_for_objdump.h"


int main(int argc, char** argv)
{
    Assembly assembly[MAX_NUM_INSTRUCTION + MAX_FUN];
    int count = 0;
 
    parseMachineCode(assembly, &count, argc, &*argv, 1);

    return 0;
}

