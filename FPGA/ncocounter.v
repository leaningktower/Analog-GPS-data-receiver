module ncocounter #(
    parameter M = 24
)(
    input wire clk, rst,
    input wire adv, ret,
    output cq, ci
);
    reg [M : 0] cnt;
    reg retg, advg;
    reg finish;
    always @(posedge clk) begin
        if(rst) begin
            cnt <= 1;
            finish <= 1'b0;
        end
        else begin
            if(advg && cnt[M - 1] == 1'b0) begin
                cnt <= {2'b0, cnt[M - 3 : 0], cnt[M - 2]};
                if (cnt[M - 2]) begin
                    finish <=1'b1;
                end
            end
            else if(retg) begin
                cnt <= {cnt, cnt[M]};
                if (cnt[M]) begin
                    finish <=1'b1;
                end
            end
            else begin
                cnt <= {1'b0, cnt[M - 2 : 0], cnt[M - 1]};
                if (finish && cnt[1]) begin
                    finish <= 1'b0;
                end
            end
        end
    end
    always @(posedge clk) begin
        if(rst || finish) begin
            retg <= 1'b0;
            advg <= 1'b0;
        end
        else if(ret) begin
            retg <= 1'b1;
        end
        else if(adv) begin
            advg <= 1'b1;
        end
    end
    assign cq = cnt[0];
    assign ci = cnt[M / 2];
endmodule
