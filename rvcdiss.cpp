/*
	This is just a skeleton. It DOES NOT implement all the requirements.
	It only recognizes the RV32I "ADD", "SUB" and "ADDI" instructions only.
	It prints "Unknown Instruction" for all other instructions!

	Usage example:
		$ rvcdiss t1.bin
	should print out:
		0x00000000	0x00100013	ADDI	x0, x0, 0x1
		0x00000004	0x00100093	ADDI	x1, x0, 0x1
		0x00000008	0x00100113	ADDI	x2, x0, 0x1
		0x0000000c	0x001001b3	ADD		x3, x0, x1
		0x00000010	0x00208233	ADD		x4, x1, x2
		0x00000014	0x004182b3	ADD		x5, x3, x4
		0x00000018	0x00100893	ADDI	x11, x0, 0x1
		0x0000001c	0x00028513	ADDI	xa, x5, 0x0
		0x00000020	0x00000073	Unknown Instruction
	References:
	(1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
	(2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <string>
using namespace std;

unsigned int pc = 0x0;

char memory[8 * 1024];	// only 8KB of memory located at address 0

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
	//unsigned int address;

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
			cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		else
			cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		break;

	case 1:
		cout << "\tSLL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		break;

	case 2:
		cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		break;

	case 3:
		cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		break;

	case 4:
		cout << "\tXOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		break;

	case 5:
		if (funct7 == 0)
			cout << "\tSRL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		else
			cout << "\tSRA\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		break;

	case 6:
		cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		break;

	case 7:
		cout << "\tAND\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
		break;

	default:
		cout << "\tUnknown R Instruction \n";
	}
}

void I_Type(unsigned int instWord)
{
	unsigned int rd, rs1, imm, funct3, funct7 = 0, opcode;
	unsigned int temp = 0;
	//unsigned int address;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	imm = (instWord >> 20) & 0x00000FFF;

	printPrefix(instPC, instWord);

	if (opcode == 0x13) {    // I instructions
		switch (funct3) {
		case 0:    cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 1:
			temp = imm & 0x0000001F; //check if this is necessary
			cout << "\tSLLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)temp << "\n";
			break;
		case 2:
			cout << "\tSLTI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 3:
			cout << "\tSLTIU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 4:
			cout << "\tXORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 5:
			temp = (instWord >> 30) & 0x3;
			if (temp == 0)
				cout << "\tSRLI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			else
				cout << "\tSRAI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;

		case 6:
			cout << "\tORI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 7:
			cout << "\tANDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;

		default:
			cout << "\tUnknown I Instruction \n";
		}
	}
	else if (opcode == 0x3) {    // I-load instructions
		switch (funct3) {
		case 0:
			cout << "\tLB\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 1:
			cout << "\tLH\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 2:
			cout << "\tLW\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 4:
			cout << "\tLBU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;
		case 5:
			cout << "\tLHU\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
			break;
		default:
			cout << "\tUnknown I Instruction \n";

		}
	}else if (opcode == 0x67) { 
		
		cout << "\tJALR\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)imm << "\n";
		
	}
	else {
		cout << "\tUnknown Instruction \n";
	}

}


void S_Type(unsigned int instWord)
{
	unsigned int rs1, rs2, funct3, imm1, imm2, imm = 0, opcode;
	//unsigned int address;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	imm1 = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	imm2 = (instWord >> 25) & 0x0000007F;
	imm = imm2;
	imm = (imm2 << 5) | imm1;
	printPrefix(instPC, instWord);

	switch (funct3) {
	case 0:
		//Store byte
		cout << "\tSB\tx" << rs2 << ", " << hex << "0x" << (int)imm << " (x" << rs1 << ")" << "\n";
		break;
	case 1:
		//Store half word
		cout << "\tSH\tx" << rs2 << ", " << hex << "0x" << (int)imm << " (x" << rs1 << ")" << "\n";
		break;
	case 2:
		//Store word
		cout << "\tSW\tx" << rs2 << ", " << hex << "0x" << (int)imm << " (x" << rs1 << ")" << "\n";
		break;
	default:
		cout << "\tUnknown S Instruction \n";
	}
}

void U_Type(unsigned int instWord)
{
	unsigned int rd, imm, opcode;
	//unsigned int address;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	imm = (instWord >> 12) & 0x000FFFFF;
	//imm = imm << 12;

	printPrefix(instPC, instWord);

	if (opcode == 0x37) 
		cout << "\tLUI\tx" << rd << ", " << hex << "0x" << imm << "\n";
	else if (opcode == 0x17)
		cout << "\tAUIPC\tx" << rd << ", " << hex << "0x" << imm << "\n";
	else
		cout << "\tUnknown U Instruction \n";

}

void B_Type(unsigned int instWord)
{
	unsigned int rs1, rs2, funct3, opcode, imm, temp;
	//unsigned int address;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
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
	cout << "\tBEQ\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)imm << "\n";
		break;

	case 1: 
	cout << "\tBNE\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)imm << "\n";
		break;

	case 4: 
	cout << "\tBLT\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)imm << "\n";
		break;

	case 5: 
	cout << "\tBGE\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)imm << "\n";
		break;

	case 6: 
	cout << "\tBLTU\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)imm << "\n";
		break;

	case 7: 
	cout << "\tBGEU\tx" << rs1 << ", x" << rs2 << ", " << hex << "0x" << (int)imm << "\n";
		break;
	
	default:
		cout << "\tUnknown B Instruction \n";
	}

}

void J_Type(unsigned int instWord)
{
	unsigned int rd, imm, opcode, temp;
	//unsigned int address;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	imm = (instWord >> 12) & 0x000000FF;
	temp = (instWord >> 20) & 0x000001;
	imm = imm + temp;
	imm = (instWord >> 21) & 0x00003FF;
	temp = (instWord >> 30) & 0x00001;
	imm = imm + temp;
	
	imm = imm << 1;
	printPrefix(instPC, instWord);

	cout << "\tJAL\tx" << rd << ", " << "0x" << hex << imm << "\n";
	
	
}
void instDecExec(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7 = 0, opcode;
	//unsigned int address;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;

	if (opcode == 0x33) {	// R Instructions
		R_Type(instWord);
	}
	else if (opcode == 0x13 || opcode==0x3 || opcode == 0x67) {	// I instructions
		I_Type(instWord);
	}
	else if (opcode == 0x23) {	// S Instructions
		S_Type(instWord);
	}
	else if (opcode == 0x63){ // B Instructions
		B_Type(instWord);
	}
	else if (opcode == 0x17 || opcode == 0x37){ // U Instructions
		U_Type(instWord);
	}
	else if (opcode == 0x6F){ // J Instructions
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
			if(opcode == 0x0) break; // Stops when opcode is 0
			instDecExec(instWord);
		}
	}
	else emitError("Cannot access input file\n");
}
