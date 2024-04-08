Acompiler:symt.c scanner.l parser.y exp_tree.c queue.c
	flex scanner.l
	bison parser.y
	clang++ -std=c++17 -O2 -g -o compiler main.cpp -lm
	./compiler -S -o test.s test.sy

