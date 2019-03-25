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
echo

path=compiler_testbench/test_cases/cprograms/MULTI_FUNCS.c
dpath=compiler_testbench/test_cases/cdrivers/MULTI_FUNCS_driver.c

rm -f -r compiler_testbench/working
mkdir compiler_testbench/working

p=compiler_testbench/working/F
pd=compiler_testbench/working/Fd

echo "### Compile"
./bin/c_compiler -S $path -o $p.s
./bin/c_compiler -S $dpath -o $pd.s
mips-linux-gnu-gcc -mfp32 -S $path -o ${p}_ref.s
mips-linux-gnu-gcc -mfp32 -S $dpath -o ${pd}_ref.s

echo "### Assemble"
mips-linux-gnu-gcc -mfp32 -o $p.o -c $p.s
mips-linux-gnu-gcc -mfp32 -o $pd.o -c $pd.s
mips-linux-gnu-gcc -mfp32 -o ${p}_ref.o -c ${p}_ref.s
mips-linux-gnu-gcc -mfp32 -o ${pd}_ref.o -c ${pd}_ref.s

echo "### Link"
mips-linux-gnu-gcc -mfp32 -static -o compiler_testbench/working/out $p.o $pd.o
mips-linux-gnu-gcc -mfp32 -static -o compiler_testbench/working/out_ref ${p}_ref.o ${pd}_ref.o

echo "### Simulate"
qemu-mips compiler_testbench/working/out
o1=$?
qemu-mips compiler_testbench/working/out_ref
o2=$?

echo "got: " $o1
echo "ref: " $o2

