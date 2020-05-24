# ALCO_Project_3
Homework for Assembled Language and Computer Organization  Project One

## Project Goal
可以將輸入的一段 RISC-V 組合語言的 code 轉換成對應的 Machine Code

Input : 一段RISC-V組語的code

Output : 對應的machine code

## Project Method
透過讀檔，將RISC-V Type種類讀入程式中。

並先將Label所在的行數先存入`vector<string> LABEL` 

先比對輸入 Name 對應的 Type ，並將其 Type 讀入剩下參數，並轉換輸出 opcode、funct7、 funct3

之後將剩下的register轉換成二進位，印出結果。


## How to Use?
Sample Input : 
```
add x2,x2,x23
addi x24,x24,2
bne x24,x23,L2
sd x27, 0(x10)
beq x0,x0,L1
L2:
sd x1, 0(x2)
L1:
exit
```

Ssample Output :
```
0000000 10111 00010 000 00010 0110011
0000000000010 11000 000 11000 0010011
000010 10111 11000 001 000001 1100011
0000000 11011 01010 011 00000 0100011
000100 00000 00000 000 000000 1100011
0000000 00001 00010 011 00000 0100011
```

## 程式碼

```c++
#include<iostream>
#include<string>
#include<vector>
using namespace std;
```
`include<iostream>`  用來在Terminal輸入輸出

`include<string>`  用來使用string的 operator[]、find 等功能

`include<vector>`  儲存所有Opreator 和輸入的Instruction的空間

`include<fstream>`  讀取檔案

`include<sstream>`  分割string用
