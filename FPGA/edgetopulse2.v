module edgetopulse2 (
    input wire clk, sig,
    output pe
);
    reg [1:0] sigdly;
    always @(posedge clk) begin
	      sigdly <= {sigdly[0],sig};
	  end
    assign pe = sigdly[1] & ~sigdly[0];
endmodule
