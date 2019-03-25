#!/bin/bash

echo "========================================"
echo " Cleaning the temporaries and outputs"
make clean

echo "========================================"
echo " Set up..."
make  bin/c_compiler
mkdir translator_testbench/cprograms/bin
mkdir translator_testbench/pyprograms
echo "========================================="

PASSED=0
CHECKED=0

# for each test.
for i in translator_testbench/cprograms/*.c; do
    
    # create in working a folder for the test.
    b=$(basename ${i});
    b="${b%.*}"
    mkdir -p translator_testbench/working/$b
    
    # compile the cprogram.
    g++ $i -o translator_testbench/cprograms/bin/$b

    # translate it into python.
    ./bin/c_compiler --translate translator_testbench/cprograms/$b.c -o translator_testbench/pyprograms/$b.py
    echo "==========================="
    echo ""
    echo "Input file : ${i}"
    echo "Testing $b"

    # compare the results.
    translator_testbench/cprograms/bin/$b
    C_RESULT=$?;
    echo "${C_RESULT}" > translator_testbench/working/$b/c-result.txt
    
    chmod u+x translator_testbench/pyprograms/$b.py
    python3 translator_testbench/pyprograms/$b.py
    PY_RESULT=$?;
    echo "${PY_RESULT}" > translator_testbench/working/$b/py-result.txt
    

    # Compare outputs to see if they match. 
    OK=0;

    if [[ "${C_RESULT}" -ne "${PY_RESULT}" ]]; then
        echo "  c result : ${C_RESULT}"
        echo "  py result : ${PY_RESULT}"
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

