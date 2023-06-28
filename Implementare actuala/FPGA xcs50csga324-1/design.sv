module ClockDivider (
  input wire clk_in,
  output reg clk_out
);
  reg [31:0] count;

  initial begin
    clk_out = 0; // Initializare cu valoarea 0
    count = 0; // Initializare cu valoarea 0
  end

  always @(posedge clk_in) begin
    if (count == 50000000-1) begin 
      clk_out <= ~clk_out;
      count <= 0;
    end else begin
      count <= count + 1;
    end
  end
endmodule
module processor (
    input wire clk,
    input wire reset,
  output reg [15:0] result
);
  reg [6:0] opcode;
  reg [6:0] opcode1;
  reg [4:0] funct3;
  reg [6:0] funct7;
  reg [11:0] imm12;
  reg [11:0] imm20;
  reg [6:0] shamt;
  reg [4:0] rs1;
  reg [4:0] rs2;
  reg [4:0] rd;
  reg [63:0] pc;
  reg [31:0] instruction;
  reg [63:0] regfile[0:31];
  reg [8:0] memory[0:71];
  integer i;
  initial begin
    {memory[0], memory[1], memory[2], memory[3]} = 32'h00A00093;//addi ra, zero, 10  
{memory[4], memory[5], memory[6], memory[7]} = 32'h01408113;//addi sp, ra, 20 
{memory[8], memory[9], memory[10], memory[11]} = 32'h002081B3;//add gp, ra, sp 
{memory[12], memory[13], memory[14], memory[15]} = 32'h40218233;//sub tp, gp, sp 
    {memory[16], memory[17], memory[18], memory[19]} = 32'h000012B7;//lui t0, 1 
{memory[20], memory[21], memory[22], memory[23]} = 32'h03200313;// addi t1, zero, 50
{memory[24], memory[25], memory[26], memory[27]} = 32'h0062F3B3;//and t2, t0, t1 
{memory[28], memory[29], memory[30], memory[31]} = 32'h0053EE33;//or t3, t2, t0
{memory[32], memory[33], memory[34], memory[35]} = 32'h007E4EB3;//xor t4, t3, t2 
{memory[36], memory[37], memory[38], memory[39]} = 32'h003EFF13;//andi t5, t4, 3
{memory[40], memory[41], memory[42], memory[43]} = 32'h006F6F93;//ori t6, t5, 6 
{memory[44], memory[45], memory[46], memory[47]} = 32'h009FC413;//xori s0, t6, 9
{memory[48], memory[49], memory[50], memory[51]} = 32'h00245493;// srli s1, s0, 2 
{memory[52], memory[53], memory[54], memory[55]} = 32'h00949933;//sll s2, s1, s1
{memory[56], memory[57], memory[58], memory[59]} = 32'h009959B3;//srl s3, s2, s1
{memory[60], memory[61], memory[62], memory[63]} = 32'h00241493;//slli s1, s0, 2 
{memory[64], memory[65], memory[66], memory[67]} = 32'h00002503;//lw a0, 0(zero)
{memory[68], memory[69], memory[70], memory[71]} = 32'h000000e7; //jalr ra,zero,0(utilizata doar pentru resetare)    
    pc=0;
    for (i = 0; i < 32; i = i + 1) begin
        regfile[i] <= 64'h0;
      end
  end
  wire clk_div;
  ClockDivider clk_divider (
    .clk_in(clk),
    .clk_out(clk_div)
  );
  always @(posedge clk_div) begin
    if (reset) begin
      result <= 64'h0;
      pc <= 64'h0;
      for (i = 0; i < 32; i = i + 1) begin
        regfile[i] <= 64'h0;
      end
    end else begin
      instruction<={memory[pc],memory[pc+1],memory[pc+2],memory[pc+3]};       
      opcode <= instruction[6:0];
      funct3 <= instruction[14:12];
      funct7 <= instruction[31:25];
      imm12 <= instruction[31:20];
      imm20 <= instruction[31:12];
      shamt <= instruction[24:20];
      rs1 <= instruction[19:15];
      rs2 <= instruction[24:20];
      rd <= instruction[11:7];
      pc <= pc + 4; 
      case (opcode)
        7'b0110011: begin
          // R-type instructions
          case (funct3)
            3'b000: begin  // ADD, SUB
              if (funct7 == 7'b0000000)begin result <= regfile[rs1] + regfile[rs2];regfile[rd]<= regfile[rs1] + regfile[rs2]; end
              else if (funct7 == 7'b0100000) begin result <= regfile[rs1] - regfile[rs2];regfile[rd]<= regfile[rs1] - regfile[rs2]; end              
            end
            3'b001: begin  // SLL
              result <= regfile[rs1] << (regfile[rs2] & 5'b11111);
              regfile[rd] <= regfile[rs1] << (regfile[rs2] & 5'b11111);
            end
            3'b100: begin  // XOR
              result <= regfile[rs1] ^ regfile[rs2];
              regfile[rd] <= regfile[rs1] ^ regfile[rs2];
            end
            3'b101: begin
              if (funct7 == 7'b0000000) begin  // SRL
                result <= regfile[rs1] >> (regfile[rs2] & 5'b11111);
                regfile[rd] <= regfile[rs1] >> (regfile[rs2] & 5'b11111);
              end 
            end
            3'b110: begin  // OR
              result <= regfile[rs1] | regfile[rs2];
              regfile[rd] <= regfile[rs1] | regfile[rs2];
            end
            3'b111: begin  // AND
              result <= regfile[rs1] & regfile[rs2];
              regfile[rd] <= regfile[rs1] & regfile[rs2];
            end
            default: result <= 64'h0;
          endcase
        end
        7'b0010011: begin
          // I-type instructions
          case (funct3)
            3'b000: begin  // ADDI
              result <= regfile[rs1] + imm12;
              regfile[rd] <= regfile[rs1] + imm12;
            end
            3'b100: begin  // XORI
              result <= regfile[rs1] ^ imm12;
              regfile[rd] <= regfile[rs1] ^ imm12;
            end
            3'b110: begin  // ORI
              result <= regfile[rs1] | imm12;
              regfile[rd] <= regfile[rs1] | imm12;
            end
            3'b111: begin  // ANDI
              result <= regfile[rs1] & imm12;
              regfile[rd] <= regfile[rs1] & imm12;
            end
            3'b001: begin
              if (funct7 == 7'b0000000) begin  // SLLI
                result <= regfile[rs1] << shamt;
                regfile[rd] <= regfile[rs1] << shamt;
              end
            end
            3'b101: begin
              if (funct7 == 7'b0000000) begin  // SRLI
                result <= regfile[rs1] >> shamt;
                regfile[rd] <= regfile[rs1] >> shamt;
              end 
            end
            default: result <= 64'h0;
          endcase
        end
        7'b0000011: begin
          // Load instruction
          case (funct3)
            3'b010: begin  // LW
              result <= memory[regfile[rs1]+imm12];
              regfile[rd] <= memory[regfile[rs1]+imm12];
            end
            default: result <= 64'h0;
          endcase
        end
        7'b0110111: begin  // LUI
          result <= {imm20, 12'b0};
          regfile[rd] <= {imm20, 12'b0};
        end
        7'b1100111: begin  // JALR care face resetarea
          result <= 0;
          regfile[rd] <= 0;
          pc <= 0;
          for (i = 0; i < 32; i = i + 1) begin
        regfile[i] <= 64'h0;
      end
        end
        default: result <= 64'h0;
      endcase
    end
  end
endmodule
