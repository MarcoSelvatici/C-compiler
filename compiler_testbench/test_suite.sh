#!/bin/bash


echo
echo "#########################################"
echo "# Clean and rebuild compiler for tests. #"
echo "#########################################"
echo
make clean
make bin/c_compiler

if [[ $? -ne 0 ]]; then
    exit
fi

echo
echo "##############"
echo "# Run tests. #"
echo "##############"

PASSED=0
CHECKED=0
TESTSFAILED=()

# For each test.
for i in compiler_testbench/test_cases/cprograms/*.c; do
    CHECKED=$(( ${CHECKED}+1 ));

    echo
    echo "#####################################################"
    echo "# Testing ${i}."
    echo "#####################################################"

    # Create working a folder for the test.
    program_name=$(basename ${i});
    program_name="${program_name%.*}"
    mkdir -p compiler_testbench/working/${program_name}

    # Declare paths.
    WORKING_DIR="compiler_testbench/working/${program_name}"
    DRIVER_DIR="compiler_testbench/test_cases/cdrivers"

    echo
    echo "%%%%%%%%%%%%% 1. Compile program using c_compiler. %%%%%%%%%%%%%"
    # Compile the cprogram using the compiler under testing.
    bin/c_compiler -S ${i} -o ${WORKING_DIR}/${program_name}.s

    if [[ $? -ne 0 ]]; then
        echo "  FAIL!"
        TESTSFAILED+=("${program_name}")
        continue
    fi

    echo
    echo "%%%%%%%%%%%%% 2. Compile program using gcc. %%%%%%%%%%%%%"
    # Compile the cprogram using the reference compiler.
    mips-linux-gnu-gcc -mfp32 -S ${i} -o ${WORKING_DIR}/${program_name}_ref.s

    if [[ $? -ne 0 ]]; then
        echo "  FAIL!"
        TESTSFAILED+=("${program_name}")
        continue
    fi

    echo
    echo "%%%%%%%%%%%%% 3. Create object file from compiled programs. %%%%%%%%%%%%%"
    # Use GCC to assemble the assembly programs.
    mips-linux-gnu-gcc -mfp32 -o ${WORKING_DIR}/${program_name}.o -c ${WORKING_DIR}/${program_name}.s
    if [[ $? -ne 0 ]]; then
        echo "  FAIL!"
        TESTSFAILED+=("${program_name}")
        continue
    fi
    mips-linux-gnu-gcc -mfp32 -o ${WORKING_DIR}/${program_name}_ref.o -c ${WORKING_DIR}/${program_name}_ref.s
    if [[ $? -ne 0 ]]; then
        echo "  FAIL!"
        TESTSFAILED+=("${program_name}")
        continue
    fi

    echo
    echo "%%%%%%%%%%%%% 4. Link files with the driver. %%%%%%%%%%%%%"
    # Use GCC to link the generated object files with the driver program, to produce executables.
    mips-linux-gnu-gcc -mfp32 -static -o ${WORKING_DIR}/${program_name} ${WORKING_DIR}/${program_name}.o ${DRIVER_DIR}/${program_name}_driver.c
    if [[ $? -ne 0 ]]; then
        echo "  FAIL!"
        TESTSFAILED+=("${program_name}")
        continue
    fi
    mips-linux-gnu-gcc -mfp32 -static -o ${WORKING_DIR}/${program_name}_ref ${WORKING_DIR}/${program_name}_ref.o ${DRIVER_DIR}/${program_name}_driver.c
    if [[ $? -ne 0 ]]; then
        echo "  FAIL!"
        TESTSFAILED+=("${program_name}")
        continue
    fi

    echo
    echo "%%%%%%%%%%%%% 5. Run simulator. %%%%%%%%%%%%%"
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
        TESTSFAILED+=("${program_name}")
        FAILED=1;
    fi

    if [[ ${FAILED} -eq "0" ]]; then
        PASSED=$(( ${PASSED}+1 ));
    fi
done

echo
echo "############"
echo "# Summary. #"
echo "############"
echo
echo "Passed ${PASSED} out of ${CHECKED}".
echo "Tests failed: "
for i in ${TESTSFAILED[@]}; do echo $i; done
