module edgetopulse (
    input wire clk, sig,
    output pe
);
    reg sigdly;
    always @(posedge clk) begin
	      sigdly <= sig;
	  end
    assign pe = sig & ~sigdly;
endmodule
