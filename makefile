CPP = g++
CPPFLAGS = -std=c++11 -W -g -Wno-unused-parameter -I include
BISON = bison -v -d

# Actual final binary.
bin/c_compiler : common/src/c_lexer.yy.o common/src/c_parser.tab.o c_compiler/src/compiler.o c_translator/src/translator.o compiler_and_translator.o c_compiler/src/compiler_util.o
	mkdir -p bin
	${CPP} ${CPPFLAGS} $^ -o $@

# Debug.
bin/print_ast : common/src/c_lexer.yy.o common/src/c_parser.tab.o c_compiler/src/compiler.o  c_compiler/src/compiler_util.o common/src/print_ast.o
	${CPP} ${CPPFLAGS} $^ -o $@

# To see if they build.
lexer_and_parser : common/src/c_lexer.yy.cpp

common/src/c_parser.tab.cpp common/src/c_parser.tab.hpp : common/src/c_parser.y
	${BISON} $< -o common/src/c_parser.tab.cpp

common/src/c_lexer.yy.cpp : common/src/c_lexer.flex common/src/c_parser.tab.hpp
	flex -o $@ $<

clean :
	@echo Cleaning bin/ ...
	rm -f bin/*
	@echo Cleaning common/src/ ...
	rm -f common/src/*.tab.cpp
	rm -f common/src/*.tab.hpp
	rm -f common/src/*.output
	rm -f common/src/*.yy.cpp
	rm -f common/src/*.o
	@echo Cleaning c_compiler/src/ ...
	rm -f c_compiler/src/*.o
	@echo Cleaning c_translator/src/ ...
	rm -f c_translator/src/*.o
	@echo Cleaning root ...
	rm -f *.o
	@echo Cleaning tests ...
	rm -r -f compiler_testbench/working
	rm -r -f translator_testbench/cprograms/bin
	rm -r -f translator_testbench/pyprograms
	rm -r -f translator_testbench/working
	rm -r -f test_deliverable/working
