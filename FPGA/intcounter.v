module intcounter #(
    parameter M = 1024
)(
    input wire clk, rst, en, ud,
    output reg add, sub
);
    reg [$clog2(M) - 1 : 0] cnt;
    assign co = en & (cnt == M-1);
    always @(posedge clk) begin
        if(rst) cnt <= 0;
        else if(en) begin
            if(ud) begin
                if(cnt < M - 1) begin
                    cnt = cnt + 1'b1;
                    add <= 1'b0;
                    sub <= 1'b0;
                end
                else begin
                    cnt <= 0;
                    add <= 1'b1;
                    sub <= 1'b0;
                end
            end
            else begin
                if(cnt > 0) begin
                    cnt = cnt - 1'b1;
                    add <= 1'b0;
                    sub <= 1'b0;
                end
                else begin
                    cnt <= M - 1;
                    sub <= 1'b1;
                    add <= 1'b0;
                end
            end
        end
        else begin
            add <= 1'b0;
            sub <= 1'b0;
        end
    end
endmodule
