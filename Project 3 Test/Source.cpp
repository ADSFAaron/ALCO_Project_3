#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct doPrediction
{
	int entry;
	string rs1;
	string rs2;
	string label;
};

struct label
{
	string name;
	int pos;
};

struct predict
{
	string state[4] = { "SN","SN","SN","SN" };	//預設的4個state
	string his[2] = { "N","N" };								//his[0] = 上個 his[1] = 上上個
	int pred1 = 0;
	int pred2 = 0;
	string pred = "N";											//當前預測值
	int miss = 0;													//預測錯誤次數
};

struct instruct
{
	string name;		//Instruction Name
	string reg1;		//Instruction  Register Destination
	string reg2;		//(Optional) Instruction Register
	string imm;		//Instruction Immediate
};

doPrediction pred[100];
predict State[100];
vector<instruct> implement;
vector<label> Label;
int reg[32] = {};
int rowPos = 0;
int entry = 0;

bool label_detected(string& input)
{
	//找到label 
	if (input.find(':') != string::npos)
	{
		input[input.size() - 1] = '\0';
		input.resize(input.size() - 1);
		return true;
	}
	else
	{
		return false;
	}
}

void li(string rs1, string imm)
{
	int r = atoi(rs1.substr(1, rs1.length() - 1).c_str());	//將rd 改成數字，從register 取值
	int num = atoi(imm.c_str());
	reg[r] = num;
}

void addi(string rd, string rs, string imm)
{
	int r1 = atoi(rd.substr(1, rd.length() - 1).c_str());
	int r2 = atoi(rs.substr(1, rs.length() - 1).c_str());
	int num = atoi(imm.c_str());
	reg[r1] = reg[r2] + num;
}

void prediction(int rs1, int rs2, string label)
{
	string nowstate;		//判斷目前在哪個state
	int beqno = 0;				//判斷是再跑which beq
	int statenum;				//判斷第幾個state
	string result = "T";	//state實際結果
	int beqCount = 0;

	//找instruction中所有beq
	for (int i = 0; i < implement.size(); ++i)
	{
		if (implement[i].name == "beq")
		{
			pred[beqCount].entry = i % entry;
			pred[beqCount].label = implement[i].imm;
			pred[beqCount].rs1 = implement[i].reg1;
			pred[beqCount].rs2 = implement[i].reg2;
			++beqCount;
		}
	}

	int predtime = 0;

	//找出所有beq
	for (int i = 0; i < beqCount; ++i)
	{
		if (pred[i].label == label)
		{
			predtime = pred[i].entry;

			//印出beq對應的entry
			cout << "entry: " << predtime << setw(7) << "beq " << pred[i].rs1 << "," << pred[i].rs2 << "," << label << endl;
			break;
		}
	}

	//判斷 2-bit history 為哪個
	if (State[predtime].his[0] == "N")
		State[predtime].pred1 = 0;
	else
		State[predtime].pred1 = 1;

	if (State[predtime].his[1] == "N")
		State[predtime].pred2 = 0;
	else
		State[predtime].pred2 = 1;

	//按照2-bit history設定目前state
	if (State[predtime].pred2 == 0 && State[predtime].pred1 == 0)
	{
		nowstate = "00";
		statenum = 0;
	}
	else if (State[predtime].pred2 == 0 && State[predtime].pred1 == 1)
	{
		nowstate = "01";
		statenum = 1;
	}
	else if (State[predtime].pred2 == 1 && State[predtime].pred1 == 0)
	{
		nowstate = "10";
		statenum = 2;
	}
	else
	{
		nowstate = "11";
		statenum = 3;
	}

	//預測是N or T
	if (State[predtime].state[statenum] == "SN" || State[predtime].state[statenum] == "WN")
		State[predtime].pred = "N";
	else
		State[predtime].pred = "T";

	//輸出結果
	cout << "(" << nowstate << ", " << State[predtime].state[0] << ", " << State[predtime].state[1] << ", " << State[predtime].state[2] << ", " << State[predtime].state[3] << ") "
		<< State[predtime].pred << " ";

	//實際beq結果
	if (reg[rs1] == reg[rs2])
	{
		result = "T";

		//改變state裡的狀態
		if (result == State[predtime].pred)
		{
			if (State[predtime].state[statenum] == "WT")
				State[predtime].state[statenum] = "ST";
		}
		else
		{
			if (State[predtime].state[statenum] == "SN")
				State[predtime].state[statenum] = "WN";
			else if (State[predtime].state[statenum] == "WN")
				State[predtime].state[statenum] = "WT";
			State[predtime].miss++;
		}
	}
	else
	{
		result = "N";

		//改變state狀態
		if (result == State[predtime].pred)
		{
			if (State[predtime].state[statenum] == "WN")
				State[predtime].state[statenum] = "SN";
		}
		else
		{
			if (State[predtime].state[statenum] == "ST")
				State[predtime].state[statenum] = "WT";
			else if (State[predtime].state[statenum] == "WT")
				State[predtime].state[statenum] = "WN";

			State[predtime].miss++;
		}
	}

	cout << result << setw(18) << "misprediction: " << State[predtime].miss << endl << endl;

	State[predtime].his[1] = State[predtime].his[0];
	State[predtime].his[0] = result;
}

void beq(string rs1, string rs2, string label)
{
	int r1 = atoi(rs1.substr(1, rs1.length() - 1).c_str());
	int r2 = atoi(rs2.substr(1, rs2.length() - 1).c_str());

	prediction(r1, r2, label);

	if (reg[r1] == reg[r2] && label == "End")
	{
		rowPos = -2;
		return;
	}

	//if(r1 == r2) 
	//		jump to Label
	for (int i = 0; i < Label.size(); ++i)
	{
		if (reg[r1] == reg[r2] && label == Label[i].name)
		{
			rowPos = Label[i].pos - 1;
		}
	}
}

int main()
{
	label tmp;
	instruct file;
	string input;
	int row = 0;			//判斷輸入到第幾行

	//讀RISC V code
	ifstream infile("test.txt", ios::in);

	//檔案不存在
	if (!infile)
	{
		cerr << "Risc v file not found" << endl;
		system("pause");
		exit(0);
	}

	cout << "Input : number of entries" << endl;
	cin >> entry;

	//讀RISC V code
	while (getline(infile, input))
	{
		//label存在，將label名字和行數儲存
		if (label_detected(input))
		{
			tmp.name = input;
			tmp.pos = row;
			Label.push_back(tmp);
		}
		else
		{
			stringstream ss(input);

			//分割instruction name
			getline(ss, file.name, ' ');

			if (file.name == "li")
			{
				getline(ss, file.reg1, ',');
				getline(ss, file.imm, '\n');
			}
			else if (file.name == "addi" || file.name == "beq")
			{
				getline(ss, file.reg1, ',');
				getline(ss, file.reg2, ',');
				getline(ss, file.imm, '\n');
			}
			else
			{
				cout << "error infile \n\n";
				system("pause");
			}
			++row;

			//將Instruction儲存到vector中
			implement.push_back(file);
		}
	}

	infile.close();

	while (rowPos > -1)
	{
		//判斷是哪個instruction
		if (implement[rowPos].name == "li")
		{
			li(implement[rowPos].reg1, implement[rowPos].imm);
		}
		else if (implement[rowPos].name == "addi")
		{
			addi(implement[rowPos].reg1, implement[rowPos].reg2, implement[rowPos].imm);
		}
		else if (implement[rowPos].name == "beq")
		{
			beq(implement[rowPos].reg1, implement[rowPos].reg2, implement[rowPos].imm);
		}

		++rowPos;
	}

	system("pause");
	return 0;
}
