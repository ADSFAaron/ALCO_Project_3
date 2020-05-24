#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
using namespace std;

int reg[32] = {}; //register
int labtime = 0;
int beqtime = 0;
int now = 0;

struct beq {
	int entry;
	string r1;
	string r2;
	string label;
}_beq[50]; //存入每個beq

struct label {
	string name;
	int pos;
}_lab[50];

struct predict {
	string state[4] = { "SN","SN","SN","SN" };
	string his[2] = { "N","N" }; //his[0]=上個 his[1]=上上個
	int pred1 = 0; //上個
	int pred2 = 0; //上上個
	string pred = "N"; //當前預測值
	int miss = 0; //預測錯誤次數
}_pred[50];

void li(string s1, string c) {
	//cout << "li " << s1 << "," << c << endl;
	int r = atoi(s1.substr(1, s1.length() - 1).c_str()); //擷取去掉r之後的數字
	int num = atoi(c.c_str());
	reg[r] = num;  //把當前的reg的值放入reg裡
	//cout << reg[0] << reg[1] << reg[2];
}

void addi(string s1, string s2, string c) {
	//cout << "addi " << s1 << "," << s2 << "," << c << endl;
	int r1 = atoi(s1.substr(1, s1.length() - 1).c_str());
	int r2 = atoi(s2.substr(1, s2.length() - 1).c_str());
	int num = atoi(c.c_str());
	reg[r1] = reg[r2] + num;
	//cout << reg[0] << reg[1] << reg[2];
}

void beq(string s1, string s2, string s3) {
	string nowstate;
	int beqno = 0;
	int statenum;
	string result = "T";
	for (int i = 0; i < beqtime; i++) {
		if (_beq[i].r1 == s1 && _beq[i].r2 == s2 && _beq[i].label == s3)
			beqno = i;
	}
	cout << "entry: " << _beq[beqno].entry << setw(7) << "beq " << s1 << "," << s2 << "," << s3 << endl;

	int r1 = atoi(s1.substr(1, s1.length() - 1).c_str());
	int r2 = atoi(s2.substr(1, s2.length() - 1).c_str());
	int predtime = _beq[beqno].entry / 2 - 1;
	if (_pred[predtime].his[0] == "N") //判斷歷史條件並設定好
		_pred[predtime].pred1 = 0;
	else
		_pred[predtime].pred1 = 1;
	if (_pred[predtime].his[1] == "N")
		_pred[predtime].pred2 = 0;
	else
		_pred[predtime].pred2 = 1;

	if (_pred[predtime].pred2 == 0 && _pred[predtime].pred1 == 0) //設定現在的state
		nowstate = "00";
	else if (_pred[predtime].pred2 == 1 && _pred[predtime].pred1 == 0)
		nowstate = "10";
	else if (_pred[predtime].pred2 == 0 && _pred[predtime].pred1 == 1)
		nowstate = "01";
	else
		nowstate = "11";
	statenum = _pred[predtime].pred2 * 2 + _pred[predtime].pred1;
	if (_pred[predtime].state[statenum] == "SN" || _pred[predtime].state[statenum] == "WN")
		_pred[predtime].pred = "N";
	else
		_pred[predtime].pred = "T";
	cout << "(" << nowstate << ", " << _pred[predtime].state[0] << ", " << _pred[predtime].state[1] << ", " << _pred[predtime].state[2] << ", " << _pred[predtime].state[3] << ")" << _pred[predtime].pred << " ";
	if (reg[r1] == reg[r2]) {
		result = "T";
		if (result == _pred[predtime].pred) {//改變STATE裡的狀態
			if (_pred[predtime].state[statenum] == "WT")
				_pred[predtime].state[statenum] = "ST";
		}
		else {
			if (_pred[predtime].state[statenum] == "SN")
				_pred[predtime].state[statenum] = "WN";
			else if (_pred[predtime].state[statenum] == "WN")
				_pred[predtime].state[statenum] = "WT";
			_pred[predtime].miss++;
		}
		cout << result << setw(18) << "misprediction: " << _pred[predtime].miss << endl;
		for (int i = 0; i < labtime; i++) {
			if (_lab[i].name == s3) {
				now = _lab[i].pos - 1;
			}
		}
	}
	else {
		result = "N";
		if (result == _pred[predtime].pred) {//改變STATE裡的狀態
			if (_pred[predtime].state[statenum] == "WN")
				_pred[predtime].state[statenum] = "SN";
		}
		else {
			if (_pred[predtime].state[statenum] == "ST")
				_pred[predtime].state[statenum] = "WT";
			else if (_pred[predtime].state[statenum] == "WT")
				_pred[predtime].state[statenum] = "WN";
			_pred[predtime].miss++;
		}
		cout << result << setw(18) << "misprediction: " << _pred[predtime].miss << endl << endl;
	}
	_pred[predtime].his[1] = _pred[predtime].his[0];
	_pred[predtime].his[0] = result;
	//cout << reg[0] << reg[1] << reg[2];
}

int main() {

	//讀RISC V code
	ifstream infile("test.txt", ios::in);

	if (!infile)
	{
		cerr << "Risc v file not found" << endl;
		system("pause");
		exit(0);
	}

	string s[200], ss;
	int entry;
	int entrystrart = 2;
	cout << "Input : number of entries" << endl;
	cin >> entry;
	int num = 0;

	while (infile >> ss) {
		for (int j = 0; j < ss.length(); j++) {
			if (ss[j] == ',') {
				j++;
				num++;
			}
			s[num] += ss[j];
		}
		num++;
	}

	for (int i = 0; i < num; i++) {
		if (s[i] == "beq") {
			int tmp = i + 3;
			_lab[labtime].name = s[tmp];
			_beq[beqtime].r1 = s[i + 1];
			_beq[beqtime].r2 = s[i + 2];
			_beq[beqtime].label = s[tmp];
			_beq[beqtime].entry = entrystrart;
			entrystrart += 2;
			beqtime++;
		}
		for (int j = 0; j < num; j++) {
			if ((_lab[labtime].name + ':') == s[j]) {
				_lab[labtime].pos = j;
				labtime++;
			}
		}
	}

	//cout << labtime << endl; TEST

/*	for (int i = 0; i < labtime; i++) {
		cout << _lab[i].name << " " << _lab[i].pos << endl;
	}*/
	for (; now < num; now++) {
		if (s[now] == "li") {
			li(s[now + 1], s[now + 2]);
		}
		else if (s[now] == "addi") {
			addi(s[now + 1], s[now + 2], s[now + 3]);
		}
		else if (s[now] == "beq") {
			beq(s[now + 1], s[now + 2], s[now + 3]);
		}
	}
	cout << reg[0] << reg[1] << reg[2];
	system("pause");
}