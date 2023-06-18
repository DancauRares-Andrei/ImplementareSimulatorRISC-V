#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;
// Definirea tipului de instrucțiune RISC-V
struct Instruction {
    std::string opcode,name;
    int rd, rs1, rs2;
    int imm12, imm20, shamt, offset;
};

// Definirea clasei de fișier de înregistrare
class RegFile {
private:
    std::unordered_map<int, int> registers;

public:
    RegFile() {
        registers.reserve(32);
    }

    void write(int reg, int value) {
        registers[reg] = value;
    }

    int read(int reg){
        return registers[reg];
    }
};
#define MEM_SIZE 10
// Definirea clasei de memorie
class Memory {
private:
    std::vector<int> data;

public:
    Memory() {
        data.resize(MEM_SIZE); // Inițializarea memoriei cu 1024 de locații
    }

    void write(int address, int value) {
        data[address] = value;
    }

    int read(int address) const {
        return data[address];
    }

    int size() const {
        return data.size();
    }
};
void printMemoryContent(const Memory& memory) {
    for (int i = 0; i < memory.size(); i ++) {
         std::cout << memory.read(i) << " ";
    }
    cout<<"\n";
}
// Funcție pentru afișarea conținutului registrilor
void getRegisters(RegFile& regFile) {
    const std::vector<std::string> registerAliases = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };

    const int registersPerLine = 8;
    for (int i = 0; i < 32; i += registersPerLine) {
        for (int j = 0; j < registersPerLine; ++j) {
            int regIndex = i + j;
            if (regIndex < 32) {
                std::cout << registerAliases[regIndex] << ": " << regFile.read(regIndex) << "   ";
            }
        }
        std::cout << std::endl;
    }
}


// Funcția de executare a instrucțiunilor
void executeInstruction(const Instruction& instr, RegFile& regFile, Memory& memory) {
    cout<<"Inainte de executarea instructiunii: "<<instr.name<<"\n";
    getRegisters(regFile);
    std::cout << "Memory content:" << std::endl;
    printMemoryContent(memory);
    std::cout << std::endl;

    // Executare instructiune
    if (instr.opcode == "ADD") {
        regFile.write(instr.rd, regFile.read(instr.rs1) + regFile.read(instr.rs2));
    } else if (instr.opcode == "SUB") {
        regFile.write(instr.rd, regFile.read(instr.rs1) - regFile.read(instr.rs2));
    } else if (instr.opcode == "ADDI") {
        regFile.write(instr.rd, regFile.read(instr.rs1) + instr.imm12);
    } else if (instr.opcode == "LUI") {
        regFile.write(instr.rd, instr.imm20 << 12);
        cout<<(instr.imm20 << 12);
    } else if (instr.opcode == "AND") {
        regFile.write(instr.rd, regFile.read(instr.rs1) & regFile.read(instr.rs2));
    } else if (instr.opcode == "OR") {
        regFile.write(instr.rd, regFile.read(instr.rs1) | regFile.read(instr.rs2));
    } else if (instr.opcode == "XOR") {
        regFile.write(instr.rd, regFile.read(instr.rs1) ^ regFile.read(instr.rs2));
    } else if (instr.opcode == "ANDI") {
        regFile.write(instr.rd, regFile.read(instr.rs1) & instr.imm12);
    } else if (instr.opcode == "ORI") {
        regFile.write(instr.rd, regFile.read(instr.rs1) | instr.imm12);
    } else if (instr.opcode == "XORI") {
        regFile.write(instr.rd, regFile.read(instr.rs1) ^ instr.imm12);
    } else if (instr.opcode == "SLL") {
        regFile.write(instr.rd, regFile.read(instr.rs1) << regFile.read(instr.rs2));
    } else if (instr.opcode == "SRL") {
        regFile.write(instr.rd, regFile.read(instr.rs1) >> regFile.read(instr.rs2));
    } else if (instr.opcode == "SRA") {
        regFile.write(instr.rd, regFile.read(instr.rs1) >> regFile.read(instr.rs2));  // Shift aritmetic la dreapta
    } else if (instr.opcode == "SLLI") {
        regFile.write(instr.rd, regFile.read(instr.rs1) << instr.shamt);
    } else if (instr.opcode == "SRLI") {
        regFile.write(instr.rd, regFile.read(instr.rs1) >> instr.shamt);
    } else if (instr.opcode == "SRAI") {
        regFile.write(instr.rd, regFile.read(instr.rs1) >> instr.shamt);  // Shift aritmetic la dreapta
    } else if (instr.opcode == "LD") {
        regFile.write(instr.rd, memory.read(regFile.read(instr.rs1) + instr.imm12));
    } else if (instr.opcode == "LW") {
        regFile.write(instr.rd, memory.read(regFile.read(instr.rs1) + instr.imm12));
    } else if (instr.opcode == "LH") {
        regFile.write(instr.rd, memory.read(regFile.read(instr.rs1) + instr.imm12));
    } else if (instr.opcode == "LB") {
        regFile.write(instr.rd, memory.read(regFile.read(instr.rs1) + instr.imm12));
    } else if (instr.opcode == "LWU") {
        regFile.write(instr.rd, memory.read(regFile.read(instr.rs1) + instr.imm12));
    } else if (instr.opcode == "LHU") {
        regFile.write(instr.rd, memory.read(regFile.read(instr.rs1) + instr.imm12));
    } else if (instr.opcode == "LBU") {
        regFile.write(instr.rd, memory.read(regFile.read(instr.rs1) + instr.imm12));
    } else if (instr.opcode == "SD") {
        memory.write(regFile.read(instr.rs1) + instr.imm12, regFile.read(instr.rs2));
    } else if (instr.opcode == "SW") {
        memory.write(regFile.read(instr.rs1) + instr.imm12, regFile.read(instr.rs2));
    } else if (instr.opcode == "SH") {
        memory.write(regFile.read(instr.rs1) + instr.imm12, regFile.read(instr.rs2));
    } else if (instr.opcode == "SB") {
        memory.write(regFile.read(instr.rs1) + instr.imm12, regFile.read(instr.rs2));
    }
    std::cout << "Dupa executare: "<< std::endl;
    getRegisters(regFile);
    printMemoryContent(memory);
}

int main() {
    RegFile regFile;
    Memory memory;

    Instruction instr1 = { "ADDI","addi ra, zero, 10", 1, 0, 0, 10, 0, 0, 0 };
    Instruction instr2 = { "ADDI", "addi sp, ra, 20",2, 1, 0, 20, 0, 0, 0 };
    Instruction instr3 = { "ADD", "add gp, ra, sp",3, 1, 2, 0, 0, 0, 0 };
    Instruction instr4 = { "SUB","sub tp, gp, sp", 4, 3, 2, 0, 0, 0, 0 };
    Instruction instr5 = { "LUI","lui t0, 100" ,5, 0, 0, 0, 100, 0, 0 };
    Instruction instr6 = { "ADDI","addi t1, zero, 50" ,6, 0, 0, 50, 0, 0, 0 };
    Instruction instr7 = { "AND","and t2, t0, t1" ,7, 5, 6, 0, 0, 0, 0 };
    Instruction instr8 = { "ORI", "ori t3, t2, 255",8, 7, 255, 0, 0, 0, 0 };
    Instruction instr9 = { "SLL", "sll t4, t3, 4",9, 8, 4, 0, 0, 0, 0 };
    Instruction instr10 = { "LD","ld a0, 0(zero)" ,10, 0, 0, 0, 0, 0, 0 };
    Instruction instr11 = { "SD", "sd gp, 0(zero)",0, 0, 3, 0, 0, 0, 0 };

    executeInstruction(instr1, regFile, memory);
    cout<<"\n\n";
    executeInstruction(instr2, regFile, memory);
    cout<<"\n\n";
    executeInstruction(instr3, regFile, memory);
    cout<<"\n\n";
    executeInstruction(instr4, regFile, memory);
    cout<<"\n\n";
    executeInstruction(instr5, regFile, memory);
    cout<<"\n\n";
    executeInstruction(instr6, regFile, memory);
    cout<<"\n\n";
    executeInstruction(instr7, regFile, memory);
    cout<<"\n\n";
    executeInstruction(instr8, regFile, memory);
    cout<<"\n\n";
    executeInstruction(instr9, regFile, memory);
    cout<<"\n\n";
    executeInstruction(instr11, regFile, memory);
    cout<<"\n\n";
    executeInstruction(instr10, regFile, memory);
    
    return 0;
}

