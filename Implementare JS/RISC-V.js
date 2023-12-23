class RISCVProcessor {
    constructor() {
        this.pc = 0;
        this.regdif = 0;
        this.Jump = false;
        this.jumpAddress = 0;
        this.registers = Array(32).fill(0);
        this.dataMemory = Array(100).fill(0);
        this.memory = Array(176).fill(0);
        this.instruction = 0;
        this.opcode = 0;
        this.funct3 = 0;
        this.funct7 = 0;
        this.imm12 = 0;
        this.shamt = 0;
        this.rs1 = 0;
        this.rs2 = 0;
        this.rd = 0;
        this.result = 0;
    }

    signExtend(number, bits) {
        const mask = (1 << bits) - 1;
        const signBit = number & (1 << (bits - 1));
        if (signBit) {
            return (number | (~mask & 0xFFFFFFFF)>>>0);
        } else {
            return number;
        }
    }

    limit32bits(number) { //Javascript nu suporta operatii pe biti pentru numere pe 64 de biti!
        return (number>>>0)& 0xFFFFFFFF;
    }

    limit16bits(number) {
        return (number>>>0) & 0xFFFF;
    }

    initializeInstructions(file) {
        const fs = require('fs');
        const fileContent = fs.readFileSync(file, 'utf-8');
        const normalizedContent = fileContent.replace(/\r\n/g, '\n');
        const hexInstructions = normalizedContent.split('\n');
        this.memory = hexInstructions.filter(n => n);
    }


    reset() {
        this.pc = 0;
        this.regdif = 0;
        this.Jump = false;
        this.jumpAddress = 0;
        this.registers = Array(32).fill(0);
        this.dataMemory = Array(100).fill(0);
        this.memory = Array(176).fill(0);
        this.instruction = 0;
        this.opcode = 0;
        this.funct3 = 0;
        this.funct7 = 0;
        this.imm12 = 0;
        this.shamt = 0;
        this.rs1 = 0;
        this.rs2 = 0;
        this.rd = 0;
        this.result = 0;
    }

    execute() {
        const instruction =
            (parseInt(this.memory[this.pc], 16) << 24) +
            (parseInt(this.memory[this.pc + 1], 16) << 16) +
            (parseInt(this.memory[this.pc + 2], 16) << 8) +
            parseInt(this.memory[this.pc + 3], 16);
        this.opcode = instruction & 0b1111111;
        this.funct3 = (instruction >> 12) & 0b111;
        this.funct7 = (instruction >> 25) & 0b1111111;
        this.imm12 = (instruction >> 20) & 0b111111111111;
        this.shamt = (instruction >> 20) & 0b11111;
        this.rs1 = (instruction >> 15) & 0b11111;
        this.rs2 = (instruction >> 20) & 0b11111;
        this.rd = (instruction >> 7) & 0b11111;
        if (this.opcode == 0b0110011) { //Instructiuni R
            if (this.funct3 == 0b000) {
                if (this.funct7 == 0b0000000) { //ADD
                    this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] + this.registers[this.rs2]);
                    this.result = this.limit16bits(this.registers[this.rd]).toString(16);
                } else if (this.funct7 == 0b0100000) { //SUB
                    this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] - this.registers[this.rs2]);
                    this.result = this.limit16bits(this.registers[this.rd]).toString(16);
                }
            } else if (this.funct3 == 0b001) { //SLL
                this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] << (this.registers[this.rs2] & 0b11111));
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b010) { //SLT
                this.regdif = this.limit32bits(this.registers[this.rs1] - this.registers[this.rs2]);
                this.registers[this.rd] = parseInt(this.regdif >> 31)
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b011) { //SLTU
                this.registers[this.rd] = ((this.registers[this.rs1]>>>0) < (this.registers[this.rs2]>>>0)) ? 1 : 0;
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b100) { //XOR
                this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] ^ this.registers[this.rs2]);
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b101) {
                if (this.funct7 == 0b0000000) { //SRL
                    this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] >> (this.registers[this.rs2] & 0b11111))
                    this.result = this.limit16bits(this.registers[this.rd]).toString(16);
                } else if (this.funct7 == 0b0100000) { //SRA
                    this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] >> (this.registers[this.rs2] & 0b11111))
                    this.result = this.limit16bits(this.registers[this.rd]).toString(16);
                }
            } else if (this.funct3 == 0b110) { //OR
                this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] | this.registers[this.rs2]);
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b111) { //AND
                this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] & this.registers[this.rs2]);
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            }
        } else if (this.opcode == 0b0010011) { //Instructiuni I
            if (this.funct3 == 0b000) { //ADDI
                this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12));
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b001) { //SLLI
                this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] << this.shamt);
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b010) { //SLTI
                this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] - this.signExtend(this.imm12, 12)) >> 63;
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b011) { //SLTIU
                this.registers[this.rd] = (this.registers[this.rs1]>>>0) < ((this.signExtend(this.imm12, 12)>>>0))
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b100) { //XORI
                this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] ^ this.signExtend(this.imm12, 12));
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b101) {
                if (this.funct7 == 0b0000000) { //SRLI
                    this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] >> this.shamt)
                    this.result = this.limit16bits(this.registers[this.rd]).toString(16);
                } else if (this.funct7 == 0b0100000) { //SRAI
                    this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] >> this.shamt)
                    this.result = this.limit16bits(this.registers[this.rd]).toString(16);
                }
            } else if (this.funct3 == 0b110) { //ORI
                this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] | this.signExtend(this.imm12, 12));
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            } else if (this.funct3 == 0b111) { //ANDI
                this.registers[this.rd] = this.limit32bits(this.registers[this.rs1] & this.signExtend(this.imm12, 12));
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            }
        } else if (this.opcode == 0b0000011) {
           if (this.funct3 == 0b000) { //LB
                this.registers[this.rd] = this.signExtend(this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))] & 0xFF, 8);
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            }
            else if (this.funct3 == 0b001) { //LH
                this.registers[this.rd] = this.signExtend(this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))] & 0xFFFF, 16);
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            }
           else if (this.funct3 == 0b010) { //LW
                this.registers[this.rd] = this.signExtend(this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))] & 0xFFFFFFFF, 32);
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            }
            else if (this.funct3 == 0b011) { //LD neoficial(echivalent LW)
                this.registers[this.rd] = this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))];               
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            }
            else if (this.funct3 == 0b100) { //LBU
                this.registers[this.rd] = this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))] & 0xFF;
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            }
            else if (this.funct3 == 0b101) { //LHU
                this.registers[this.rd] = this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))] & 0xFFFF;
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            }
            else if (this.funct3 == 0b110) { //LWU
                this.registers[this.rd] = this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))] & 0xFFFFFFFF;
                this.result = this.limit16bits(this.registers[this.rd]).toString(16);
            }
        } else if (this.opcode == 0b0100011) {
        if (this.funct3 == 0b000) { //SB
                this.imm12 = ((instruction >> 25) << 7) | ((instruction >> 7) & 0b11111);
                this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))] = this.registers[this.rs2] & 0xFF;
                this.result = this.limit16bits(this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))]).toString(16);
            }
            else if (this.funct3 == 0b001) { //SH
                this.imm12 = ((instruction >> 25) << 7) | ((instruction >> 7) & 0b11111);
                this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))] = this.registers[this.rs2] & 0xFFFF;
                this.result = this.limit16bits(this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))]).toString(16);
            }
           else if (this.funct3 == 0b010) { //SW
                this.imm12 = ((instruction >> 25) << 7) | ((instruction >> 7) & 0b11111);
                this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))] = this.registers[this.rs2] & 0xFFFFFFFF;
                this.result = this.limit16bits(this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))]).toString(16);
            }
            else if (this.funct3 == 0b011) { //SD neoficial
                this.imm12 = ((instruction >> 25) << 7) | ((instruction >> 7) & 0b11111);
                this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))] = this.registers[this.rs2] & 0xFFFFFFFF;
                this.result = this.limit16bits(this.dataMemory[this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12, 12))]).toString(16);
            }
        }
        else if (this.opcode == 0b0110111){//LUI 
            this.imm12=instruction>>12
            this.registers[this.rd] = this.limit32bits(this.signExtend(this.imm12,20) << 12)
            this.result = this.limit16bits(this.registers[this.rd]).toString(16);
        }
        else if (this.opcode == 0b0010111){//AUIPC
            this.imm12=instruction>>12
            this.registers[this.rd] = this.limit32bits(this.pc+(this.signExtend(this.imm12,20) << 12))
            this.result = this.limit16bits(this.registers[this.rd]).toString(16);
        }
        else if (this.opcode == 0b1101111){//JAL
            this.registers[this.rd] = this.limit32bits(this.pc+4)
            this.Jump=true
            this.imm12=(((instruction>>31)<<19)|(((instruction>>12)&0xFF)<<11)|(instruction>>21)&0x3FF)<<1
            this.jumpAddress=this.limit32bits(this.pc+this.signExtend(this.imm12,20))
            this.result = this.limit16bits(this.jumpAddress).toString(16);
        }
        else if (this.opcode == 0b1100111){//JALR
            this.registers[this.rd] = this.pc + 4
            this.Jump = true
            this.jumpAddress = this.limit32bits(this.registers[this.rs1] + this.signExtend(this.imm12/2*2,12))
            this.result = this.jumpAddress.toString(16);
        }
        else if(this.opcode==0b1100011){
            this.imm12=((instruction>>31)<<12)|(((instruction>>7)&1)<<11)|(((instruction>>25)&0b111111)<<5)|((instruction>>8)&0xf)<<1
            if(this.funct3 == 0b000){//BEQ
                this.Jump=(this.registers[this.rs1]==this.registers[this.rs2])
                this.jumpAddress=this.limit32bits(this.pc+this.signExtend(this.imm12,12))
                this.result=this.Jump?this.limit16bits(this.jumpAddress).toString(16):'0';
            }
            if(this.funct3 == 0b001){//BNE
                this.Jump=(this.registers[this.rs1]!=this.registers[this.rs2])
                this.jumpAddress=this.limit32bits(this.pc+this.signExtend(this.imm12,12))
                this.result=this.Jump?this.limit16bits(this.jumpAddress).toString(16):'0';
            }
            if(this.funct3 == 0b100){//BLT
                this.Jump=(this.registers[this.rs1]-this.registers[this.rs2])>>31
                this.jumpAddress=this.limit32bits(this.pc+this.signExtend(this.imm12,12))
                this.result=this.Jump?this.limit16bits(this.jumpAddress).toString(16):'0';
            }
            if(this.funct3 == 0b101){//BGE probleme
                this.Jump=1-((this.registers[this.rs1]-this.registers[this.rs2])>>31)
                this.jumpAddress=this.limit32bits(this.pc+this.signExtend(this.imm12,12))
                this.result=this.Jump?this.limit16bits(this.jumpAddress).toString(16):'0';
            }
            if(this.funct3 == 0b110){//BLTU
                this.Jump=((this.registers[this.rs1]>>>0)<(this.registers[this.rs2]>>>0))
                this.jumpAddress=this.limit32bits(this.pc+this.signExtend(this.imm12,12))
                this.result=this.Jump?this.limit16bits(this.jumpAddress).toString(16):'0';
            }
            if(this.funct3 == 0b111){//BGEU
                this.Jump=((this.registers[this.rs1]>>>0)>=(this.registers[this.rs2]>>>0))
                this.jumpAddress=this.limit32bits(this.pc+this.signExtend(this.imm12,12))
                this.result=this.Jump?this.limit16bits(this.jumpAddress).toString(16):'0';
            }
        }
        else {
            this.result = '0';
        }
        if (this.Jump) {
            this.pc = this.jumpAddress;
            this.Jump = false;
        } else {
            this.pc += 4;
        }
        return "0x" + this.result;
    }

    run(expectedResultList) {
        const resultList = [];
        while (this.pc < this.memory.length) {
            resultList.push(this.execute());
        }

        if (JSON.stringify(expectedResultList) === JSON.stringify(resultList)) {
            console.log('The produced list matches the expected one. Displaying it:');
            console.log(resultList);
        } else {
            console.log('Something is wrong. Displaying both lists for verification:');
            console.log(expectedResultList);
            console.log(resultList);
        }
    }
}

if (typeof module !== 'undefined' && typeof module.exports !== 'undefined') {
    // Export the class for Node.js
    module.exports = RISCVProcessor;
}
// Example usage:
const processor = new RISCVProcessor();
processor.initializeInstructions('instructions_add_load.mem');
processor.run(['0x5', '0x7', '0xc', '0xc', '0x0', '0xc']);
processor.reset();
processor.initializeInstructions('instructions_counter.mem');
processor.run(['0xa', '0x1', '0x9', '0x8', '0x8', '0x8', '0x7', '0x8', '0x6', '0x8', '0x5', '0x8', '0x4', '0x8', '0x3', '0x8', '0x2', '0x8', '0x1', '0x8', '0x0', '0x0']);
processor.reset();
processor.initializeInstructions("instructions_mare.mem")
processor.run(['0xa', '0x1e', '0x28', '0xa', '0x1000', '0x32', '0x0', '0x1000', '0x1000', '0x0', '0x6', '0xf', '0x3', '0x18', '0x3', '0x3c', '0x28', '0x28', '0x0', '0x1', '0x7', '0x0', '0x1', '0xa', '0x3', '0xf', '0x1000', '0x3', '0xf', '0x3', '0xf', '0x28', '0x1000', '0x8c', '0x408c', '0x0', '0x98', '0x0', '0xa0', '0x0', '0xa8'])
    processor.reset()
    processor.initializeInstructions("instructions_paritate.mem")
    processor.run(['0x9', '0x1', '0x0', '0x3e7', '0x18'])
    processor.reset()
    processor.initializeInstructions("instructions_sll_xor.mem")
    processor.run(['0xa', '0x2', '0x28', '0x8', '0x8', '0x2', '0x4', '0x5', '0x4', '0x2', '0x1', '0x34', '0x3000', '0x0', '0x1', '0xfffe', '0x0', '0x1', '0x6', '0x3', '0x1', '0x0', '0x0', '0x0', '0x0', '0x0', '0x0', '0x0', '0x0', '0x0'])
    processor.reset()
    processor.initializeInstructions("instructions_stocari_extrageri.mem")
    processor.run(['0xffff', '0x1', '0xff', '0xfffe', '0xfffe', '0xfffd', '0xfffd', '0x24', '0x2c', '0xffff', '0xfffd', '0xfffe', '0xfd', '0xff', '0xfffe'])
    processor.reset()
    processor.initializeInstructions("instructions_subrutina.mem")
    processor.run(['0x0', '0x10', '0x4', '0x8', '0x5', '0x3', '0x7', '0xa', '0x8', '0x0', '0x8', '0x0', '0x34', '0x0'])
    processor.reset()
    processor.initializeInstructions("instructions_suma.mem")
    processor.run(['0x5', '0x1', '0x1', '0x2', '0x8', '0x3', '0x3', '0x8', '0x6', '0x4', '0x8', '0xa', '0x5', '0x0', '0x0'])
