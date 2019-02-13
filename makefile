CPPFLAGS += -std=c++11 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I include
BISON = bison -v -d

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
