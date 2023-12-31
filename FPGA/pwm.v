module pwm # (
    parameter WIDTH = 12
)(
    input wire clk, rst,
    input wire [WIDTH - 1:0] data,
    output reg pwm
);
    reg[WIDTH - 1:0] cnt;
    always @(posedge clk) begin
        if(rst) begin
            cnt <= 0;
        end
        else begin
            cnt <= cnt + 1'b1;
        end
    end
    always @(posedge clk) begin
        pwm <= (data > cnt);
    end
endmodule
