import re

# Definirea procesorului RISC-V
mem_size = 40

class RISC_V_Processor:
    def __init__(self):
        self.registers = [0] * 32  # Inițializarea băncii de registre cu 32 de registre
        self.memory = [0] * mem_size   # Inițializarea memoriei cu 256 de adrese
        self.reg_aliases = {
            'zero': 0, 'ra': 1, 'sp': 2, 'gp': 3, 'tp': 4, 't0': 5, 't1': 6, 't2': 7,
            's0': 8, 'fp': 8, 's1': 9, 'a0': 10, 'a1': 11, 'a2': 12, 'a3': 13,
            'a4': 14, 'a5': 15, 'a6': 16, 'a7': 17, 's2': 18, 's3': 19, 's4': 20,
            's5': 21, 's6': 22, 's7': 23, 's8': 24, 's9': 25, 's10': 26, 's11': 27,
            't3': 28, 't4': 29, 't5': 30, 't6': 31
        }

    def parse_register(self, reg):
        if reg in self.reg_aliases:
            return self.reg_aliases[reg]
        else:
            print(f"Alias de registru necunoscut: {reg}")
            return None

    def execute_instruction(self, instruction):
        print(f"Bancul de registre înainte de execuția instrucțunii {instruction}:")
        for reg_alias, reg_idx in self.reg_aliases.items():
            print(f"{reg_alias} : {self.registers[reg_idx]}", end="  ")
        print()
        print("Memoria înainte de execuția instrucțunii:", self.memory)

        parts = instruction.split()  # Descompunerea instrucțiunii în părți

        rd = self.parse_register(parts[1].rstrip(','))

        if parts[0] == 'ld':
            match = re.search(r'(.+)\((.+)\)', parts[2])
            offset = int(match.group(1))
            rs = self.parse_register(match.group(2))
            if rd is not None and rs is not None:
                address = self.registers[rs] + offset
                self.registers[rd] = self.memory[address]
        elif parts[0] == 'sd':
            match = re.search(r'(.+)\((.+)\)', parts[2])
            offset = int(match.group(1))
            rs = self.parse_register(parts[1][0:-1])
            rd = self.parse_register(match.group(2))
            if rs is not None and rd is not None:
                address = self.registers[rd] + offset
                self.memory[address] = self.registers[rs]
        else:
            rs = self.parse_register(parts[2].rstrip(','))
            rs2 = None
            imm = None

            if len(parts) > 3:
                operand = parts[3].rstrip(',')
                if operand.isdigit():
                    imm = int(operand)
                else:
                    rs2 = self.parse_register(operand)
            
            if parts[0] == 'add':
                if rd is not None and rs is not None and rs2 is not None:
                    self.registers[rd] = self.registers[rs] + self.registers[rs2]
            elif parts[0] == 'sub':
                if rd is not None and rs is not None and rs2 is not None:
                    self.registers[rd] = self.registers[rs] - self.registers[rs2]
            elif parts[0] == 'addi':
                if rd is not None and rs is not None and imm is not None:
                    self.registers[rd] = self.registers[rs] + imm
            elif parts[0] == 'lui':
                imm = int(parts[2])
                if rd is not None and imm is not None:
                    self.registers[rd] = imm << 12
            elif parts[0] == 'and':
                if rd is not None and rs is not None and rs2 is not None:
                    self.registers[rd] = self.registers[rs] & self.registers[rs2]
            elif parts[0] == 'or':
                if rd is not None and rs is not None and rs2 is not None:
                    self.registers[rd] = self.registers[rs] | self.registers[rs2]
            elif parts[0] == 'xor':
                if rd is not None and rs is not None and rs2 is not None:
                    self.registers[rd] = self.registers[rs] ^ self.registers[rs2]
            elif parts[0] == 'andi':
                if rd is not None and rs is not None and imm is not None:
                    self.registers[rd] = self.registers[rs] & imm
            elif parts[0] == 'ori':
                if rd is not None and rs is not None and imm is not None:
                    self.registers[rd] = self.registers[rs] | imm
            elif parts[0] == 'xori':
                if rd is not None and rs is not None and imm is not None:
                    self.registers[rd] = self.registers[rs] ^ imm
            elif parts[0] == 'sll':
                print(rs2)
                if rd is not None and rs is not None and rs2 is not None:
                    self.registers[rd] = self.registers[rs] <<  self.registers[rs2]
            elif parts[0] == 'srl':
                if rd is not None and rs is not None and rs2 is not None:
                    self.registers[rd] = self.registers[rs] >>  self.registers[rs2]
            elif parts[0] == 'slli':
                if rd is not None and rs is not None and imm is not None:
                    self.registers[rd] = self.registers[rs] << imm
            elif parts[0] == 'srli':
                if rd is not None and rs is not None and imm is not None:
                    self.registers[rd] = self.registers[rs] >> imm
        print("Bancul de registre după execuția instrucțunii:")
        for reg_alias, reg_idx in self.reg_aliases.items():
            print(f"{reg_alias} : {self.registers[reg_idx]}", end="  ")
        print()
        print("Memoria după execuția instrucțunii:", self.memory)

# Crearea unui obiect procesor RISC-V
processor = RISC_V_Processor()

# Exemple de instrucțiuni
instructions = [
    "addi ra, zero, 10",
    "addi sp, ra, 20",
    "add gp, ra, sp",
    "sub tp, gp, sp",
    "lui t0, 100",
    "addi t1, zero, 50",
    "and t2, t0, t1",
    "or t3, t2, t0",
    "xor t4, t3, t2",
    "andi t5, t4, 3",
    "ori t6, t5, 6",
    "xori s0, t6, 9",
    "srli s1, s0, 2",
    "sll s2, s1, s1",
    "srl s3, s2, s1",
    "slli s1, s0, 2",
    "ld a0, 0(zero)",
    "sd gp, 0(zero)"
]

# Executarea instrucțiunilor
for instr in instructions:
    processor.execute_instruction(instr)
    print("-" * 50)
    print()

