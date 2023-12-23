#include <stdio.h>

#include <stdint.h>

#define MEM_SIZE 176
#define DATA_MEM_SIZE 100
#define REGISTERS_NUM 32

typedef struct {
  uint64_t pc;
  uint64_t regdif;
  int Jump;
  uint64_t jumpAddress;
  uint64_t registers[REGISTERS_NUM];
  uint64_t dataMemory[DATA_MEM_SIZE];
  uint64_t memory[MEM_SIZE];
  int memSize;
  uint64_t result;
}
RISCVProcessor;

uint64_t sign_extend(uint64_t number, int bits) {
  uint64_t mask = (1ULL << bits) - 1;
  uint64_t sign_bit = (number) & (1ULL << (bits - 1));
  if (sign_bit) {
    return number | (~mask & 0xFFFFFFFFFFFFFFFFULL);
  }
  return number;
}

uint64_t limit16bits(uint64_t number) {
  return number & 0xFFFFULL;
}

void initialize_instructions(RISCVProcessor * processor,
  const char * file) {
  FILE * f = fopen(file, "r");
  processor -> memSize = 0;
  for (int i = 0; i < MEM_SIZE; ++i) {
    if (fscanf(f, "%lx", & (processor -> memory[i])) > 0)
      processor -> memSize++;
  }
  fclose(f);
}

void reset(RISCVProcessor * processor) {
  processor -> pc = 0;
  processor -> regdif = 0;
  processor -> Jump = 0;
  processor -> jumpAddress = 0;
  processor -> memSize = 0;
  for (int i = 0; i < REGISTERS_NUM; ++i) {
    processor -> registers[i] = 0;
  }
  for (int i = 0; i < DATA_MEM_SIZE; ++i) {
    processor -> dataMemory[i] = 0;
  }
  for (int i = 0; i < MEM_SIZE; ++i) {
    processor -> memory[i] = 0;
  }
  processor -> result = 0;
}

uint64_t execute(RISCVProcessor * processor) {
  uint64_t instruction;
  uint64_t opcode;
  uint64_t funct3;
  uint64_t funct7;
  uint64_t imm12;
  uint64_t shamt;
  uint64_t rs1;
  uint64_t rs2;
  uint64_t rd;
  instruction = (processor -> memory[processor -> pc] << 24) + (processor -> memory[processor -> pc + 1] << 16) + (processor -> memory[processor -> pc + 2] << 8) + processor -> memory[processor -> pc + 3];
  opcode = instruction & 0b1111111;
  funct3 = (instruction >> 12) & 0b111;
  funct7 = (instruction >> 25) & 0b1111111;
  imm12 = (instruction >> 20) & 0b111111111111;
  shamt = (instruction >> 20) & 0b11111;
  rs1 = (instruction >> 15) & 0b11111;
  rs2 = (instruction >> 20) & 0b11111;
  rd = (instruction >> 7) & 0b11111;
  switch (opcode) {
  case 0b0110011:
    switch (funct3) {
    case 0b000:
      if (funct7 == 0b0000000) { //ADD
        processor -> registers[rd] = processor -> registers[rs1] + processor -> registers[rs2];
        processor -> result = limit16bits(processor -> registers[rd]);
      } else if (funct7 == 0b0100000) { //SUB
        processor -> registers[rd] = processor -> registers[rs1] - processor -> registers[rs2];
        processor -> result = limit16bits(processor -> registers[rd]);
      } else {
        processor -> result = 0;
      }
      break;
    case 0b001: //SLL
      processor -> registers[rd] = processor -> registers[rs1] << (processor -> registers[rs2] & 0b11111);
      processor -> result = limit16bits(processor -> registers[rd]);
      break;
    case 0b010: //SLT
      processor -> regdif = processor -> registers[rs1] - processor -> registers[rs2];
      processor -> registers[rd] = processor -> regdif >> 63;
      processor -> result = limit16bits(processor -> registers[rd]);
      break;
    case 0b011: //SLTU
      processor -> registers[rd] = (processor -> registers[rs1] < processor -> registers[rs2]);
      processor -> result = limit16bits(processor -> registers[rd]);
      break;
    case 0b100: //XOR
      processor -> registers[rd] = processor -> registers[rs1] ^ processor -> registers[rs2];
      processor -> result = limit16bits(processor -> registers[rd]);
      break;
    case 0b101: //SRL, SRA
      if (funct7 == 0b0000000) { //SRL
        processor -> registers[rd] = processor -> registers[rs1] >> (processor -> registers[rs2] & 0b11111);
        processor -> result = limit16bits(processor -> registers[rd]);
      } else if (funct7 == 0b0100000) { //SRA
        processor -> registers[rd] = processor -> registers[rs1] >> (processor -> registers[rs2] & 0b11111);
        processor -> result = limit16bits(processor -> registers[rd]);
      } else {
        processor -> result = 0;
      }
      break;
    case 0b110: //OR
      processor -> registers[rd] = processor -> registers[rs1] | processor -> registers[rs2];
      processor -> result = limit16bits(processor -> registers[rd]);
      break;
    case 0b111: //AND
      processor -> registers[rd] = processor -> registers[rs1] & processor -> registers[rs2];
      processor -> result = limit16bits(processor -> registers[rd]);
      break;
    default:
      processor -> result = 0;
      break;
    }
    break;
  case 0b0010011:
    switch (funct3) {
    case 0b000: { //ADDI
      processor -> registers[rd] = processor -> registers[rs1] + sign_extend(imm12, 12);
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b001: { //SLLI
      processor -> registers[rd] = processor -> registers[rs1] << shamt;
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b010: { //SLTI
      processor -> registers[rd] = (processor -> registers[rs1] - sign_extend(imm12, 12)) >> 63;
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b011: { //SLTIU
      processor -> registers[rd] = processor -> registers[rs1] < sign_extend(imm12, 12);
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b100: { //XORI
      processor -> registers[rd] = processor -> registers[rs1] ^ sign_extend(imm12, 12);
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b110: { //ORI
      processor -> registers[rd] = processor -> registers[rs1] | sign_extend(imm12, 12);
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b111: { //ANDI
      processor -> registers[rd] = processor -> registers[rs1] & sign_extend(imm12, 12);
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b101:
      if (funct7 == 0b0000000) { //SRLI
        processor -> registers[rd] = processor -> registers[rs1] >> shamt;
        processor -> result = limit16bits(processor -> registers[rd]);
      } else if (funct7 == 0b0100000) { //SRAI
        processor -> registers[rd] = processor -> registers[rs1] >> shamt;
        processor -> result = limit16bits(processor -> registers[rd]);
      } else {
        processor -> result = 0;
      }
      break;
    default:
      processor -> result = 0;
      break;
    }
    break;
  case 0b0000011:
    switch (funct3) {
    case 0b000: { //LB
      processor -> registers[rd] = sign_extend(processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)] & 0xFF, 8);
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b001: { //LH
      processor -> registers[rd] = sign_extend(processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)] & 0xFFFF, 16);
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b010: { //LW
      processor -> registers[rd] = sign_extend(processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)] & 0xFFFFFFFF, 32);
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b011: { //LD
      processor -> registers[rd] = processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)];
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b100: { //LBU
      processor -> registers[rd] = processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)] & 0xFF;
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b101: { //LHU
      processor -> registers[rd] = processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)] & 0xFFFF;
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    case 0b110: { //LWU
      processor -> registers[rd] = processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)] & 0xFFFFFFFF;
      processor -> result = limit16bits(processor -> registers[rd]);
    }
    break;
    default:
      processor -> result = 0;
      break;
    }
    break;
  case 0b0100011:
    imm12 = ((instruction >> 25) << 7) | ((instruction >> 7) & 0b11111);
    switch (funct3) {
    case 0b000: { //SB
      processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)] = processor -> registers[rs2] & 0xFF;
      processor -> result = limit16bits(processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)]);
    }
    break;
    case 0b001: { //SH
      processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)] = processor -> registers[rs2] & 0xFFFF;
      processor -> result = limit16bits(processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)]);
    }
    break;
    case 0b010: { //SW
      processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)] = processor -> registers[rs2] & 0xFFFFFFFF;
      processor -> result = limit16bits(processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)]);
    }
    break;
    case 0b011: { //SD
      processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)] = processor -> registers[rs2];
      processor -> result = limit16bits(processor -> dataMemory[processor -> registers[rs1] + sign_extend(imm12, 12)]);
    }
    break;
    default:
      processor -> result = 0;
      break;
    }
    break;
  case 0b0110111: //LUI
    imm12 = instruction >> 12;
    processor -> registers[rd] = sign_extend(imm12, 20) << 12;
    processor -> result = limit16bits(processor -> registers[rd]);
    break;
  case 0b0010111: //AUIPC
    imm12 = instruction >> 12;
    processor -> registers[rd] = processor -> pc + (sign_extend(imm12, 20) << 12);
    processor -> result = limit16bits(processor -> registers[rd]);
    break;
  case 0b1101111: //JAL
    processor -> registers[rd] = processor -> pc + 4;
    processor -> Jump = 1;
    imm12 = (((instruction >> 31) << 19) | (((instruction >> 12) & 0xFF) << 11) | (instruction >> 21) & 0x3FF) << 1;
    processor -> jumpAddress = processor -> pc + sign_extend(imm12, 20);
    processor -> result = limit16bits(processor -> jumpAddress);
    break;
  case 0b1100111: //JALR
    processor -> registers[rd] = processor -> pc + 4;
    processor -> Jump = 1;
    processor -> jumpAddress = processor -> registers[rs1] + sign_extend(imm12 / 2 * 2, 12);
    processor -> result = limit16bits(processor -> jumpAddress);
    break;
  case 0b1100011:
    imm12 = ((instruction >> 31) << 12) | (((instruction >> 7) & 1) << 11) | (((instruction >> 25) & 0b111111) << 5) | ((instruction >> 8) & 0xf) << 1;
    switch (funct3) {
    case 0b000: //BEQ
      processor -> Jump = processor -> registers[rs1] == processor -> registers[rs2];
      processor -> jumpAddress = processor -> pc + sign_extend(imm12, 12);
      processor -> result = processor -> Jump ? limit16bits(processor -> jumpAddress) : 0;
      break;
    case 0b001: //BNE
      processor -> Jump = processor -> registers[rs1] != processor -> registers[rs2];
      processor -> jumpAddress = processor -> pc + sign_extend(imm12, 12);
      processor -> result = processor -> Jump ? limit16bits(processor -> jumpAddress) : 0;
      break;
    case 0b100: //BLT
      processor -> Jump = (processor -> registers[rs1] - processor -> registers[rs2]) >> 63;
      processor -> jumpAddress = processor -> pc + sign_extend(imm12, 12);
      processor -> result = processor -> Jump ? limit16bits(processor -> jumpAddress) : 0;
      break;
    case 0b101: //BGE
      processor -> Jump = 1 - ((processor -> registers[rs1] - processor -> registers[rs2]) >> 63);
      processor -> jumpAddress = processor -> pc + sign_extend(imm12, 12);
      processor -> result = processor -> Jump ? limit16bits(processor -> jumpAddress) : 0;
      break;
    case 0b110: //BLTU
      processor -> Jump = processor -> registers[rs1] < processor -> registers[rs2];
      processor -> jumpAddress = processor -> pc + sign_extend(imm12, 12);
      processor -> result = processor -> Jump ? limit16bits(processor -> jumpAddress) : 0;
      break;
    case 0b111: //BGEU
      processor -> Jump = processor -> registers[rs1] >= processor -> registers[rs2];
      processor -> jumpAddress = processor -> pc + sign_extend(imm12, 12);
      processor -> result = processor -> Jump ? limit16bits(processor -> jumpAddress) : 0;
      break;
    default:
      processor -> result = 0;
      break;
    }
    break;
  default:
    processor -> result = 0;
    break;
  }

  if (processor -> Jump) {
    processor -> Jump = 0;
    processor -> pc = processor -> jumpAddress;
  } else {
    processor -> pc += 4;
  }
  return processor -> result;
}

void run(RISCVProcessor * processor,
  const uint64_t * expected_result_list, int expected_result_size) {
  uint64_t result_list[expected_result_size];
  int result_index = 0;
  while (processor -> pc < processor -> memSize) {
    result_list[result_index++] = execute(processor);
  }
  int are_results_equal = 1;
  for (int i = 0; i < expected_result_size; ++i) {
    if (expected_result_list[i] != result_list[i]) {
      are_results_equal = 0;
      break;
    }
  }
  if (are_results_equal) {
    printf("Lista produsa in cadrul executiei corespunde cu cea asteptata. O afisez.\n");
    for (int i = 0; i < expected_result_size; ++i) {
      printf("%#lx ", result_list[i]);
    }
    printf("\n");
  } else {
    printf("Ceva e gresit. Afisez ambele liste pentru verificare.\n");
    for (int i = 0; i < expected_result_size; ++i) {
      printf("Asteptat: %#lx, Obtinut: %#lx\n", expected_result_list[i], result_list[i]);
    }
  }
}

int main() {
  RISCVProcessor processor;
  reset( & processor);
  initialize_instructions( & processor, "instructions_add_load.mem");
  uint64_t expected_result_list[] = {
    0x5,
    0x7,
    0xc,
    0xc,
    0x0,
    0xc
  };
  int expected_result_size = sizeof(expected_result_list) / sizeof(expected_result_list[0]);
  run( & processor, expected_result_list, expected_result_size);
  reset( & processor);
  initialize_instructions( & processor, "instructions_counter.mem");
  uint64_t expected_result_list1[] = {
    0xa,
    0x1,
    0x9,
    0x8,
    0x8,
    0x8,
    0x7,
    0x8,
    0x6,
    0x8,
    0x5,
    0x8,
    0x4,
    0x8,
    0x3,
    0x8,
    0x2,
    0x8,
    0x1,
    0x8,
    0x0,
    0x0
  };
  expected_result_size = sizeof(expected_result_list1) / sizeof(expected_result_list1[0]);
  run( & processor, expected_result_list1, expected_result_size);
  reset( & processor);
  initialize_instructions( & processor, "instructions_mare.mem");
  uint64_t expected_result_list2[] = {
    0xa,
    0x1e,
    0x28,
    0xa,
    0x1000,
    0x32,
    0x0,
    0x1000,
    0x1000,
    0x0,
    0x6,
    0xf,
    0x3,
    0x18,
    0x3,
    0x3c,
    0x28,
    0x28,
    0x0,
    0x1,
    0x7,
    0x0,
    0x1,
    0xa,
    0x3,
    0xf,
    0x1000,
    0x3,
    0xf,
    0x3,
    0xf,
    0x28,
    0x1000,
    0x8c,
    0x408c,
    0x0,
    0x98,
    0x0,
    0xa0,
    0x0,
    0xa8
  };
  expected_result_size = sizeof(expected_result_list2) / sizeof(expected_result_list2[0]);
  run( & processor, expected_result_list2, expected_result_size);
  reset( & processor);
  initialize_instructions( & processor, "instructions_paritate.mem");
  uint64_t expected_result_list3[] = {
    0x9,
    0x1,
    0x0,
    0x3e7,
    0x18
  };
  expected_result_size = sizeof(expected_result_list3) / sizeof(expected_result_list3[0]);
  run( & processor, expected_result_list3, expected_result_size);
  reset( & processor);
  initialize_instructions( & processor, "instructions_sll_xor.mem");
  uint64_t expected_result_list4[] = {
    0xa,
    0x2,
    0x28,
    0x8,
    0x8,
    0x2,
    0x4,
    0x5,
    0x4,
    0x2,
    0x1,
    0x34,
    0x3000,
    0x0,
    0x1,
    0xfffe,
    0x0,
    0x1,
    0x6,
    0x3,
    0x1,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0
  };
  expected_result_size = sizeof(expected_result_list4) / sizeof(expected_result_list4[0]);
  run( & processor, expected_result_list4, expected_result_size);
  reset( & processor);
  initialize_instructions( & processor, "instructions_stocari_extrageri.mem");
  uint64_t expected_result_list5[] = {
    0xffff,
    0x1,
    0xff,
    0xfffe,
    0xfffe,
    0xfffd,
    0xfffd,
    0x24,
    0x2c,
    0xffff,
    0xfffd,
    0xfffe,
    0xfd,
    0xff,
    0xfffe
  };
  expected_result_size = sizeof(expected_result_list5) / sizeof(expected_result_list5[0]);
  run( & processor, expected_result_list5, expected_result_size);
  reset( & processor);
  initialize_instructions( & processor, "instructions_subrutina.mem");
  uint64_t expected_result_list6[] = {
    0x0,
    0x10,
    0x4,
    0x8,
    0x5,
    0x3,
    0x7,
    0xa,
    0x8,
    0x0,
    0x8,
    0x0,
    0x34,
    0x0
  };
  expected_result_size = sizeof(expected_result_list6) / sizeof(expected_result_list6[0]);
  run( & processor, expected_result_list6, expected_result_size);
  reset( & processor);
  initialize_instructions( & processor, "instructions_suma.mem");
  uint64_t expected_result_list7[] = {
    0x5,
    0x1,
    0x1,
    0x2,
    0x8,
    0x3,
    0x3,
    0x8,
    0x6,
    0x4,
    0x8,
    0xa,
    0x5,
    0x0,
    0x0
  };
  expected_result_size = sizeof(expected_result_list7) / sizeof(expected_result_list7[0]);
  run( & processor, expected_result_list7, expected_result_size);
  return 0;
}
