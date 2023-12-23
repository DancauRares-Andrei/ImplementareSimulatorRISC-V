module testbench;
    reg clk;
    reg reset;
    reg [31:0] instruction;
  wire [63:0] result;

    processor dut (
        .clk(clk),
        .reset(reset),
        .instruction(instruction),
        .result(result)
    );

    always begin
        clk = 1;
        #5;
        clk = 0;
        #5;
    end

    initial begin
        $dumpfile("dump.vcd");
        $dumpvars;

        reset = 1;
        #10;
        reset = 0;//Momentul 10
        #10;

        // Test ADDI ra, zero, 10 Momentul 20
        instruction = 32'h00A00093;
        #20;

        // Test ADDI sp, ra, 20 Momentul 40
        instruction = 32'h01408113;
        #20;

        // Test ADD gp, ra, sp Momentul 60
        instruction = 32'h002081B3;
        #20;

        // Test SUB tp, gp, sp Momentul 80
        instruction = 32'h40218233;
        #20;

        // Test LUI t0, 100 Momentul 100
        instruction = 32'h000642B7;
        #20;

        // Test ADDI t1, zero, 50 Momentul 120
        instruction = 32'h03200313;
        #20;

        // Test AND t2, t0, t1 Momentul 140
        instruction = 32'h0062F3B3;
        #20;

        // Test OR t3, t2, t0 Momentul 160
        instruction = 32'h0053EE33;
        #20;

        // Test XOR t4, t3, t2 Momentul 180
        instruction = 32'h007E4EB3;
        #20;

        // Test ANDI t5, t4, 3 Momentul 200
        instruction = 32'h003EFF13;
        #20;

        // Test ORI t6, t5, 6 Momentul 220
        instruction = 32'h006F6F93;
        #20;

        // Test XORI s0, t6, 9 Momentul 240
        instruction = 32'h009FC413;
        #20;

        // Test SRLI s1, s0, 2 Momentul 260
        instruction = 32'h00245493;
        #20;

        // Test SLL s2, s1, s1 Momentul 280
        instruction = 32'h00949933;
        #20;

        // Test SRL s3, s2, s1 Momentul 300
        instruction = 32'h009959B3;
        #20;

        // Test SLLI s1, s0, 2 Momentul 320
        instruction = 32'h00241493;
        #20;

      // Test SW gp, 0(zero) Momentul 340
        instruction = 32'h00302023;
        #20;

      // Test LW a0, 0(zero) Momentul 360
        instruction = 32'h00002503;
        #20;
	//Momentul 380
        $finish;
    end
endmodule
