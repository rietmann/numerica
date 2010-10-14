#bison -d calc.y
#flex calc_tokens.l
# g++ `llvm-config --cxxflags --ldflags --libs core` types.cpp lex.yy.c calc.tab.c -lm -o go

bison -d parser.y -o parser.cpp
lex -o tokens.cpp lexer.l parser.hpp 
g++ `llvm-config --cxxflags` -c types.cpp
g++ `llvm-config --cxxflags` -c parser.cpp tokens.cpp
# get ride of "simple_start.cpp" for now
# g++ `llvm-config --cxxflags --ldflags --libs all` types.o tokens.o parser.o simple_start.cpp -lm -o go
g++ types.o tokens.o parser.o `llvm-config --cxxflags --ldflags --libs core jit native` -rdynamic -lm -o go
