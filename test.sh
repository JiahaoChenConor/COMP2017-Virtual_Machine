#!/bin/bash
echo "##########################"
echo "### Running tests! #######"
echo "##########################"
echo ""

count=0 # number of test cases run so far

for file in tests/*.asm;do
    name=$(basename $file .asm)
 
    binaryFile=tests/$name.x2017

    assembly=tests/$name.asm
    expected=tests/$name.out
    
    export ASAN_OPTIONS=verify_asan_link_order=0
    ./objdump_x2017 $binaryFile 2>&1 | diff - $assembly || echo "Test $name: failed for objdump_x2017!\n"

    export ASAN_OPTIONS=verify_asan_link_order=0
    ./vm_x2017 $binaryFile 2>&1 | diff - $expected || echo "Test $name: failed for vm_x2017!\n"
    count=$((count+1))
    
done
echo "Finished running $count tests!"
