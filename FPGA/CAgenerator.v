module CAgenerator (
    input wire clk, rst, caclki, caclkq,
    input wire [4:0] sat,
    input wire dither, dataclkrst,
    output CAcode, ditherCAcode,
	  output epoch, dataclk
);
    wire codeout;
    wire epoch;
    reg [9:0] satxor [0:31];
    initial begin
      satxor[0] <= 10'h022;
      satxor[1] <= 10'h044;
      satxor[2] <= 10'h088;
      satxor[3] <= 10'h110;
      satxor[4] <= 10'h101;
      satxor[5] <= 10'h202;
      satxor[6] <= 10'h081;
      satxor[7] <= 10'h102;
      satxor[8] <= 10'h204;
      satxor[9] <= 10'h006;
      satxor[10] <= 10'h00c;
      satxor[11] <= 10'h030;
      satxor[12] <= 10'h060;
      satxor[13] <= 10'h0c0;
      satxor[14] <= 10'h180;
      satxor[15] <= 10'h300;
      satxor[16] <= 10'h009;
      satxor[17] <= 10'h012;
      satxor[18] <= 10'h024;
      satxor[19] <= 10'h048;
      satxor[20] <= 10'h090;
      satxor[21] <= 10'h120;
      satxor[22] <= 10'h005;
      satxor[23] <= 10'h028;
      satxor[24] <= 10'h050;
      satxor[25] <= 10'h0a0;
      satxor[26] <= 10'h140;
      satxor[27] <= 10'h280;
      satxor[28] <= 10'h021;
      satxor[29] <= 10'h042;
      satxor[30] <= 10'h084;
      satxor[31] <= 10'h108;
    end
    wire [9:0] pseudo1, pseudo2;
    wire pseudo_in1, pseudo_in2;
    wire [1:0] dlyline_code;
    assign pseudo_in1 = ^(pseudo1 & 10'h204);
    assign pseudo_in2 = ^(pseudo2 & 10'h3a6);
    shiftreg #(10) the_shiftreg1(.clk(clk), .rst(rst), .shift(caclki), .d(10'h3ff), .serial_in(pseudo_in1), .q(pseudo1));
    shiftreg #(10) the_shiftreg2(.clk(clk), .rst(rst), .shift(caclki), .d(10'h3ff), .serial_in(pseudo_in2), .q(pseudo2));
    assign CAcode = (^(pseudo1 & 10'h200))^(^(pseudo2 & satxor[sat]));  //satellate 7 1^8
    assign epoch = & pseudo1;
	  shiftreg #(2)  code_reg(.clk(clk), .rst(rst), .shift(caclki^caclkq), .d(2'h3), .serial_in(CAcode), .q(dlyline_code));
    assign ditherCAcode = (dither) ? dlyline_code[1] : dlyline_code[0];
    edgetopulse epochpulsetoedge(.clk(clk), .sig(epoch), .pe(epochedge));
    edgetopulse2 dataclkresetpulsetoedge(.clk(clk), .sig(dataclkrst), .pe(dataclkrstedge));
    counter #(20) data_ctr(.clk(clk), .rst(dataclkrstedge), .en(epochedge), .cnt(), .co(dataclk));
endmodule
