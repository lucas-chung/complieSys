# compile_system

##环境要求

clang++ -std=c++17

##文件说明

ir.txt存放生成的中间代码

test.sy作为输入，存放SysY2020代码

test.s作为输出的ARM架构汇编代码

map.txt 为生成的中间代码映射文件

##使用说明

本次编译器仅实现了变量定义、常量定义、函数定义、函数传参、函数调用、if、while、for、do...while、break、continue、return语句，所以在将test.sy作为输入时需要注意。cmd 输入 make or make build 即可得到test.s文件
