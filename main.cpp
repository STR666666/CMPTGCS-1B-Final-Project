#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct {
  uint32_t op;
  uint32_t rs;
  uint32_t rt;
  uint32_t rd;
  uint32_t shamt;
  uint32_t funct;
  uint32_t imm;
  uint32_t address;
} TYPE;

struct regName{
  int num;
}regs[32];

int value; //holds temporary value
int pc = 0; //program counter
uint32_t dmem[32]; //data memory
uint32_t imem[32];

//initialize all the registers to be 0
void setUpReg()
{
	for (int i = 0; i < 32; i++)
	{
		regs[i].num = 0;
	}
}

//Decode
void EXEC_ADD(){
  regs[TYPE.rd>>11].num = regs[TYPE.rs>>21].num + regs[TYPE.rt>>16].num;
  value = regs[TYPE.rd>>11].num;
}

void EXEC_AND(){
  regs[TYPE.rd>>11].num = regs[TYPE.rs>>21].num & regs[TYPE.rt>>16].num;
  value = regs[TYPE.rd>>11].num;
}

void EXEC_OR(){
  regs[TYPE.rd>>11].num = regs[TYPE.rs>>21].num | regs[TYPE.rt>>16].num;
  value = regs[TYPE.rd>>11].num;
}

void EXEC_NOR(){
  regs[TYPE.rd>>11].num = !(regs[TYPE.rs>>21].num | regs[TYPE.rt>>16].num);
  value = regs[TYPE.rd>>11].num;
}

void EXEC_SLL(){
  regs[TYPE.rd>>11].num = regs[TYPE.rt>>16].num << regs[TYPE.shamt>>6].num;
  value = regs[TYPE.rd>>11].num;
}

void EXEC_ADDI(){
  regs[TYPE.rt>>16].num = regs[TYPE.rs>>21].num + TYPE.imm;
  value = regs[TYPE.rt>>16].num;
}

void EXEC_ANDI(){
  regs[TYPE.rt>>16].num = regs[TYPE.rs>>21].num & TYPE.imm;
  value = regs[TYPE.rt>>16].num;
}

void EXEC_ORI(){
  regs[TYPE.rt>>16].num = regs[TYPE.rs>>21].num | TYPE.imm;
  value = regs[TYPE.rt>>16].num;
}

void EXEC_LW(){
  regs[TYPE.rt>>16].num = dmem[regs[TYPE.rs>>21].num + TYPE.imm];
}

void EXEC_SW(){
  dmem[regs[TYPE.rs>>21].num + TYPE.imm] = regs[TYPE.rt>>16].num;
}

//void EXEC_BEQ(){
  //if (regs[TYPE.rs>>21].num == regs[TYPE.rt>>16].num) {
   // pc += (TYPE.imm << 2) + 4;
   // fetch();
  //}
//}

//void EXEC_JUMP(unsigned int input){
  //pc = (pc & 0xF0000000) | (input << 2);
//}

//LW, SW, ADD, ADDI, AND(I), OR(I), NOR, SLL
void decode (unsigned int input){
  TYPE.op = input >> 26;
  TYPE.rs = (input >> 21) & 0x1F;
  TYPE.rt = (input >> 16) & 0x1F;
  TYPE.rd = (input >> 11) & 0x1F;
  TYPE.shamt = (input >> 6) & 0x1F;
  TYPE.imm = 0xFFFF & input;
  TYPE.funct = input & 0x3F;
  TYPE.address = input & 0xF0000000;

  if(input == 0x20840001){ // if addi $a0, $a0, 1, then print.
    decode(imem[pc+4]);
    cout << value;
  }
  //R-type
  else if(TYPE.op == 0 && TYPE.funct == 0x20){
    EXEC_ADD();
  }else if(TYPE.op == 0 && TYPE.funct == 0x24){
    EXEC_AND();
  }else if(TYPE.op == 0 && TYPE.funct == 0x25){
    EXEC_OR();
  }else if(TYPE.op == 0 && TYPE.funct == 0x27){
    EXEC_NOR();
  }else if(TYPE.op == 0 && TYPE.funct == 0x00){
    EXEC_SLL();

  //I-type
  }else if(TYPE.op == 0x23){
    EXEC_LW();
  }else if(TYPE.op == 0x2b){
    EXEC_SW();
  }else if(TYPE.op == 0x8){
    EXEC_ADDI();
  }else if(TYPE.op == 0xc){
    EXEC_ANDI();
  }else if(TYPE.op == 0xd){
    EXEC_ORI();
  }
  //else if(TYPE.op == 0x4){
    //EXEC_BEQ();
  //}
  //J-Type
  //else if(TYPE.op == 0x2){
    //EXEC_JUMP();
  //}
}

int main() {
  setUpReg(); //initialize all the registers

  //open a file
  ifstream inFS;
  unsigned int fileNum;
  inFS.open("test.hex");

  inFS >> hex >> fileNum;
  int pc = 0;
  while (!inFS.fail()) {
    imem[pc] = fileNum; //memory location
    pc += 4;
    inFS >> hex >> fileNum;
  }

  int i;
  for(i = 0; i < pc; i += 4){
    if(imem[i] == 0xc){//syscall
      break;
    }else{
      decode(imem[i]);//if not, then continue
    }
  }
  return 0;
}

//Test Example:
//0x21080005 //ADDI $t0 $t0 5
//0xad280000 //SW $t0 0($t1)
//0x8d2a0000 //LW $t2 0($t1)
//0x20840001 //ADDI $a0 $a0 1
//0x214a0000 //ADDI $t2 $t2 0
//0x0000000c //syscall
//resulting value is 5