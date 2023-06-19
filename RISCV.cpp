#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

std::unordered_map<std::string, int> registers = {
    {"zero", 0},
    {"ra", 0},
    {"sp", 0},
    {"gp", 0},
    {"tp", 0},
    {"t0", 0},
    {"t1", 0},
    {"t2", 0},
    {"t3", 0},
    {"t4", 0},
    {"t5", 0},
    {"t6", 0},
    {"s0", 0},
    {"s1", 0},
    {"s2", 0},
    {"s3", 0},
    {"a0", 0}
};

std::vector<int> memory(1000, 0);

void executeInstructions() {
    std::ifstream file("instructions.txt");
    if (!file) {
        std::cout << "Eroare la deschiderea fișierului de instrucțiuni.\n";
        return;
    }

    std::string instruction;
    while (std::getline(file, instruction)) {
        std::string opcode, rd, rs1, rs2;
        int imm;

        std::istringstream iss(instruction);
        iss >> opcode;

        if (opcode == "add") {
            iss >> rd >> rs1 >> rs2;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] + registers[rs2];
        }
        else if (opcode == "addi") {
            iss >> rd >> rs1 >> imm;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] + imm;
        }
        else if (opcode == "sub") {
            iss >> rd >> rs1 >> rs2;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] - registers[rs2];
        }
        else if (opcode == "lui") {
            iss >> rd >> imm;
            registers[rd.substr(0, rd.length() - 1)] = imm << 12;
        }
        else if (opcode == "and") {
            iss >> rd >> rs1 >> rs2;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] & registers[rs2];
        }
        else if (opcode == "or") {
            iss >> rd >> rs1 >> rs2;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] | registers[rs2];
        }
        else if (opcode == "xor") {
            iss >> rd >> rs1 >> rs2;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] ^ registers[rs2];
        }
        else if (opcode == "andi") {
            iss >> rd >> rs1 >> imm;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] & imm;
        }
        else if (opcode == "ori") {
            iss >> rd >> rs1 >> imm;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] | imm;
        }
        else if (opcode == "xori") {
            iss >> rd >> rs1 >> imm;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] ^ imm;
        }
        else if (opcode == "srli") {
            iss >> rd >> rs1 >> imm;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] >> imm;
        }
        else if (opcode == "slli") {
            iss >> rd >> rs1 >> imm;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] << imm;
        }
        else if (opcode == "srl") {
            iss >> rd >> rs1 >> rs2;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] >> registers[rs2];
        }
        else if (opcode == "sll") {
            iss >> rd >> rs1 >> rs2;
            registers[rd.substr(0, rd.length() - 1)] = registers[rs1.substr(0, rs1.length() - 1)] << registers[rs2];
        }
        else if (opcode == "sw") {
            iss >> rs1 >> imm >> rs2;
            int memory_address = registers[rs2.substr(1, rs2.length() - 2)] + imm;
            memory[memory_address] = registers[rs1.substr(0, rs1.length() - 1)];
        }
        else if (opcode == "lw") {
            iss >> rd >> imm >> rs2;
            int memory_address = registers[rs2.substr(1, rs2.length() - 2)] + imm;
            registers[rd.substr(0, rd.length() - 1)] = memory[memory_address];
        }
    }

    file.close();
}

int main() {
    executeInstructions();

    // Afișarea valorilor înregistrate în ordinea definită
    for (const auto& registerName : {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "t3", "t4", "t5", "t6", "s0", "s1", "s2", "s3", "a0"
    }) {
        std::cout << registerName << ": " << registers[registerName] << std::endl;
    }

    return 0;
}
