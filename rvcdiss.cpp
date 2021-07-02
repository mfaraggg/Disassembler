#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <string>
using namespace std;

unsigned int pc = 0x0;

char memory[8 * 1024];	// only 8KB of memory located at address 0

//all ABI register names are added into array of strings and will be called when outputting
string ABI[32] = { "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
		"s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6" };

void emitError(string s) //used to output an error message
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW) { //outputs the machine code in hexadecimal and the PC address
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

void compressedInst(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, opcode;
	unsigned int imm, imm1, imm2;
	unsigned int instPC = pc - 4; //

	opcode = instWord & 0x3; 
	funct3 = (instWord >> 12) & 0x7;
	printPrefix(instPC, instWord);

	if (opcode == 0)
	{
		imm1 = (instWord >> 5) & 0x1;
		imm2 = (instWord >> 10) & 0x7;
		imm = (imm1 << 3) | (imm2);
		imm = (imm << 1) | ((instWord >> 6) & 0x1);
		rd = (instWord >> 2) & 0x7;
		rs1 = (instWord >> 7) & 0x7;
		switch(funct3)
		{
			case 2:
				cout << "\tC.LW\t" << ABI[rd] << ", " << ABI[rs1]  << ", " << hex << "0x" << (int)imm << "\n" ;
				break;

			case 6:
				cout << "\tC.SW\t" << ABI[rd] << ", " << hex << "0x" << (int)imm << " (" << ABI[rs1] << ")" << "\n";
				//rd is rs2
				break;
			default:
				cout << "\tUnknown Compressed Instruction \n";
		}
	}
	else if (opcode == 1)
	{
		switch(funct3)
		{
			case 0: // C.ADDI
				imm1 = ((instWord >> 2) & 0x1F);
				imm2 = ((instWord >> 12) & 0x1);
				imm = (imm2 << 5) | imm1;
				if (imm==0)
					break;
				rs1 = (instWord >> 7) & 0x1F;
				if (rs1 == 0)
					break;
				cout << "\tC.ADDI\t" << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
				break;

			case 1: //C.JAL
				imm1 = (instWord >> 3) & 0x7;
				imm2 = (instWord >> 11) & 0x1;
				imm = (imm2 << 3) | (imm1); // adding Imm[3:1] to Imm[4]
				imm = (((instWord >> 2)&0x1) << 4)| imm; // adding Imm[5]
				imm = (((instWord >> 7) & 0x1) << 5) | imm;// adding Imm[6]
				imm = (((instWord >> 6 )& 0x1) << 6) | imm; // adding Imm[7]
				imm = (((instWord >> 9) & 0x3) << 8) | imm; // adding Imm[9:8]
				imm = (((instWord >> 8) & 0x1) << 9) | imm; // adding Imm[10]
				imm = (((instWord >> 12) & 0x1) << 10) | imm ; // adding Imm[11]
				cout << "\tC.JAL\t" << "0x" << hex << imm << "\n";
				break;
			
			case 3:
			
				imm1 = (instWord >> 2) & 0x1F;
				imm2 = (instWord >> 12) & 0x1;
				imm = (imm2 << 5) | imm1;
				if (imm==0)
					break;
				rd = (instWord >> 7) & 0x1F;
				if ((rd == 0) || (rd == 2))
					break;
				cout << "\tC.LUI\t" << ABI[rd] << ", " << hex << "0x" << imm << "\n";
				break;	

			case 4: 
				rs1 = (instWord >> 7) & 0x7;
				int temp = (instWord >> 10) & 0x3;
				imm1 = ((instWord >> 2) & 0x1F);
				imm2 = ((instWord >> 12) & 0x1);
				imm = (imm2 << 5) | imm1;
				if (temp == 2)
				{
					cout << "\tC.ANDI\t" << ABI[rs1] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
					break;
				}
				if (imm == 0)
					break;
				rs1 = (instWord >> 7) & 0x1F;
				if (temp == 0)
				{
					cout << "\tC.SRLI\t" << ABI[rs1] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
					break;
				}
				else if (temp == 1)
				{
					cout << "\tC.SRAI\t" << ABI[rs1] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
					break;
				}
				else
				{
					rs2 = ((instWord >> 2) & 0x7);
					temp = ((instWord >> 5) & 0x3);
					switch (temp)
					{
						case 0:
							cout << "\tC.SUB\t" << ABI[rs1] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
							break;
						case 1:
							cout << "\tC.XOR\t" << ABI[rs1] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
							break;
						case 2:
							cout << "\tC.OR\t" << ABI[rs1] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
							break;
						case 3:
							cout << "\tC.AND\t" << ABI[rs1] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
							break;
						default:
							cout << "Unknown Compressed Instruction" << endl;
							break;
					}
				}
		}
	}
	else if (opcode == 2)
	{
		switch(funct3)
		{
			case 0: // C.SLLI
				imm1 = ((instWord >> 2) & 0x1F);
				imm2 = ((instWord >> 12) & 0x1);
				imm = (imm2 << 5) | imm1;
				if (imm==0)
					break;
				rs1 = (instWord >> 7) & 0x1F;
				if (rs1 == 0)
					break;
				cout << "\tC.SLLI\t" << ABI[rs1] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
				break;
			case 4:
				rs1 = (instWord >> 7) & 0x1F;
				rs2 = (instWord >> 2) & 0x1F;
				if (rs1 == 0)
				{
					cout << "\tC.EBREAK\t" << endl;
					break;
				}
				else if (rs2 == 0)
				{
					cout << "\tC.JALR\t" << ABI[rs1] << "\n"; 
					break;
				}
				else
				{
					cout << "\tC.ADD\t" << ABI[rs1] << ", " << ABI[rs1] << ", " << ABI[rs2] << "\n";
					break;
				}
		}
	}

}
void R_Type(unsigned int instWord) //function for all R-Type instructions
{
	unsigned int rd, rs1, rs2, funct3, funct7 = 0, opcode;

	unsigned int instPC = pc - 4; //

	opcode = instWord & 0x0000007F; //inserts first 7 bits in opcode
	rd = (instWord >> 7) & 0x0000001F; //next 5 bits
	funct3 = (instWord >> 12) & 0x00000007; //next 3 bits
	rs1 = (instWord >> 15) & 0x0000001F; //next 5 bits
	rs2 = (instWord >> 20) & 0x0000001F; //next 5 bits
	funct7 = (instWord >> 25) & 0x0000007F; //final 7 bits

	printPrefix(instPC, instWord);

	switch (funct3) { //all r type functions depend on funct3
	case 0:
		if (funct7 == 0) //funct7 is needed since 2 instructions have same funct3
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
		if (funct7 == 0) //conflicting funct3 codes need an if to compare funct7
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

void I_Type(unsigned int instWord) //I Type instruction set
{
	unsigned int rd, rs1, funct3, opcode;

	unsigned int imm; //holds the immediate value
	unsigned int temp = 0, temp2 = 0;


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

			temp = (instWord >> 30) & 0x3; //compares the last 3 digits since they differ in SRLI and SRAI
			temp2 = imm & 0x0000001F; //only the first 5 bits are represented in the assembly
			if (temp == 0)
				cout << "\tSRLI\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)temp2 << "\n";
			else
				cout << "\tSRAI\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)temp2 << "\n";
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
	else if (opcode == 0x67) { //JALR

		cout << "\tJALR\t" << ABI[rd] << ", " << ABI[rs1] << ", " << hex << "0x" << (int)imm << "\n";
	}
	else if (opcode == 0x73) //ecall
	{
		cout << "\tECALL\n";
	}
	else {
		cout << "\tUnknown Instruction \n";
	}

}


void S_Type(unsigned int instWord) //function for S Type instruction set
{
	unsigned int rs1, rs2, funct3, imm1, imm2, imm = 0;

	unsigned int instPC = pc - 4;

	//opcode = instWord & 0x0000007F;
	imm1 = (instWord >> 7) & 0x0000001F; //stores least siginigcant part of the imm
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	//imm2 = (instWord >> 25) & 0x0000007F;
	imm2 = (instWord >> 25) & 0x0000003F; //stores most signifcant half
	imm = imm2;
	imm = (imm2 << 5) | imm1; //concatenates the 2 values
	imm = (imm) | (((instWord >> 31) ? 0xFFFFF800 : 0x0)); //makes sure values will appear as signed

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

void U_Type(unsigned int instWord) //instruction set for U type
{
	unsigned int rd, imm, opcode;

	unsigned int instPC = pc - 4;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	imm = (instWord >> 12) & 0x7FFFF;
	//imm = imm << 12;
	imm = (imm) | (((instWord >> 31) ? 0xFFF80000 : 0x0));

	printPrefix(instPC, instWord);

	if (opcode == 0x37) //the two U type functions have different opcodes
		cout << "\tLUI\t" << ABI[rd] << ", " << hex << "0x" << imm << "\n";
	else if (opcode == 0x17)
		cout << "\tAUIPC\t" << ABI[rd] << ", " << hex << "0x" << imm << "\n";
	else
		cout << "\tUnknown U Instruction \n";

}

void B_Type(unsigned int instWord) //all B type functions
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

void J_Type(unsigned int instWord) //J Type instruction set
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
void instDecExec(unsigned int instWord) //main function that calls all other functions(R,I,J,U,B,S types)
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
