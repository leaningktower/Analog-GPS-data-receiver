module counter #(
    parameter M = 1024
)(
    input wire clk, rst, en,
    output reg [$clog2(M) - 1 : 0] cnt,
    output co
);
    assign co = en & (cnt == M-1);
    always @(posedge clk) begin
        if(rst) cnt <= 0;
        else if(en) begin
            if(cnt < M - 1) cnt = cnt + 1'b1;
            else cnt <= 0;
        end
    end
endmodule
