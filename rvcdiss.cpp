#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <string>
using namespace std;

unsigned int pc = 0x0;

char memory[8 * 1024];	// only 8KB of memory located at address 0

string ABI[32] = { "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
		"s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6" };

void emitError(string s)
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW) {
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}


void R_Type(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7 = 0, opcode;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	funct7 = (instWord >> 25) & 0x0000007F;

	printPrefix(instPC, instWord);

	switch (funct3) {
	case 0:
		if (funct7 == 0)
			cout << "\tADD\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
		else
			cout << "\tSUB\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
		break;

	case 1:
		cout << "\tSLL\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
		break;

	case 2:
		cout << "\tSLT\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
		break;

	case 3:
		cout << "\tSLTU\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
		break;

	case 4:
		cout << "\tXOR\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
		break;

	case 5:
		if (funct7 == 0)
			cout << "\tSRL\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
		else
			cout << "\tSRA\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
		break;

	case 6:
		cout << "\tOR\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
		break;

	case 7:
		cout << "\tAND\t" << ABI[rd] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
		break;

	default:
		cout << "\tUnknown R Instruction \n";
	}
}

void I_Type(unsigned int instWord)
{
	unsigned int rd, rs1, funct3, opcode;
	unsigned int imm;
	unsigned int temp = 0;
	

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

	printPrefix(instPC, instWord);

	if (opcode == 0x13) {    // I instructions
		switch (funct3) {
		case 0:    cout << "\tADDI\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 1:
			temp = imm & 0x0000001F; //check if this is necessary
			cout << "\tSLLI\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)temp << "\n";
			break;
		case 2:
			cout << "\tSLTI\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 3:
			cout << "\tSLTIU\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 4:
			cout << "\tXORI\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 5:
			temp = (instWord >> 30) & 0x3;
			if (temp == 0)
				cout << "\tSRLI\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			else
				cout << "\tSRAI\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;

		case 6:
			cout << "\tORI\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 7:
			cout << "\tANDI\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;

		default:
			cout << "\tUnknown I Instruction \n";
		}
	}
	else if (opcode == 0x3) {    // I-load instructions
		switch (funct3) {
		case 0:
			cout << "\tLB\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 1:
			cout << "\tLH\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 2:
			cout << "\tLW\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 4:
			cout << "\tLBU\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 5:
			cout << "\tLHU\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
			break;
		default:
			cout << "\tUnknown I Instruction \n";

		}
	}
	else if (opcode == 0x67) {

		cout << "\tJALR\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
	}
	else if (opcode == 0x73)
	{
		cout << "\tECALL\n";
	}
	else {
		cout << "\tUnknown Instruction \n";
	}

}


void S_Type(unsigned int instWord)
{
	unsigned int rs1, rs2, funct3, imm1, imm2, imm = 0;
	

	unsigned int instPC = pc - 4;

	//opcode = instWord & 0x0000007F;
	imm1 = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	//imm2 = (instWord >> 25) & 0x0000007F;
	imm2 = (instWord >> 25) & 0x0000003F;
	imm = imm2;
	imm = (imm2 << 5) | imm1;
	imm = (imm) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));

	printPrefix(instPC, instWord);

	switch (funct3) {
	case 0:
		//Store byte
		cout << "\tSB\t" << ABI[rs2] << ", " << hex << "0x" << (int)imm << " (" << ABI[rs1] << ")" << "\n";
		break;
	case 1:
		//Store half word
		cout << "\tSH\t" << ABI[rs2] << ", " << hex << "0x" << (int)imm << " (" << ABI[rs1] << ")" << "\n";
		break;
	case 2:
		//Store word
		cout << "\tSW\t" << ABI[rs2] << ", " << hex << "0x" << (int)imm << " (" << ABI[rs1] << ")" << "\n";
		break;
	default:
		cout << "\tUnknown S Instruction \n";
	}
}

void U_Type(unsigned int instWord)
{
	unsigned int rd, imm, opcode;
	

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	imm = (instWord >> 12) & 0x7FFFF;
	//imm = imm << 12;
	imm = (imm) | (((instWord >> 31) ? 0xFFF80000 : 0x0));

	printPrefix(instPC, instWord);

	if (opcode == 0x37)
		cout << "\tLUI\t" << ABI[rd] << ", " << hex << "0x" << imm << "\n";
	else if (opcode == 0x17)
		cout << "\tAUIPC\t" << ABI[rd] << ", " << hex << "0x" << imm << "\n";
	else
		cout << "\tUnknown U Instruction \n";

}

void B_Type(unsigned int instWord)
{
	unsigned int rs1, rs2, funct3, imm, temp;
	

	unsigned int instPC = pc - 4;

	//opcode = instWord & 0x0000007F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;

	// Adding immediate bytes in order
	imm = (instWord >> 8) & 0x000000F;
	temp = (instWord >> 25) & 0x0000001F;
	imm = imm + temp;
	temp = (instWord >> 7) & 0x000001;
	imm = imm + temp;
	temp = (instWord >> 30) & 0x00001;
	imm = imm + temp;

	imm = imm << 1;

	printPrefix(instPC, instWord);

	switch (funct3)
	{
	case 0:
		cout << "\tBEQ\t" << ABI[rs1] << ", " << ABI[rs2] << ", " << hex << "0x" << (int)imm << "\n";
		break;

	case 1:
		cout << "\tBNE\t" << ABI[rs1] << ", " << ABI[rs2] << ", " << hex << "0x" << (int)imm << "\n";
		break;

	case 4:
		cout << "\tBLT\t" << ABI[rs1] << ", " << ABI[rs2] << ", " << hex << "0x" << (int)imm << "\n";
		break;

	case 5:
		cout << "\tBGE\t" << ABI[rs1] << ", " << ABI[rs2] << ", " << hex << "0x" << (int)imm << "\n";
		break;

	case 6:
		cout << "\tBLTU\t" << ABI[rs1] << ", " << ABI[rs2] << ", " << hex << "0x" << (int)imm << "\n";
		break;

	case 7:
		cout << "\tBGEU\t" << ABI[rs1] << ", " << ABI[rs2] << ", " << hex << "0x" << (int)imm << "\n";
		break;

	default:
		cout << "\tUnknown B Instruction \n";
	}

}

void J_Type(unsigned int instWord)
{
	unsigned int rd, imm, temp;

	unsigned int instPC = pc - 4;

	//opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	imm = (instWord >> 12) & 0x000000FF;
	temp = (instWord >> 20) & 0x000001;
	imm = imm + temp;
	imm = (instWord >> 21) & 0x00003FF;
	temp = (instWord >> 30) & 0x00001;
	imm = imm + temp;

	imm = imm << 1;
	printPrefix(instPC, instWord);

	cout << "\tJAL\t" << ABI[rd] << ", " << "0x" << hex << imm << "\n";

}
void instDecExec(unsigned int instWord)
{
	unsigned int opcode;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;

	if (opcode == 0x33) {// R Instructions
		R_Type(instWord);
	}
	else if (opcode == 0x13 || opcode == 0x3 || opcode == 0x67 || opcode == 0x73) {// I instructions
		I_Type(instWord);
	}
	else if (opcode == 0x23) {// S Instructions
		S_Type(instWord);
	}
	else if (opcode == 0x63) {// B Instructions
		B_Type(instWord);
	}
	else if (opcode == 0x17 || opcode == 0x37) { // U Instructions
		U_Type(instWord);
	}
	else if (opcode == 0x6F) { // J Instructions
		J_Type(instWord);
	}
	else {
		printPrefix(instPC, instWord);
		cout << "\tUnknown Instruction \n";
	}
}

int main(int argc, char* argv[]) {

	unsigned int instWord = 0;
	unsigned int opcode;
	ifstream inFile;
	ofstream outFile;

	if (argc < 2) emitError("use: rvcdiss <t1.bin>\n");

	inFile.open(argv[1], ios::in | ios::binary | ios::ate);

	if (inFile.is_open())
	{
		int fsize = inFile.tellg();

		inFile.seekg(0, inFile.beg);
		if (!inFile.read((char*)memory, fsize)) emitError("Cannot read from input file\n");

		while (true) {
			instWord = (unsigned char)memory[pc] |
				(((unsigned char)memory[pc + 1]) << 8) |
				(((unsigned char)memory[pc + 2]) << 16) |
				(((unsigned char)memory[pc + 3]) << 24);
			pc += 4;
			opcode = instWord & 0x0000007F;
			if (opcode == 0x0) break; // Stops when opcode is 0
			instDecExec(instWord);
		}
	}
	else emitError("Cannot access input file\n");
}
