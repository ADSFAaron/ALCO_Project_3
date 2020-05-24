#include<iostream>
#include<iomanip>
#include<fstream>
#include <sstream>
#include<string>
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
	string state[4] = { "SN","SN","SN","SN" };	//�w�]��4��state
	string his[2] = { "N","N" };								//his[0] = �W�� his[1] = �W�W��
	int pred1 = 0;
	int pred2 = 0;
	string pred = "N";											//��e�w����
	int miss = 0;													//�w�����~����
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
	//���label 
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
	int r = atoi(rs1.substr(1, rs1.length() - 1).c_str());	//�Nrd �令�Ʀr�A�qregister ����
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
	string nowstate;		//�P�_�ثe�b����state
	int beqno = 0;				//�P�_�O�A�]which beq
	int statenum;				//�P�_�ĴX��state
	string result = "T";	//state��ڵ��G
	int beqCount = 0;

	//��instruction���Ҧ�beq
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

	//��X�Ҧ�beq
	for (int i = 0; i < beqCount; ++i)
	{
		if (pred[i].label == label)
		{
			predtime = pred[i].entry;

			//�L�Xbeq������entry
			cout << "entry: " << predtime << setw(7) << "beq " << pred[i].rs1 << "," << pred[i].rs2 << "," << label << endl;
			break;
		}
	}

	//�P�_ 2-bit history ������
	if (State[predtime].his[0] == "N")
		State[predtime].pred1 = 0;
	else
		State[predtime].pred1 = 1;

	if (State[predtime].his[1] == "N")
		State[predtime].pred2 = 0;
	else
		State[predtime].pred2 = 1;

	//����2-bit history�]�w�ثestate
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

	//�w���ON or T
	if (State[predtime].state[statenum] == "SN" || State[predtime].state[statenum] == "WN")
		State[predtime].pred = "N";
	else
		State[predtime].pred = "T";

	//��X���G
	cout << "(" << nowstate << ", " << State[predtime].state[0] << ", " << State[predtime].state[1] << ", " << State[predtime].state[2] << ", " << State[predtime].state[3] << ") "
		<< State[predtime].pred << " ";

	//���beq���G
	if (reg[rs1] == reg[rs2])
	{
		result = "T";

		//����state�̪����A
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

		//����state���A
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
	int row = 0;			//�P�_��J��ĴX��

	//ŪRISC V code
	ifstream infile("test.txt", ios::in);

	//�ɮפ��s�b
	if (!infile)
	{
		cerr << "Risc v file not found" << endl;
		system("pause");
		exit(0);
	}

	cout << "Input : number of entries" << endl;
	cin >> entry;

	//ŪRISC V code
	while (getline(infile, input))
	{
		//label�s�b�A�Nlabel�W�r�M����x�s
		if (label_detected(input))
		{
			tmp.name = input;
			tmp.pos = row;
			Label.push_back(tmp);
		}
		else
		{
			stringstream ss(input);

			//����instruction name
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

			//�NInstruction�x�s��vector��
			implement.push_back(file);
		}
	}

	infile.close();

	while (rowPos > -1)
	{
		//�P�_�O����instruction
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