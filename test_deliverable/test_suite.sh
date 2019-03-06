#!/bin/bash

echo "========================================"
echo " Cleaning the temporaries and outputs"
make clean

echo "========================================"
echo " Set up..."
make  bin/c_compiler

echo "========================================="

PASSED=0
CHECKED=0

# for each test.
for i in test_deliverable/test_cases/cprograms/*.c; do
    
    # create in working a folder for the test.
    b=$(basename ${i});
    b="${b%.*}"
    mkdir -p test_deliverable/working/$b
    
    # declare shortcuts
    TARG="${test_deliverable/working/$b}"
    DRIVER="${test_deliverable/test_cases/cdrivers}"

    # compile the cprogram using the compiler under testing.
    bin/c_compiler -S ${i} -o ${TARG}/$b.s
    
    # Use GCC to assemble the assembly program .
    mips-linux-gnu-gcc -mfp32 -o ${TARG}/$b.o -c ${TARG}/$b.s
   
    # Use GCC to link the generated object file with the driver program, to produce an executable
    mips-linux-gnu-gcc -mfp32 -static -o ${TARG}/$b ${TARG}/$b.o ${DRIVER}/${b}_driver.c

    # Use QEMU to simulate the executable on MIPS
    qemu-mips ${TARG}/$b

    RESULT=$?;
    echo "${RESULT}" > TARG/$b/result.txt

    echo "==========================="
    echo ""
    echo "Input file : ${i}"
    echo "Testing $b"

    # Compare outputs to see if they match. 
    OK=0;

    if [[ "${RESULT}" -ne "0" ]]; then
        echo "  FAIL!";
        OK=1;
    fi

    if [[ "$OK" -eq "0" ]]; then
        PASSED=$(( ${PASSED}+1 ));
    fi

    CHECKED=$(( ${CHECKED}+1 ));

    echo ""
done

echo "########################################"
echo "Passed ${PASSED} out of ${CHECKED}".
echo ""

