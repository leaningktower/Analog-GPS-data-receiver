module shiftreg #(
    parameter DW = 10
)(
    input wire clk, rst, shift,
    input wire [DW - 1 : 0] d,
    input wire serial_in,
    output reg [DW - 1 : 0] q
);
    always @(posedge clk) begin
        if(rst) q <= d;
        else if(shift) q <= {q, serial_in};
    end
endmodule
