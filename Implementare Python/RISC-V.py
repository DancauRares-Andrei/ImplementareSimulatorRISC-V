import time
def sign_extend(number, bits):
    # Construiește un număr cu toți biții setați pe 1
    mask = (1 << bits) - 1
    
    # Obține cel mai semnificativ bit al numărului
    sign_bit = number & (1 << (bits - 1))
    
    # Extinde semnul la 64 de biți
    if sign_bit:
        return number | (~mask & 0xFFFFFFFFFFFFFFFF)
    else:
        return number
def limit64bits(number):
    return number&0xFFFFFFFFFFFFFFFF
def limit16bits(number):
    return number&0xFFFF
class RISCVProcessor:
    def __init__(self):
        self.pc = 0
        self.regdif = 0
        self.Jump = False
        self.jumpAddress = 0
        self.registers = [0] * 32
        self.dataMemory = [0] * 100
        self.memory = [0] * 176
        self.instruction = 0
        self.opcode = 0
        self.funct3 = 0
        self.funct7 = 0
        self.imm12 = 0
        self.shamt = 0
        self.rs1 = 0
        self.rs2 = 0
        self.rd = 0
        self.result = 0
    def initializare_instructiuni(self,file:str):
        with open(file, "r") as f:
            hex_instructions = f.read().split()
            self.memory= hex_instructions
    def reset(self):
        self.pc = 0
        self.regdif = 0
        self.Jump = False
        self.jumpAddress = 0
        self.registers = [0] * 32
        self.dataMemory = [0] * 100
        self.memory = [0] * 176
        self.instruction = 0
        self.opcode = 0
        self.funct3 = 0
        self.funct7 = 0
        self.imm12 = 0
        self.shamt = 0
        self.rs1 = 0
        self.rs2 = 0
        self.rd = 0
        self.result = 0
    def execute(self):
        instruction = (int(self.memory[self.pc],16)<<24)+(int(self.memory[self.pc+1],16)<<16)+(int(self.memory[self.pc+2],16)<<8)+(int(self.memory[self.pc+3],16))
        self.opcode = instruction & 0b1111111
        self.funct3 = (instruction >> 12) & 0b111
        self.funct7 = (instruction >> 25) & 0b1111111
        self.imm12 = (instruction >> 20) & 0b111111111111
        self.shamt = (instruction >> 20) & 0b11111
        self.rs1 = (instruction >> 15) & 0b11111
        self.rs2 = (instruction >> 20) & 0b11111
        self.rd = (instruction >> 7) & 0b11111

        if self.opcode == 0b0110011:  # R-type instructions

            if self.funct3 == 0b000:  # ADD, SUB
                if self.funct7 == 0b0000000:
                    self.registers[self.rd] = limit64bits(self.registers[self.rs1] + self.registers[self.rs2])
                    self.result = hex(limit16bits(self.registers[self.rd]))
                    #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
                elif self.funct7 == 0b0100000:
                    self.registers[self.rd] = limit64bits(self.registers[self.rs1] - self.registers[self.rs2])
                    self.result = hex(limit16bits(self.registers[self.rd]))
                    #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b001:  # SLL
                self.registers[self.rd] = limit64bits(self.registers[self.rs1] << (self.registers[self.rs2]&0b11111))
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b010:  # SLT
                self.regdif = limit64bits(self.registers[self.rs1] - self.registers[self.rs2])
                self.registers[self.rd] = int(self.regdif >> 63)
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b011:  # SLTU
                self.registers[self.rd] = 1 if self.registers[self.rs1] < self.registers[self.rs2] else 0
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b100:  # XOR
                self.registers[self.rd] = limit64bits(self.registers[self.rs1] ^ self.registers[self.rs2])
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b101:
                if self.funct7 == 0b0000000:  # SRL
                    self.registers[self.rd] = limit64bits(self.registers[self.rs1] >> (self.registers[self.rs2]&0b11111))
                    self.result = hex(limit16bits(self.registers[self.rd]))
                    #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
                elif self.funct7 == 0b0100000:  # SRA
                    self.registers[self.rd] = limit64bits(self.registers[self.rs1] >> (self.registers[self.rs2]&0b11111))
                    self.result = hex(limit16bits(self.registers[self.rd]))
                    #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b110:  # OR
                self.registers[self.rd] = limit64bits(self.registers[self.rs1] | self.registers[self.rs2])
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b111:  # AND
                self.registers[self.rd] = limit64bits(self.registers[self.rs1] & self.registers[self.rs2])
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
        elif self.opcode == 0b0010011:  # I-type instructions
            if self.funct3 == 0b000:  # ADDI
                self.registers[self.rd] = limit64bits(self.registers[self.rs1] +sign_extend(self.imm12,12))
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b001:  # SLLI
                self.registers[self.rd] = limit64bits(self.registers[self.rs1] << self.shamt)
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b010:  # SLTI
                self.registers[self.rd] = limit64bits(self.registers[self.rs1] - sign_extend(self.imm12,12))>>63
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b011:  # SLTIU 
                self.registers[self.rd] = self.registers[self.rs1] < sign_extend(self.imm12,12)
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b100:  # XORI
                self.registers[self.rd] = limit64bits(self.registers[self.rs1] ^ sign_extend(self.imm12,12))
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b110:  # ORI
                self.registers[self.rd] = limit64bits(self.registers[self.rs1] | sign_extend(self.imm12,12))
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b111:  # ANDI
                self.registers[self.rd] = limit64bits(self.registers[self.rs1] & sign_extend(self.imm12,12))
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b101:  
                if self.funct7 == 0b0000000: #SRLI
                    self.registers[self.rd] = limit64bits(self.registers[self.rs1] >> self.shamt)
                    self.result = hex(limit16bits(self.registers[self.rd]))
                    #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
                elif self.funct7 == 0b0100000: #SRAI
                    self.registers[self.rd] = limit64bits(self.registers[self.rs1] >> self.shamt)
                    self.result = hex(limit16bits(self.registers[self.rd]))
                    #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            else:
             print('Operatie nesuportata:'+str(hex(funct3)))

        elif self.opcode == 0b0000011:  # Load instructions

            if self.funct3 == 0b000:  # LB
                self.registers[self.rd]=sign_extend(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]&0xff,8)
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b001: #LH  
                self.registers[self.rd]=sign_extend(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]&0xFFFF,16)
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b010: #LW
                self.registers[self.rd]=sign_extend(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]&0xFFFFFFFF,32)
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b011: #LD
                self.registers[self.rd]=sign_extend(limit64bits(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]),64)
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b100: #LBU
                self.registers[self.rd]=self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]&0xFF
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b101: #LHU
                self.registers[self.rd]=self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]&0xFFFF
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
            elif self.funct3 == 0b110: #LWU
                self.registers[self.rd]=self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]&0xFFFFFFFF
                self.result = hex(limit16bits(self.registers[self.rd]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
        elif self.opcode == 0b0100011:  # Store instructions
            if self.funct3== 0b000:  # SB 
                self.imm12=((instruction>>25)<<7)|((instruction>>7)&0b11111)
                self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]=self.registers[self.rs2]&0xff
                self.result = hex(limit16bits(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]))}")
            elif self.funct3 == 0b001: #SH
                self.imm12=((instruction>>25)<<7)|((instruction>>7)&0b11111)
                self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]=self.registers[self.rs2]&0xFFFF
                self.result = hex(limit16bits(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]))}")
            elif self.funct3 == 0b010: #SW
                self.imm12=((instruction>>25)<<7)|((instruction>>7)&0b11111)
                self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]=self.registers[self.rs2]&0xFFFFFFFF
                self.result = hex(limit16bits(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]))}")
            elif self.funct3 == 0b011: #SD
                self.imm12=((instruction>>25)<<7)|((instruction>>7)&0b11111)
                self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]=self.registers[self.rs2]
                self.result = hex(limit16bits(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.dataMemory[limit64bits(self.registers[self.rs1]+sign_extend(self.imm12,12))]))}")
        elif self.opcode == 0b0110111:  # LUI
            self.imm12=instruction>>12
            self.registers[self.rd] = limit64bits(sign_extend(self.imm12,20) << 12)
            self.result = hex(limit16bits(self.registers[self.rd]))
            #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
        elif self.opcode == 0b0010111:  # AUIPC
            self.imm12=instruction>>12
            self.registers[self.rd] = limit64bits(self.pc+(sign_extend(self.imm12,20)<< 12))
            self.result = hex(limit16bits(self.registers[self.rd]))
            #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.registers[self.rd]))}")
        elif self.opcode == 0b1101111:  # JAL
            self.registers[self.rd] = limit64bits(self.pc+4)
            self.Jump=True
            self.imm12=(((instruction>>31)<<19)|(((instruction>>12)&0xFF)<<11)|(instruction>>21)&0x3FF)<<1
            self.jumpAddress=limit64bits(self.pc+sign_extend(self.imm12,20))
            self.result = hex(limit16bits(self.jumpAddress))
            #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.jumpAddress))}")
        elif self.opcode == 0b1100111:  # JALR 
            self.registers[self.rd] = self.pc + 4
            self.Jump = True
            self.jumpAddress = limit64bits(self.registers[self.rs1] + sign_extend(self.imm12//2*2,12))
            self.result = hex(limit16bits(self.jumpAddress))
            #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.jumpAddress))}")
        elif self.opcode == 0b1100011:
            self.imm12=((instruction>>31)<<12)|(((instruction>>7)&1)<<11)|(((instruction>>25)&0b111111)<<5)|((instruction>>8)&0xf)<<1
            if self.funct3 == 0b000: #BEQ
                self.Jump=(self.registers[self.rs1]==self.registers[self.rs2])
                self.jumpAddress=limit64bits(self.pc+sign_extend(self.imm12,12))
                self.result = hex(limit16bits(self.jumpAddress if self.Jump else 0))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.jumpAddress if self.Jump else 0))}")
            elif self.funct3 == 0b001: #BNE
                self.Jump=(self.registers[self.rs1]!=self.registers[self.rs2])
                self.jumpAddress=limit64bits(self.pc+sign_extend(self.imm12,12))
                self.result = hex(limit16bits(self.jumpAddress if self.Jump else 0))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.jumpAddress if self.Jump else 0))}")
            elif self.funct3 == 0b100: #BLT
                self.Jump=(self.registers[self.rs1]-self.registers[self.rs2])>>63
                self.jumpAddress=limit64bits(self.pc+sign_extend(self.imm12,12))
                self.result = hex(limit16bits(self.jumpAddress if self.Jump else 0))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.jumpAddress if self.Jump else 0))}")
            elif self.funct3 == 0b101: #BGE
                self.Jump=1-((self.registers[self.rs1]-self.registers[self.rs2])>>63)
                self.jumpAddress=limit64bits(self.pc+sign_extend(self.imm12,12))
                self.result = hex(limit16bits(self.jumpAddress if self.Jump else 0))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.jumpAddress if self.Jump else 0))}")
            elif self.funct3 == 0b110: #BLTU
                self.Jump=(self.registers[self.rs1]<self.registers[self.rs2])
                self.jumpAddress=limit64bits(self.pc+sign_extend(self.imm12,12))
                self.result = hex(limit16bits(self.jumpAddress if self.Jump else 0))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.jumpAddress if self.Jump else 0))}")
            elif self.funct3 == 0b111: #BGEU
                self.Jump=(self.registers[self.rs1]>=self.registers[self.rs2])
                self.jumpAddress=limit64bits(self.pc+sign_extend(self.imm12,12))
                self.result = hex(limit16bits(self.jumpAddress if self.Jump else 0))
                #print(f"PC: {hex(self.pc)}, result: {hex(limit16bits(self.jumpAddress if self.Jump else 0))}")
        if self.Jump:
            self.pc = self.jumpAddress
            self.Jump = False
        else:
            self.pc += 4
        return self.result
    def run(self,expected_result_list):
        result_list=[]
        while self.pc<len(self.memory):
            result_list.append(self.execute())
            #time.sleep(0.1)
        if expected_result_list==result_list:
            print("Lista produsa in cadrul executiei corespunde cu cea asteptata. O afisez.")
            print(result_list)
        else:
            print("Ceva e gresit. Afisez ambele liste pentru verificare.")
            print(expected_result_list)
            print(result_list)
        
if __name__ == "__main__":
    processor = RISCVProcessor()
    processor.initializare_instructiuni("instructions_add_load.mem")
    processor.run(['0x5', '0x7', '0xc', '0xc', '0x0', '0xc'])
    processor.reset()
    processor.initializare_instructiuni("instructions_counter.mem")
    processor.run(['0xa', '0x1', '0x9', '0x8', '0x8', '0x8', '0x7', '0x8', '0x6', '0x8', '0x5', '0x8', '0x4', '0x8', '0x3', '0x8', '0x2', '0x8', '0x1', '0x8', '0x0', '0x0'])
    processor.reset()
    processor.initializare_instructiuni("instructions_mare.mem")
    processor.run(['0xa', '0x1e', '0x28', '0xa', '0x1000', '0x32', '0x0', '0x1000', '0x1000', '0x0', '0x6', '0xf', '0x3', '0x18', '0x3', '0x3c', '0x28', '0x28', '0x0', '0x1', '0x7', '0x0', '0x1', '0xa', '0x3', '0xf', '0x1000', '0x3', '0xf', '0x3', '0xf', '0x28', '0x1000', '0x8c', '0x408c', '0x0', '0x98', '0x0', '0xa0', '0x0', '0xa8'])
    processor.reset()
    processor.initializare_instructiuni("instructions_paritate.mem")
    processor.run(['0x9', '0x1', '0x0', '0x3e7', '0x18'])
    processor.reset()
    processor.initializare_instructiuni("instructions_sll_xor.mem")
    processor.run(['0xa', '0x2', '0x28', '0x8', '0x8', '0x2', '0x4', '0x5', '0x4', '0x2', '0x1', '0x34', '0x3000', '0x0', '0x1', '0xfffe', '0x0', '0x1', '0x6', '0x3', '0x1', '0x0', '0x0', '0x0', '0x0', '0x0', '0x0', '0x0', '0x0', '0x0'])
    processor.reset()
    processor.initializare_instructiuni("instructions_stocari_extrageri.mem")
    processor.run(['0xffff', '0x1', '0xff', '0xfffe', '0xfffe', '0xfffd', '0xfffd', '0x24', '0x2c', '0xffff', '0xfffd', '0xfffe', '0xfd', '0xff', '0xfffe'])
    processor.reset()
    processor.initializare_instructiuni("instructions_subrutina.mem")
    processor.run(['0x0', '0x10', '0x4', '0x8', '0x5', '0x3', '0x7', '0xa', '0x8', '0x0', '0x8', '0x0', '0x34', '0x0'])
    processor.reset()
    processor.initializare_instructiuni("instructions_suma.mem")
    processor.run(['0x5', '0x1', '0x1', '0x2', '0x8', '0x3', '0x3', '0x8', '0x6', '0x4', '0x8', '0xa', '0x5', '0x0', '0x0'])
