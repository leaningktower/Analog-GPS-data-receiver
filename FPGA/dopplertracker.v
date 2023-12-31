module dopplertracker (
  input wire clk, rst,
  input wire [11:0] dopset,
  input wire [9:0] pwmset,
  input wire ifin, codetrack, doptrack,
  output reg [11:0] iffreq,
  output [9:0] pwmvalue,
  // output ifedge,
  output pwm
);
  // interface
  assign pwmvalue = daccounter;
  // main code
  wire [11:0] ifcnt;
  // edgetopulse ifpulsetoedge(.clk(clk), .sig(ifin), .pe(ifedge));
  counter #(122760)
  counter100 (.clk(clk), .rst(rst), .en(1'b1), .cnt(), .co(clk200)); //200 Hz scan rate
  counter #(8)
  counter25 (.clk(clk), .rst(rst), .en(clk200), .cnt(), .co(clk25)); //25 Hz track rate
  // counter #(4096)
  // counterif (.clk(clk), .rst(clk25), .en(ifedge), .cnt(ifcnt), .co()); //if frequency counter
  edgetopulse2 clk25pulsetoedge(.clk(clk), .sig(clk25), .pe(clk25edge));
  acounter #(4096)
  counterif (.clk(ifin), .arst(clk25edge), .en(1'b1), .cnt(ifcnt), .co());
  initial begin
      ifcomp <= 1'b1;
  end
  reg ifcomp;
  always @(posedge clk) begin
      if(clk25) begin
          iffreq <= ifcnt;
          if(ifcnt > dopset) begin
              ifcomp <= 1'b0;
          end
          else begin
              ifcomp <= 1'b1;
          end
      end
  end
  wire intclk, ud;
  assign intclk = (doptrack && codetrack) ? clk25 : clk200;
  assign ud = (doptrack && codetrack) ? ifcomp : 1'b1;
  intcounter #(64)
  dopplerintcounter (.clk(clk), .rst(rst), .en(intclk), .ud(ud), .add(add), .sub(sub)); //loop filter
  reg [9:0] daccounter;
  always @(posedge clk) begin
      if(rst) begin
          daccounter <= pwmset;
      end
      else begin
          if(add) begin
              daccounter <= daccounter + 1'b1;
          end
          else if(sub) begin
              daccounter <= daccounter - 1'b1;
          end
      end
  end
  pwm #(10)
  pwmdac (.clk(clk), .rst(rst), .data(daccounter), .pwm(pwm));
endmodule
