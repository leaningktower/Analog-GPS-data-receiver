module CAtracker (
  input wire clk, rst,
  input wire codesign, codetrack,
  input wire dataclkrst,
  input wire [4:0] sat,
  output dither,
  output CAcode, ditherCAcode,
  output epoch, dataclk, ci,
  output codeNCOud,
  output reg data
);
counter #(384)
  counter64k (.clk(clk), .rst(rst), .en(1'b1), .cnt(), .co(clk64k)); //64k scan rate
counter #(767)
  counter32k (.clk(clk), .rst(rst), .en(1'b1), .cnt(), .co(clk32k)); //32k track rate
wire intclk, ud;
assign intclk = codetrack ? clk32k : clk64k;
assign codeNCOud = codesign ^ dither;
assign ud = codetrack ? codeNCOud : 1'b1;
intcounter #(256)
  codeintcounter (.clk(clk), .rst(rst), .en(intclk), .ud(ud), .add(add), .sub(sub)); //loop filter
ncocounter #(24)
  nco (.clk(clk), .rst(rst), .adv(add), .ret(sub), .cq(cq), .ci(ci)); //1.023 MHz nco
CAgenerator pagenerator(
  .clk(clk), .rst(rst), .caclki(ci), .caclkq(cq),
  .sat(sat),
  .dither(dither),
  .dataclkrst(dataclkrst),
  .CAcode(CAcode), .ditherCAcode(ditherCAcode),
  .epoch(epoch), .dataclk(dataclk)
  );
edgetopulse epochpulsetoedge(.clk(clk), .sig(epoch), .pe(epochedge));
reg [2 : 0] cnt;
always @(posedge clk) begin
    if(rst) cnt <= 0;
    else if(epochedge) begin
        if(cnt < 5) cnt = cnt + 1'b1;
        else cnt <= 0;
    end
end
assign dither = (cnt < 3);
edgetopulse2 dataclkresetpulsetoedge(.clk(clk), .sig(dataclkrst), .pe(dataclkrstedge));
always @(posedge clk) begin
    if(rst) data <= 0;
    else if(dataclkrstedge) begin
        data <= ~data;
    end
end
endmodule
