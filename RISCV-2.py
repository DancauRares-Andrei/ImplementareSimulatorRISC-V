#Alta implementare ce suporta addi, add, sub, lui, and, or, xor, andi, ori, xori, srli, sll, srl, slli, sw si lw
# Definirea registrelor
registers = {
    'zero': 0,
    'ra': 0,
    'sp': 0,
    'gp': 0,
    'tp': 0,
    't0': 0,
    't1': 0,
    't2': 0,
    't3': 0,
    't4': 0,
    't5': 0,
    't6': 0,
    's0': 0,
    's1': 0,
    's2': 0,
    's3': 0,
    'a0': 0
}

# Definirea memoriei
memory = [0] * 1000

# Citirea instrucțiunilor din fișier
with open('instructions.txt', 'r') as file:
    instructions = file.readlines()

# Executarea instrucțiunilor
for instruction in instructions:
    parts = instruction.strip().split(' ')
    opcode = parts[0]

    if opcode == 'add':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        rs2 = parts[3]
        registers[rd] = registers[rs1] + registers[rs2]
    elif opcode == 'addi':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        imm = int(parts[3])
        registers[rd] = registers[rs1] + imm
    elif opcode == 'sub':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        rs2 = parts[3]
        registers[rd] = registers[rs1] - registers[rs2]
    elif opcode == 'lui':
        rd = parts[1][:-1]
        imm = int(parts[2]) << 12  # Shiftăm imediatul cu 12 poziții în stânga
        registers[rd] = imm
    elif opcode == 'and':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        rs2 = parts[3]
        registers[rd] = registers[rs1] & registers[rs2]
    elif opcode == 'or':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        rs2 = parts[3]
        registers[rd] = registers[rs1] | registers[rs2]
    elif opcode == 'xor':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        rs2 = parts[3]
        registers[rd] = registers[rs1] ^ registers[rs2]
    elif opcode == 'andi':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        imm = int(parts[3])
        registers[rd] = registers[rs1] & imm
    elif opcode == 'ori':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        imm = int(parts[3])
        registers[rd] = registers[rs1] | imm
    elif opcode == 'xori':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        imm = int(parts[3])
        registers[rd] = registers[rs1] ^ imm
    elif opcode == 'srli':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        imm = int(parts[3])
        registers[rd] = registers[rs1] >> imm
    elif opcode == 'slli':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        imm = int(parts[3])
        registers[rd] = registers[rs1] << imm
    elif opcode == 'srl':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        rs2 = parts[3]
        registers[rd] = registers[rs1] >> registers[rs2]
    elif opcode == 'sll':
        rd = parts[1][:-1]
        rs1 = parts[2][:-1]
        rs2 = parts[3]
        registers[rd] = registers[rs1] << registers[rs2]
    elif opcode == 'sw':
        rs1 = parts[1][:-1]
        imm = int(parts[2].split('(')[0])
        rs2 = parts[2].split('(')[1][:-1]
        memory_address = registers[rs2] + imm
        memory[memory_address] = registers[rs1]
    elif opcode == 'lw':
        rd = parts[1][:-1]
        imm = int(parts[2].split('(')[0])
        rs2 = parts[2].split('(')[1][:-1]
        memory_address = registers[rs2] + imm
        registers[rd] = memory[memory_address]

# Afișarea valorilor înregistrate la sfârșitul execuției
for register, value in registers.items():
    print(f'{register}: {value}')
