# ALCO_Project_3
Homework for Assembled Language and Computer Organization  Project Three

## Project Goal
可以將輸入的一段 RISC-V 組合語言的 code 將branch的instruction做Predictin

Input : 一段RISC-V組語的code(需寫在test.txt檔裡面),entry數

Output : entry以及Prediction的state轉換,預測的狀態及實際狀態,每個entry misprediction的次數

## Project Method
透過讀檔，將RISC-V code讀入程式中。

實際做每一行RISC-V的實際運行內容。

在有beq的function中執行Prediction的程式。

Prediction處理state的轉換，判斷預測結果是否跟真實結果相符，以及Output。

在RISC-V的程式跳到END後即結束。

## How to Use?
Sample Input : 
```
	li R1,0 //等同addi R1,R0,0
	li R2,4
Loop:
	beq R1,R2,End
	addi R2,R2,-1
	beq R0,R0,Loop //R0就是我們常用的x0唷
End:
```

Ssample Output :
```
//input: number of entries
8

entry: 2        beq R1,R2,End           //beq R1,R2,End 使用編號2的entry
(00, SN, SN, SN, SN) N N                misprediction: 0
//狀態            預測值 實際值            本預測器miss次數 (從頭統計至今)
entry: 4        beq R0,R0,Loop
(00, SN, SN, SN, SN) N T                misprediction: 1

entry: 2        beq R1,R2,End
(00, SN, SN, SN, SN) N N                misprediction: 0

entry: 4        beq R0,R0,Loop
(01, WN, SN, SN, SN) N T                misprediction: 2

entry: 2        beq R1,R2,End
(00, SN, SN, SN, SN) N N                misprediction: 0

entry: 4        beq R0,R0,Loop
(11, WN, WN, SN, SN) N T                misprediction: 3

entry: 2        beq R1,R2,End
(00, SN, SN, SN, SN) N N                misprediction: 0

entry: 4        beq R0,R0,Loop
(11, WN, WN, SN, WN) N T                misprediction: 4

entry: 2        beq R1,R2,End
(00, SN, SN, SN, SN) N T                misprediction: 1
```

## 程式碼

```c++
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
using namespace std;
```
`include<iostream>`  用來在Terminal輸入輸出

`include<iomanip>`   用在輸出格式上，以方便觀看

`include<fstream>`  讀取檔案

`include<sstream>`  分割string用

`include<string>`  用來使用string的 operator[]、find 等功能

`include<vector>`  儲存所有Opreator 和輸入的Instruction的空間
