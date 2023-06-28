module testbench;
  reg clk;
  reg reset;
  wire [15:0] result;

  processor dut (
      .clk(clk),
      .reset(reset),
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
    reset=0;
    #840 $finish;
  end
endmodule
