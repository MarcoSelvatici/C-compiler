#!/bin/bash

echo
echo "%%%%%%%%%%%%% Clean and rebuild compiler for tests %%%%%%%%%%%%%"
make clean
make bin/c_compiler

echo 
echo "%%%%%%%%%%%%% Run tests %%%%%%%%%%%%%"

PASSED=0
CHECKED=0

# For each test.
for i in test_deliverable/test_cases/cprograms/*.c; do
    echo 
    echo "%%%%%%%%%%%%% Testing ${i} %%%%%%%%%%%%%"

    # Create working a folder for the test.
    program_name=$(basename ${i});
    program_name="${program_name%.*}"
    mkdir -p test_deliverable/working/${program_name}
    
    # Declare paths.
    WORKING_DIR="test_deliverable/working/${program_name}"
    DRIVER_DIR="test_deliverable/test_cases/cdrivers"

    echo
    echo "########################################"
    echo "# 1. Compile program using c_compiler. #"
    echo "########################################"
    # Compile the cprogram using the compiler under testing.
    bin/c_compiler -S ${i} -o ${WORKING_DIR}/${program_name}.s

    echo
    echo "#################################"
    echo "# 2. Compile program using gcc. #"
    echo "#################################"
    # Compile the cprogram using the reference compiler.
    mips-linux-gnu-gcc -mfp32 -S ${i} -o ${WORKING_DIR}/${program_name}_ref.s
    
    echo
    echo "#################################################"
    echo "# 3. Create object file from compiled programs. #"
    echo "#################################################"
    # Use GCC to assemble the assembly programs.
    mips-linux-gnu-gcc -mfp32 -o ${WORKING_DIR}/${program_name}.o -c ${WORKING_DIR}/${program_name}.s
    mips-linux-gnu-gcc -mfp32 -o ${WORKING_DIR}/${program_name}_ref.o -c ${WORKING_DIR}/${program_name}_ref.s

    echo
    echo "##################################"
    echo "# 4. Link files with the driver. #"
    echo "##################################"
    # Use GCC to link the generated object files with the driver program, to produce executables.
    mips-linux-gnu-gcc -mfp32 -static -o ${WORKING_DIR}/${program_name} ${WORKING_DIR}/${program_name}.o ${DRIVER_DIR}/${program_name}_driver.c
    mips-linux-gnu-gcc -mfp32 -static -o ${WORKING_DIR}/${program_name}_ref ${WORKING_DIR}/${program_name}_ref.o ${DRIVER_DIR}/${program_name}_driver.c

    echo
    echo "#####################"
    echo "# 5. Run simulator. #"
    echo "#####################"
    # Use QEMU to simulate the executable on MIPS.
    qemu-mips ${WORKING_DIR}/${program_name}
    RESULT=$?;
    qemu-mips ${WORKING_DIR}/${program_name}_ref
    RESULT_REF=$?;

    echo "${RESULT}" > ${WORKING_DIR}/result.txt
    echo "Result:     ${RESULT}"
    echo "Result_ref: ${RESULT_REF}"

    # Compare outputs to see if they match. 
    FAILED=0;

    if [[ ${RESULT} -ne ${RESULT_REF} ]]; then
        echo "  FAIL!";
        FAILED=1;
    fi

    if [[ ${FAILED} -eq "0" ]]; then
        PASSED=$(( ${PASSED}+1 ));
    fi

    CHECKED=$(( ${CHECKED}+1 ));
done

echo 
echo "%%%%%%%%%%%%% Summary %%%%%%%%%%%%%"
echo "Passed ${PASSED} out of ${CHECKED}".
echo

